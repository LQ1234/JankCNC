#pragma once
#include <SPI.h>
#include <WiFiNINA.h>
#include "/Users/larryqiu/wifi_pass.h"
#include "serve.h"



namespace Web{
    WiFiServer server(80);

    using TimerTimeout::TimeoutInterval;
    using TimerTimeout::setTimeout;
    using TimerTimeout::setInterval;

    void async_blink(TimeoutInterval* ti){
        digitalWrite(LED_BUILTIN, HIGH);
        setTimeout([](TimeoutInterval* ti){
            digitalWrite(LED_BUILTIN, LOW);
        },200);
    }

    int status = WL_IDLE_STATUS;

    void initServer() {

        TimeoutInterval* blink=setInterval(async_blink,400);
        LCD::write("Connecting...");

        status = WiFi.begin(ssid, pass);

        while (status != WL_CONNECTED) {
            Serial.println("Bruh ");
            status = WiFi.begin(ssid, pass);
        };
        blink->clear();
        server.begin();

        IPAddress myAddress = WiFi.localIP();
        Serial.print("Connected with IP: ");
        Serial.println(myAddress);
        LCD::clear();
        LCD::write("IP: ");
        LCD::lcd.setCursor(0, 1);
        myAddress.printTo(LCD::lcd);
    }
    void begin(){
        //TimerTimeout::begin must have been run at this time
        initServer();
    }


    template<typename ReadAble>
    String next(ReadAble readable) {
        String res = "";
        int nc = readable.read();
        while (nc != '\n' && nc != ' ') {
        if (nc != '\r' && nc != -1)res += (char)nc;
         nc = readable.read();
        }
        return res;
    }
    template<typename ReadAble>
    String nextColon(ReadAble readable) {
        String res = "";
        int nc = readable.read();
        while (nc != '\n' && nc != ':') {
            if (nc != '\r' && nc != -1)res += (char)nc;
            nc = readable.read();
        }
        return res;
    }
    template<typename ReadAble>
    String nextLine(ReadAble readable) {
        String res = "";
        int nc = readable.read();
        while (nc != '\n') {
        if (nc != '\r' && nc != -1)res += (char)nc;
            nc = readable.read();
        }
        return res;
    }
    template<typename ReadAble>
    String readAll(ReadAble readable) {
        String res = "";
        int nc = readable.read();
        while (true) {
            if (nc != '\r' && nc != -1)res += (char)nc;
            nc = readable.read();
        }
        return res;
    }


    void sendResponse(WiFiClient& client, int statusCode, String contentType, String content){ // phase out
        String statusString;
        switch(statusCode){
            case 404:
            statusString="404 not found";
            break;
            default:
            statusString="200 OK";
        }
        String resTemplate=
        String()+
        "HTTP/1.1 "+statusString+"\r\n"
        "Content-Length: "+String(content.length())+"\r\n"
        "Content-Type: "+contentType+"\r\n"
        "\r\n"+
        content;
        client.write(resTemplate.c_str(),resTemplate.length());
    }


    void sendHeader(WiFiClient& client, int statusCode, char* contentType, int length){
        client.write("HTTP/1.1 ");
        switch(statusCode){
            case 404:
            client.write("404 not found");
            break;
            case 500:
            client.write("500 Internal Server Error");
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

    void handlePOSTRequest(WiFiClient& client, String dir, String body){
        if(dir=="/"){
            sendResponse(client, 200, "text/html", "POSTED \r\nu sent: \r\n");
        }else{
            sendResponse(client, 404, "text/html", "404 not found");
        }
        client.stop();
    }

    WiFiClient eventStreamClient;
    bool isEventStreamClientConnected(){
        if(!eventStreamClient)return(false);

        if(eventStreamClient.connected()){
            while(eventStreamClient.read()!=-1);
            return(eventStreamClient.connected());
        }
        return(false);
    }
    void handleEventStreamRequest(WiFiClient& client){
        //Serial.println("Handling event stream request...");
        sendHeader(client, 200, "text/event-stream", -1);
        eventStreamClient=client;
    }

    void sendEvent(const char* eventname,const char* body){
        //Serial.println("isEventStreamClientConnected2");
        if(!isEventStreamClientConnected())return;
        server.write("event: ");
        server.write(eventname);
        server.write("\n");
        server.write("data: ");
        server.write(body);
        server.write("\n");
        server.write("\n");

    }

    void handleGETRequest(WiFiClient& client, String dir){
        if(dir=="/eventstream")return(handleEventStreamRequest(client));
        ResponseData resp=get_response(reinterpret_cast<const char*>(dir.c_str()));
        Serial.println("sending");
        sendHeader(client, resp.code, resp.mime, resp.length);

        int chunk_len=128;
        int len_left= resp.length;
        const unsigned char* ptr=resp.data;

        while(len_left>0){
            int to_write=min(len_left,chunk_len);
            client.write(ptr,to_write);
            ptr+=to_write;
            len_left-=to_write;
        }

        client.stop();
        Serial.println("req done");

    }

    void processRequest(WiFiClient& client) {
        String method = next(client);
        String dir = next(client);

        String headerkey;
        String headervalue;
        int body_size = 0;
        while (true) {
            headerkey = nextColon(client);
            if (headerkey == "") {
                break;
            }
            headervalue = nextLine(client);
            if (headerkey == "Content-Length") {
                body_size = headervalue.toInt();
            }
            /*
            Serial.print("key: '");
            Serial.print(headerkey);
            Serial.print("' value: '");
            Serial.print(headervalue);
            Serial.println("'");
            */
        }


        unsigned char * body=new char unsigned[body_size+1];
        body[body_size]=0;
        client.read(body, body_size);
        if(method=="GET"){
            handleGETRequest(client,dir);
        }else if(method=="POST"){
            handlePOSTRequest(client,dir,String(reinterpret_cast<char *>(body)));
        }
    }

    void checkForClient(){
        if(isEventStreamClientConnected())return;

        WiFiClient client = server.available();
        while (client) {
            Serial.print("client port:");
            Serial.println(client.remotePort());
            processRequest(client);
            client = server.available();
        }
    }
}
