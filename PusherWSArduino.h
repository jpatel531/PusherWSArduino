//
//  PusherWsArduino.h
//  Websockets
//
//  Created by Jamie Patel on 22/04/2015.
//  Copyright (c) 2015 Jamie Patel. All rights reserved.
//

#include <WSClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <WSClient.h>

class Pusher {
private:
    EthernetClient *ethernet;
    WSClient ws;
    String host = "ws.pusherapp.com";
    String path;
    String url;
    static const String VERSION;
    static const String PROTOCOL;
    static const String AGENT;
    int port = 80;
    void connectViaEthernet();
public:
    String key;
    Pusher (String);
    bool connected;
    void connect(EthernetClient &client);
    void subscribe(String channel);
    const char* listen(String channel, String event);
    //    std::vector<PusherChannel> channels;
    int channelIndex = 0;
};

const String Pusher::VERSION = "0.0.1";
const String Pusher::PROTOCOL = "7";
const String Pusher::AGENT = "pusher-ws-arduino";

const char* SUBSCRIPTION_SUCCEEDED = "pusher_internal:subscription_succeeded";
const char* CONNECTION_ESTABLISHED ="pusher:connection_established";

Pusher::Pusher(String k) : key(k) {
    path = "/app/" + key+ "?client="+ AGENT+ "&version="+VERSION+"&protocol=" + PROTOCOL;
    url = host + path;
}


void Pusher::connectViaEthernet(){
    Serial.println(F("connecting.."));
    if (ethernet->connect(host.c_str(), port)) {
        Serial.println(ethernet->connected());
        Serial.println(F("Connected"));
    }
    else {
        Serial.println(F("Connection failed."));
        delay(2000);
        connectViaEthernet();
    }
}


void Pusher::connect(EthernetClient &client){
    ethernet = &client;
    connectViaEthernet();
    
    delay(1000);
    
    ws.path = &path[0];
    ws.host = &host[0];
    
    Serial.flush();
    if (ws.handshake(client)) {
        Serial.println(F("Handshake successful"));
    }
    else {
        Serial.println(F("Handshake failed."));
        while(1) {
            // Hang on failure
        }
    }
    
    
}

void Pusher::subscribe(String channelName)
{
    
    StaticJsonBuffer<200> jsonBuffer;
    
    JsonObject& root = jsonBuffer.createObject();
    root["event"] = "pusher:subscribe";
    JsonObject& data = root.createNestedObject("data");
    data["channel"] = channelName.c_str();
    
    char buffer[200];
    
    root.printTo(buffer, sizeof(buffer));
    
    ws.sendData(buffer);
    Serial.println(F("Subscribed"));
}




const char* Pusher::listen(String channel, String event){
    String data;
    data = ws.getData();
    
    if (data.length() > 0) {
        
        StaticJsonBuffer<200> jsonBuffer;
        
        JsonObject& root = jsonBuffer.parseObject(&data[0]);
        
        const char* channelName = root["channel"];
        const char* eventName = root["event"];
        const char* eventData = root["data"];
        
        if (!strcmp(&channel[0], channelName) && !strcmp(&event[0], eventName)){
            return eventData;
        }
        
        
    } else {
        return NULL;
    }
}


