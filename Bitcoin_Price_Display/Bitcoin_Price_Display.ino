#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include "Arduino_LED_Matrix.h"
#include <LiquidCrystal.h>

//LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//WiFi credentials
char ssid[] = "YOUR_WIFI_NETWORK_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
const char* server = "api.blockchain.info"; //API where we are getting the Bitcoin price from

//HTTPS client
WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, server, 443);

//LED matrix instance
ArduinoLEDMatrix matrix;

//BTC letters bitmap 8 rows x 12 columns
byte btcBitmap[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,0,0,1,1,1,0,0,1,1},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {1,1,1,0,0,0,1,0,0,1,0,0},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {1,0,0,1,0,0,1,0,0,1,0,0},
  {1,1,1,0,0,0,1,0,0,0,1,1},
};

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, pass); //Enters above wifi network name and password
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1000);

  matrix.begin();
}

void loop() {
  client.get("/ticker");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  float price = 0;
  int pos = response.indexOf("\"USD\"");
  if (pos != -1) {
    int lastPos = response.indexOf("\"last\":", pos);
    if (lastPos != -1) {
      int start = response.indexOf(":", lastPos) + 1;
      int end   = response.indexOf(",", start);
      price = response.substring(start, end).toFloat();
    }
  }

  //LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bitcoin Price:");
  lcd.setCursor(0, 1);
  lcd.print("$");
  lcd.print(price, 2);

  //Displays "BTC" text on board LEDs
  matrix.renderBitmap(btcBitmap, 8, 12);

  delay(60000); //Updates price every 60 seconds
}
