#include <rom/uart.h>

extern "C" {
  #include <driver/periph_ctrl.h>

  #include <driver/uart.h>
  #include <esp_bt.h>

  #include "esp_spiffs.h"
  #include "esp_log.h"
  #include <stdio.h>
  #include <sys/types.h>
  #include <dirent.h>
  #include "esp_partition.h"
}

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Arduino.h>
#include "util.h"
#include "dictionary/dictionary.h"
#include "web/web.h"
#include "../../shared/message.h"

#define SPI_BUFFER_LEN SPI_MAX_DMA_LEN

void initDebug(){
	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[1], 0);
	PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[3], 0);

	const char* default_uart_dev = "/dev/uart/0";
	_GLOBAL_REENT->_stdin  = fopen(default_uart_dev, "r");
	_GLOBAL_REENT->_stdout = fopen(default_uart_dev, "w");
	_GLOBAL_REENT->_stderr = fopen(default_uart_dev, "w");

	uart_div_modify(CONFIG_CONSOLE_UART_NUM, (APB_CLK_FREQ << 4) / 115200);

	// uartAttach();
	ets_install_uart_printf();
	uart_tx_switch(CONFIG_CONSOLE_UART_NUM);
}



SPIMsgCommand* spicmd;
SPIMsgResult* spiresp;

void eventStreamThread(void* params){
    while(1){
        memset(reinterpret_cast<unsigned char*>(spicmd),'B',sizeof(SPIMsgCommand));
        //ets_printf("NINA: Waiting for transfer");
        int rlen=SPIS.transfer(nullptr, reinterpret_cast<unsigned char*>(spicmd), SPI_BUFFER_LEN);

        /*
        ets_printf("NINA: RECEIVED ");
        printbuf(spicmd,rlen);
        ets_printf("\n");
        */

        if(rlen!=sizeof(SPIMsgCommand))continue;

        spiresp->type=spicmd->type;
        memset(spiresp->data.echo,0,100);

        switch(spicmd->type){
            case SPIMsgType::SETPAIR:
            //ets_printf("NINA: COMMAND SETPAIR %s -> %s\n",spicmd->data.setpair.key,spicmd->data.setpair.value);
            ets_printf("NINA: COMMAND SETPAIR\n");
            Web::updatePair(spicmd->data.setpair);
            break;

            case SPIMsgType::GETIP:
            ets_printf("NINA: COMMAND GETIP\n");
            spiresp->data.getip=Web::getIP();
            ets_printf("NINA: returning %d\n",spiresp->data.getip);
            break;

            case SPIMsgType::GETPAIR:
            //ets_printf("NINA: COMMAND GETPAIR\n");
            spiresp->data.getpair=Web::updatedDictionaryPair;
            memset(Web::updatedDictionaryPair.key,0,20);
            break;

            case SPIMsgType::ECHO:
            memcpy(spiresp->data.echo,spicmd->data.echo,100);
            break;

        }
        //ets_printf("NINA: CSA: %s\n",digitalRead(SPIS._csPin)?"high":"low");

        delay(1);
        //ets_printf("NINA: CSB: %s\n",digitalRead(SPIS._csPin)?"high":"low");

        /*
        ets_printf("NINA: SENDING ");
        printbuf(spiresp,sizeof(SPIMsgResult));
        ets_printf("\n");
        */
        
        int num=SPIS.transfer(reinterpret_cast<unsigned char*>(spiresp), nullptr, sizeof(SPIMsgResult));
        //ets_printf("NINA: SENT %d\n", num);

        delay(1);

    }
}



void setup(){

	initDebug();
    pinMode(33, OUTPUT);
    digitalWrite(33, HIGH);

    esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

    SPIS.begin();
    spicmd=reinterpret_cast<SPIMsgCommand*>(heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA));
    spiresp=reinterpret_cast<SPIMsgResult*>(heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA));



    Web::setupServer();

    Web::beginServer();

    xTaskCreatePinnedToCore(eventStreamThread, "eventStreamThread", 8192, NULL, 2, NULL, 0);

    ets_printf("NINA: start\n");
    ets_printf("NINA: SPIMsgCommand %d SPIMsgResult %d\n",sizeof(SPIMsgCommand),sizeof(SPIMsgResult));
}

void loop(){
    Web::processClients();
    delay(10);
}
