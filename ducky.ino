/*
Board should be ESP32-S3-USB-OTG
Use libraries from https://github.com/Xinyuan-LilyGO/T-Dongle-S3 under /lib if available
Go to IP address on screen or 192.168.0.1 if in AP mode
First input will type out the string in the box
Second is to save a file to USB. 
Third is to run a file
MUST save and run a file with a slash in front and extension after i.e. "/example.txt"
PRINT this is a string - types out the following string
PRINTLN also string but hits enter - types out the following string then hits enter
GUI - presses and releases the left GUI key
DELAY 1000 - waits x ms
ENTER - hits enter
*/
#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else

#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "SD_MMC.h"
#include "FS.h"
#include "pin_config.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "TFT_eSPI.h"

USBHIDKeyboard Keyboard;
TFT_eSPI tft = TFT_eSPI();

//Button handling
const int buttonPin = 0;          // input pin for pushbutton
int previousButtonState = HIGH;   // for checking the state of a pushButton
int counter = 0;                  // button push counter

//AP handling
bool apmode = false;       // Change for ap
const char* ssidap     = "Iphone13_Hotspot";
const char* passwordap = NULL;
const char* ssidnet = "WifiName"; // Change to network name you want to connect to
const char* passwordnet = "WifiPassword"; // Change to network password
IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

//Server handling
AsyncWebServer server(80);
String header;

#define PRINT_STR(str, x, y)                                                                                                                         \
  do {                                                                                                                                               \
    tft.drawString(str, x, y);                                                                                                                       \
    y = y + 8;                                                                                                                                          \
  } while (0);

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<title>ESP Input Form</title>";
  ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  ptr += "</head><body>";
  ptr += "<form action=\"/get\">";
  ptr += "String: <input type=\"text\" id=\"input1\" name=\"input1\">";
  ptr += "<input type=\"submit\" value=\"Submit\">";
  ptr += "</form><br>";
  ptr += "<form action=\"/get\">";
  ptr += "<textarea id=\"code\" name=\"code\" id=\"code\" rows=\"10\" cols=\"30\"></textarea><br>";
  ptr += "Filename: <input type=\"text\" id=\"filesave\" name=\"filesave\">";
  ptr += "<input type=\"submit\" value=\"Save\">";
  ptr += "</form><br>";
  ptr += "<form action=\"/get\">";
  ptr += "Filename: <input type=\"text\" id=\"filerun\" name=\"filerun\">";
  ptr += "<input type=\"submit\" value=\"Run from file\">";
  ptr += "</body></html>";
  return ptr;
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void typeout(String text) {
  Keyboard.println(text);
}

void RickRoll() {
  Keyboard.pressRaw(0xe3);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.println("web");
  delay(1000);
  if (!apmode) {
    Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
  }
}

void parse(String text) {
  char *txt = (char *)text.c_str();
  const char *delimiter = "\n";
  char* ptr = strtok(txt, delimiter);
  while(ptr != NULL) {
      String com = String(ptr);
      if (com.startsWith("GUI")) {
        Keyboard.pressRaw(0xe3);
        Keyboard.releaseAll();
      }
      else if (com.startsWith("ENTER")) {
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
      }
      else if (com.startsWith("DELAY ")) {
        com.remove(0, 6);
        delay(com.toInt());
      }
      else if (com.startsWith("PRINT ")) {
        com.remove(0, 6);
        Keyboard.print(com);
      }
      else if (com.startsWith("PRINTLN ")) {
        com.remove(0, 8);
        Keyboard.print(com);
        Keyboard.press(KEY_RETURN);
        Keyboard.releaseAll();
      }
      ptr = strtok(NULL, delimiter);
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    File file = fs.open(path);
    if(!file){
        return;
    }
    String line = "";
    while(file.available()){
        line = line + (char)file.read();
    }
    parse(line);
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        return;
    }
    file.print(message);
}

void setup() {
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialise TFT
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_LEDA_PIN, 0);
  tft.setTextFont(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Connect to Wi-Fi
  if (apmode) {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssidap, passwordap);
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssidnet, passwordnet);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  }
  
  // initialize control over the keyboard:
  Keyboard.begin();
  USB.begin();

  // initialize sd card
  int32_t x, y;
  x = 0;
  y = 0;
  SD_MMC.setPins(SD_MMC_CLK_PIN, SD_MMC_CMD_PIN, SD_MMC_D0_PIN, SD_MMC_D1_PIN, SD_MMC_D2_PIN, SD_MMC_D3_PIN);
  if (!SD_MMC.begin()) {
    PRINT_STR("Card Mount Failed", x, y)
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    PRINT_STR("No SD_MMC card attached", x, y)    
  }
  
  //Add text to screen
  PRINT_STR("Perfmorming network", x, y);
  PRINT_STR("diagnostics...", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("Do not remove", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR(WiFi.localIP().toString(), x, y);
  
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", (char *)SendHTML().c_str());
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputFile;

    if (request->hasParam("input1")) {
      inputMessage = request->getParam("input1")->value();
      inputParam = "input1";
      typeout(inputMessage);
    }
    else if (request->hasParam("button1")) {
      inputMessage = request->getParam("button1")->value();
      inputParam = "button1";
      RickRoll();
    }
    else if (request->hasParam("code")) {
      inputMessage = request->getParam("code")->value();
      inputFile = request->getParam("filesave")->value();
      inputParam = "code";
      char *txt = (char *)inputMessage.c_str();
      char *fle = (char *)inputFile.c_str();
      writeFile(SD_MMC, fle, txt);
    }
    else if (request->hasParam("filerun")) {
      inputMessage = request->getParam("filerun")->value();
      inputParam = "filerun";
      char *txt = (char *)inputMessage.c_str();
      readFile(SD_MMC, txt);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }

  request->send_P(200, "text/html", (char *)SendHTML().c_str());
  /*
  request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                   + inputParam + ") with value: " + inputMessage +
                                   "<br><a href=\"/\">Return to Home Page</a>");
  */
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  // read the pushbutton:
  int buttonState = digitalRead(buttonPin);
  // if the button state has changed,
  if ((buttonState != previousButtonState)
      // and it's currently pressed:
      && (buttonState == LOW)) {
    // increment the button counter
    counter++;
    // Write code below to run when button is pressed
    
    
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
}
#endif /* ARDUINO_USB_MODE */
