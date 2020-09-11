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
#include "web/web.h"


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

bool sendEventFlag=false;
char* samdInBuf;
char* samdOutBuf;
void eventStreamThread(void* params){
    while(1){
        sendEventFlag=true;

        memset(samdInBuf,'B',21);
        int rlen=SPIS.transfer(NULL, (unsigned char*)samdInBuf, SPI_BUFFER_LEN);
        if(rlen==0)continue;
        ets_printf("NINA: RECEIVED\n");
        ets_printf("NINA (len %d): ",rlen);
        samdInBuf[20]=0;
        ets_printf(samdInBuf);
        ets_printf("\n");

        delay(50);

        ets_printf("NINA: SENDING\n");
        int num=SPIS.transfer((unsigned char*)samdOutBuf, (unsigned char*)samdInBuf, strlen(samdOutBuf));
        ets_printf("NINA: SENT %d\n",num);
        /*
        ets_printf("NINA: ALSO RECIEVED\n");
        ets_printf("NINA:");
        samdInBuf[20]=0;
        ets_printf(samdInBuf);
        ets_printf("\n");*/
    }

}



void setup(){

	initDebug();
    pinMode(33, OUTPUT);
    digitalWrite(33, HIGH);

    esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);

    SPIS.begin();
    samdInBuf=(char*)heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA);
    samdOutBuf=(char*)heap_caps_malloc(SPI_BUFFER_LEN, MALLOC_CAP_DMA);
    strcpy(samdOutBuf,"message from ninaPAD");



    Web::setupServer();

    Web::beginServer();

    xTaskCreatePinnedToCore(eventStreamThread, "eventStreamThread", 8192, NULL, 2, NULL, 0);

    ets_printf("NINA: start\n");
}

void loop(){
    Web::processClients();
    if(sendEventFlag){
        Web::sendEvent("test","fred");
        sendEventFlag=false;

    }
    delay(10);
}
