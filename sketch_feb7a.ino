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

void setup()
{
  // Initilize hardware:
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // Connect to the WiFi network (see function below loop)
  connectToWiFi(networkName, networkPswd);

  //digitalWrite(LED_PIN, LOW); // LED off
  //Serial.print("Press button 0 to connect to ");
  //Serial.println(hostDomain);

  configTime(0, 0, ntpServer);

}

void loop()
{
  if (digitalRead(BUTTON_PIN) == LOW)
  { // Check if button has been pressed
    while (digitalRead(BUTTON_PIN) == LOW)
      ; // Wait for button to be released

    //digitalWrite(LED_PIN, HIGH); // Turn on LED
    //requestURL(hostDomain, hostPort); // Connect to server
    //digitalWrite(LED_PIN, LOW); // Turn off LED
    epochTime = getTime();
    char buff[40];
    sprintf(buff, "%lu", epochTime);

    Serial.println(buff);

    String encoded = base64::encode(buff);

    Serial.println(encoded);

    String message = "{\"messages\":[{\"data\": \"" + encoded + "\"}]}";

    Serial.println(message);

    HTTPClient http;
    http.begin(PUBSUB_POST_URL);
    http.addHeader("Authorization", PUBSUB_ACCESS_TOKEN);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "keep-alive");
    int httpResponseCode = http.POST(message.c_str());
    //int httpResponseCode = http.POST("{\"url\": \"https://www.jsulzsss.com\"}");
    //int httpResponseCode = http.GET();
    Serial.println(httpResponseCode);
    http.end();
  }
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

void requestURL(const char * host, uint8_t port)
{
  printLine();
  Serial.println("Connecting to domain: " + String(host));

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected!");
  printLine();

  // This will send the request to the server
  client.print((String)"GET / HTTP/1.1\r\n" +
               "Host: " + String(host) + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) 
  {
    if (millis() - timeout > 5000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) 
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}
