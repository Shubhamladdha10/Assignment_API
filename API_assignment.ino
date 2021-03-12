#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif


const char* ssid = "JioFiber-cNS58";
const char* password = "saisadan123";

// define task
void TaskAPI( void *pvParameters );


void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  while( WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi Network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  
//Setting up task using freeRTOS
  xTaskCreatePinnedToCore(
    TaskAPI
    ,  "API"  
    ,  1000 //Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL 
    ,  ARDUINO_RUNNING_CORE);
}// Task scheduller started

void loop()
{}

void TaskAPI(void *pvParameters)
{
  (void) pvParameters;
  {
    if((WiFi.status() == WL_CONNECTED))
      {
      long rndm = random(1, 10);
      HTTPClient client;

      client.begin("https://fastag-internal.parkzap.com/account/mockable_test/" + String(rndm));
      int httpCode = client.GET();
      String payload;
              if(httpCode > 0)
              {
                payload = client.getString();
                Serial.println(payload);
        
                client.end();
              }
              else
              {
                Serial.println("Error in API request");
              }     
      char json[1000];
      payload.toCharArray(json, 1000);
      // Serial.println(json); // data in payload is converted to char array
      DynamicJsonDocument doc(8192);
      serializeJson(doc, json);
      JsonObject obj = doc.to<JsonObject>();
      Serial.println(obj);
      Serial.println(obj.size());
       
      
      String key;
      Serial.println("Enter the key you want to search: ");
      while (Serial.available()==0)   
      {
        while (Serial.available()==0){}
        key = Serial.readString();
        
        long startTime = millis();         
        Serial.println(key);
        Serial.print(" ");
        Serial.print(obj.containsKey(key));
        Serial.print(" - ");
        const char* value = obj[key];
        Serial.print(value);
        
        long elapsedTime =   millis() - startTime; 
        Serial.println(" Time for query: "); 
        Serial.print( (long )(elapsedTime)); 
        Serial.println(" msec "); 

      } 
      
    WiFi.disconnect();
    }
  }  
}
