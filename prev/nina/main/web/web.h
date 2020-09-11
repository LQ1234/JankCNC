#include <SPIS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "/Users/larryqiu/wifi_pass.h"
#include "serve.h"

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

namespace Web{
    WiFiServer server(80);
    int status = WL_IDLE_STATUS;

    WiFiClient eventStreamClients[CONFIG_LWIP_MAX_SOCKETS];

    void setupServer() {;

        esp_vfs_spiffs_conf_t conf = {
          .base_path = "/fs",
          .partition_label = "storage",
          .max_files = 20,
          .format_if_mount_failed = true
        };

        esp_err_t ret = esp_vfs_spiffs_register(&conf);

        if (WiFi.status() == WL_NO_SHIELD) {
          while (1); // no shield
        }
    }


    void beginServer() {
        ets_printf("NINA: Connecting...\n");


        int status;
        do {
            status = WiFi.begin(ssid, pass);
            for(int i=0;i<50;i++){
                delay(100);
                status=WiFi.status();
                if((status!=WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED))break;
                ets_printf("NINA: try\n");

            }
            ets_printf("NINA: Fail %d\n", status);

        } while (status != WL_CONNECTED);

        server.begin();

        ets_printf("NINA: Connected\n");
    }


    void sendHeader(WiFiClient& client, int statusCode, const char* contentType, int length){
        client.write("HTTP/1.1 ");
        switch(statusCode){
            case 404:
            client.write("404 not found");
            break;
            default:
            client.write("200 OK");
        }
        client.write("\r\n");

        if(length>=0){
            client.write("Content-Length: ");
            client.write(length);
            client.write("\r\n");
        }

        client.write("Content-Type: ");
        client.write(contentType);
        client.write("\r\n");

        client.write("\r\n");
    }

    void sendData(WiFiClient& client, const unsigned char* str, int len){
        int chunk_len=512;
        int len_left= len;
        const unsigned char* ptr=str;

        while(len_left>0&&client.connected()){
            //ets_printf("ptr %d\n", ptr);

            int to_write=min(len_left,chunk_len);
            int written=client.write(ptr,to_write);
            ptr+=written;
            len_left-=written;
        }
    }
    void sendEvent(const char* eventname,const char* body){
        for(int i=0;i<CONFIG_LWIP_MAX_SOCKETS;i++){
            WiFiClient client=eventStreamClients[i];
            if(client.connected()){
                client.write("event: ");
                client.write(eventname);
                client.write("\n");
                client.write("data: ");
                client.write(body);
                client.write("\n");
                client.write("\n");
            }
        }
    }
    void processGETRequest(WiFiClient& client,const char* dir){
        if(strcmp(dir,"/eventstream")==0){
            sendHeader(client, 200, "text/event-stream", -1);
            for(int i=0;i<CONFIG_LWIP_MAX_SOCKETS;i++){
                if(!eventStreamClients[i].connected()){
                    eventStreamClients[i]=client;
                    break;
                }
            }
        }else{
            ResponseData resp=get_response(dir);
            sendHeader(client, resp.code, resp.mime, resp.length);

            sendData(client, resp.data, resp.length);

            client.stop();
        }
    }

    void processPOSTRequest(WiFiClient& client,const char* dir,int contLen){

    }

    void processRequest(WiFiClient& client) {
        char method[10+1];
        char dir[100+1];
        int contLen=-1;
        int ch,i;

        memset(method,0,sizeof(method));
        memset(dir,0,sizeof(dir));

        i=0;
        while(1){
            ch=client.read();
            if(ch==' '||ch==-1) break;
            else if(i==10);
            else method[i++]=ch;
        }

        /*
        ets_printf("Method: ");
        ets_printf(method);
        ets_printf("\n");
        */

        i=0;
        while(1){
            ch=client.read();
            if(ch==' '||ch==-1) break;
            else if(i==100);
            else dir[i++]=ch;
        }

        /*
        ets_printf("Dir: ");
        ets_printf(dir);
        ets_printf("\n");
        */

        i=0;
        while(1){
            ch=client.read();
            if(ch=='\n'||ch==-1) break;
        }

        while(1){
            char key[100+1];
            char val[100+1];
            memset(key,0,sizeof(key));
            memset(val,0,sizeof(val));

            i=0;
            while(1){
                ch=client.read();
                if(ch==':') {
                    client.read();
                    break;
                } else if(ch=='\n'||ch==-1) break;
                else if(i==100);
                else key[i++]=ch;
            }
            if(i==0)break;

            i=0;
            while(1){
                ch=client.read();
                if(ch=='\n'||ch==-1) break;
                else if(i==100);
                else val[i++]=ch;
            }

            /*
            ets_printf("Key: ");
            ets_printf(key);
            ets_printf("\n");

            ets_printf("Val: ");
            ets_printf(val);
            ets_printf("\n");
            */

            if(strcmp(key,"Content-Length")==0) contLen=atoi(val);
        }

        if(strcmp(method, "GET")==0){
            processGETRequest(client,dir);
        }else if(strcmp(method, "POST")==0){
            processPOSTRequest(client,dir,contLen);
        }
    }

    void processClients(){
        WiFiClient client = server.available();
        while (client) {
            ets_printf("Processing request...\n");
            processRequest(client);
            client = server.available();
            ets_printf("Processed request.\n");

        }
    }
}
