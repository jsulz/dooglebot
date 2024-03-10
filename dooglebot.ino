#include <WiFi.h>
#include "Sensitive.h"
#include "time.h"
#include <HTTPClient.h>
#include <base64.h>

// WiFi network name and password:
const char * networkName = WIFI_NAME;
const char * networkPswd = WIFI_PASSWORD;

// Internet domain to request from:
const char * hostDomain = "example.com";
const int hostPort = 80;

const char * ntpServer = "pool.ntp.org";
unsigned long epochTime;

const int BUTTON_PIN = 0;
const int LED_PIN = 13;
const int PIR_DOUT = 3;
int SEND = 0;

void setup()
{
  // Initilize hardware:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_DOUT, INPUT);

  // Connect to the WiFi network (see function below loop)
  connectToWiFi(networkName, networkPswd);

  //digitalWrite(LED_PIN, LOW); // LED off
  //Serial.print("Press button 0 to connect to ");
  //Serial.println(hostDomain);

  configTime(0, 0, ntpServer);

}

void loop()
{
  checkAndConnect();
  readDigitalValue(); 

}

// Make sure we're connected to the wifi, and if we're not, hammer away, spinning on a relatively long delay
void checkAndConnect()
{

  if(Wifi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED) 
    {
      // Blink LED while we're connecting:
      digitalWrite(LED_PIN, ledState);
      ledState = (ledState + 1) % 2; // Flip ledState
      delay(5000);
      Serial.print(".");
    }
  }

}

void readDigitalValue()
{
  // The OpenPIR's digital output is active high
  int motionStatus = digitalRead(PIR_DOUT);

  // If motion is detected, turn the onboard LED on:
  if (motionStatus == HIGH){
    //Serial.println("High");
    if (SEND == 0){
      epochTime = getTime();
      String strEpoch = timeToString(epochTime);

      String encoded = encodeTime(strEpoch);

      String message = buildMessage(encoded);

      int httpResponse = sendPOST(message);

      Serial.println(httpResponse);

      SEND = 1;
    }
  }
  else{ // Otherwise turn the LED off:{
    //Serial.println("Low");
    SEND = 0;
  }
}

int sendPOST(String messageBody) {
  char url[80];
  strcpy(url, HOST);
  strcat(url, REST_ENDPOINT);
  HTTPClient http;
  http.setTimeout(5000);
  http.begin(url);
  http.addHeader("Authorization", REST_ACCESS_TOKEN);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "keep-alive");
  int httpResponseCode = http.POST(messageBody.c_str());
  //int httpResponseCode = http.GET();
  http.end();
  return httpResponseCode;
}

String buildMessage(String encoded) {
  String message = "{\"messages\":{\"data\": \"" + encoded + "\"}}";
  Serial.println(message);
  return message;
}

String encodeTime(String time) {
 String encoded = base64::encode(time);
 return encoded;
}

String timeToString(unsigned long epochTime) {

    char buff[40];
    sprintf(buff, "%lu", epochTime);
    return buff;

}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  int ledState = 0;

  printLine();
  Serial.println("Connecting to WiFi network: " + String(ssid));

  WiFi.begin(ssid, pwd);

  while (WiFi.status() != WL_CONNECTED) 
  {
    // Blink LED while we're connecting:
    digitalWrite(LED_PIN, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}
