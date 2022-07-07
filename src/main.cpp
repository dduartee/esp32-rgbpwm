#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "...";
const char *password = "...";

WebServer server(80);

const int ledR = 16;
const int ledG = 17;
const int ledB = 4;
const int channelR = 0;
const int channelG = 1;
const int channelB = 2;
void handleLEDColor()
{
  if (server.method() == HTTP_POST)
  {
    int brightnessR = server.arg("brightnessR").toInt(); // 0 - 255
    int brightnessG = server.arg("brightnessG").toInt(); // 0 - 255
    int brightnessB = server.arg("brightnessB").toInt(); // 0 - 255
    ledcWrite(channelR, brightnessR);
    ledcWrite(channelG, brightnessG);
    ledcWrite(channelB, brightnessB);
  }
  server.send(200, "application/json", "{\"channelR\":\"" + String(ledcRead(channelR)) + "\",\"channelG\":\"" + String(ledcRead(channelG)) + "\",\"channelB\":\"" + String(ledcRead(channelB)) + "\"}");
}
void setup(void)
{
  ledcSetup(channelR, 5000, 8);
  ledcSetup(channelG, 5000, 8);
  ledcSetup(channelB, 5000, 8);
  ledcAttachPin(ledR, 0);
  ledcAttachPin(ledG, 1);
  ledcAttachPin(ledB, 2);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }
  server.enableCORS();
  server.on("/", []() {
    server.send(200, "text/html", "<h1>Hello ESP32!</h1>");
  });
  server.on("/led", handleLEDColor);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  delay(2); // allow the cpu to switch to other tasks
}
