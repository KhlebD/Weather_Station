#include <wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char *ssid = "DanielDaniel";
const char *password = "super4dan";

WebServer server(80);
DHT dht(0, DHT22);
float readDHTTemperature();
float readDHTHumidity();
void handleRoot()
{
    char msg[1500];
    snprintf(msg, 1500,
             "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <title>Weather Station</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body style = 'background-color:#00add6;'>\
      <h2>Weather Station</h2>\
      <p>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>",
             readDHTTemperature(), readDHTHumidity());
    server.send(200, "text/html", msg);
}




void OLED_display(void *parameters)
{
    while (1)
    {
        float temp = readDHTTemperature();
        float hum = readDHTHumidity();
        display.clearDisplay();
        // display.setTextSize(1);
        // display.setTextColor(WHITE);
        display.setCursor(0, 10);
        display.printf("Temprature: %f\nHumidity: %f", temp, hum);
        display.display();
        vTaskDelay(4000);
    }
}
void setup()
{
    Serial.begin(115200);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.print(F("failed"));
        for (;;)
            ;
    }
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.printf("connecting");
    display.display();

    dht.begin();

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
    server.on("/", handleRoot);

    server.begin();
    Serial.println("HTTP server started");

    xTaskCreatePinnedToCore(OLED_display,
                            "display",
                            2048,
                            NULL,
                            1,
                            NULL,
                            1);
}
void loop()
{
    server.handleClient();
    delay(2); // allow the cpu to switch to other tasks
}

float readDHTTemperature()
{
    // Sensor readings may also be up to 2 seconds
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    if (isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return -1;
    }
    else
    {
        Serial.println(t);
        return t;
    }
}

float readDHTHumidity()
{
    // Sensor readings may also be up to 2 seconds
    float h = dht.readHumidity();
    if (isnan(h))
    {
        Serial.println("Failed to read from DHT sensor!");
        return -1;
    }
    else
    {
        Serial.println(h);
        return h;
    }
}