#include <SPI.h>
#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"
#include "arduino_secrets.h"
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include <string.h>
#define COLORED     0
#define UNCOLORED   1

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverAddress[] = "image2eink.herokuapp.com";  // server address
int port = 80;
WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

void setup() {

  
//  // put your setup code here, to run once:
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  
 if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    }
     Serial.println("Connected to Wifi");
     Serial.println();
    printWiFiStatus();

  delay(3000);
  Serial.println("---------------");
   Serial.println("Making GET Request...");
   Serial.println("");
    delay(3000);

   Serial.println("Receiving Data from Server...");
   Serial.println("");
    delay(3000);
  client.get("/getData");

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

unsigned int str_len = response.length()+1;
char charArray[str_len];
response.toCharArray(charArray, str_len);

uint8_t byteArray[response.length()+1];
int i=0;

// split charArray at each ',' and convert to uint8_t
char *p = strtok(charArray, ",");
while(p != NULL) {
  byteArray[i++] = strtoul(p, NULL, 16);
  p = strtok(NULL, ",");
}

//// Print the byteArray
//for (i=0; i<sizeof(byteArray)/sizeof(char); i++) {
//  Serial.print(byteArray[i]);
//  Serial.println(", ");
//}
Serial.println("---------------");

Serial.println("Updating Display ...");
   delay(3000);
  updateDisplay((char*)((void*)byteArray));

}

void loop() {

}

void updateDisplay(char* arr){
  Epd epd;

  if (epd.Init() != 0) {
    Serial.print("e-Paper init failed");
    return;
  }
  epd.ClearFrame();
  epd.SetPartialWindow(arr, 170, 100, 150, 100);
  epd.DisplayFrame();

  Serial.println("---------------");
  Serial.println("Display updated!");

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
