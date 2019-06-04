/*******************Esp8266_Websocket.ino****************************************/

#include <ESP8266WiFi.h> 
#include <WebSocketClient.h> //remember to add library
#include <SocketIoClient.h>
#include "DHT.h"
#include <ArduinoJson.h>


#define DHTTYPE DHT11   // DHT 11 

const int DHTPin = 5; //change this pin
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

volatile unsigned long previousMillis2;
boolean handshakeFailed=0;
float data[2];

char path[] = "";   //identifier of this device

const char* ssid     = "enter your wifi ssid here";
const char* password = "enter your wifi password here";
char* host = "192.168.0.23";  //replace this ip address with the ip address (remember ipconfig)
const int espport= 3000;
  
SocketIoClient webSocketClient;
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval=300; //interval for sending data to the websocket server in ms

// Use WiFiClient class to create TCP connections
WiFiClient client;



void setup() {
  Serial.begin(115200); //depends on what you set it to  
  dht.begin();

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);//3000
  
wsconnect();
//  wifi_set_sleep_type(LIGHT_SLEEP_T);

void loop() {

  if (client.connected()) {
    currentMillis=millis(); 
    webSocketClient.getData(data); 
    
    if (data.length() > 0) {
      Serial.println(data);
      //*************send log data to server in certain interval************************************
      //currentMillis=millis();   
      if (abs(currentMillis - previousMillis) >= interval) {
        previousMillis = currentMillis;
           //***************************************SET DATA HERE******************************************
           data[0] = dht.readHumidity();
        
        webSocketClient.emit("updateData", data);
        }
      }
      else{
    }
    delay(5);

  }

//*********************************************************************************************************************
//***************function definitions**********************************************************************************
void wsconnect(){
  // Connect to the websocket server
  if (client.connect(host, espport)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
      delay(1000);  
   
   if(handshakeFailed){
    handshakeFailed=0;
    ESP.restart();
    }
    handshakeFailed=1;
  }

  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    
    Serial.println("Handshake failed.");
   delay(4000);  
   
   if(handshakeFailed){
    handshakeFailed=0;
    ESP.restart();
    }
    handshakeFailed=1;
  }
}
