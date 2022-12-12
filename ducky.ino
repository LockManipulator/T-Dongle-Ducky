/*
Go to IP address on screen or 192.168.0.1 if in AP mode
First input will type out the string in the box
Second is to write code
Third is to save above code with supplied filename
Fourth is to run code with supplied filename
MUST save and run a file with a SLASH IN FRONT and EXTENSION AFTER i.e. /example.txt

PRINT: Types out the following string
PRINTLN: Types out the following string then hits enter
GUI: Presses and releases the left GUI key
GUI : Presses the left GUI and a single character i.e. GUI r
DELAY x: Waits x ms
ENTER: Hits enter
HOLD: Holds a single key. Can be used with numbers, lowercase letters, SPACE, BACKSPACE, ENTER, or TAB. Anything that doesn't
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

String chars[62] = {
  "a",
  "b",
  "c",
  "d",
  "e",
  "f",
  "g",
  "h",
  "i",
  "j",
  "k",
  "l",
  "m",
  "n",
  "o",
  "p",
  "q",
  "r",
  "s",
  "t",
  "u",
  "v",
  "w",
  "x",
  "y",
  "z",
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "SPACE",
  "BACKSPACE",
  "TAB",
  "ENTER"
};

#define SHIFT 0x80
const uint8_t char_bytes[62]{
  0x04,          // a
  0x05,          // b
  0x06,          // c
  0x07,          // d
  0x08,          // e
  0x09,          // f
  0x0a,          // g
  0x0b,          // h
  0x0c,          // i
  0x0d,          // j
  0x0e,          // k
  0x0f,          // l
  0x10,          // m
  0x11,          // n
  0x12,          // o
  0x13,          // p
  0x14,          // q
  0x15,          // r
  0x16,          // s
  0x17,          // t
  0x18,          // u
  0x19,          // v
  0x1a,          // w
  0x1b,          // x
  0x1c,          // y
  0x1d,          // z
  0x27,          // 0
  0x1e,          // 1
  0x1f,          // 2
  0x20,          // 3
  0x21,          // 4
  0x22,          // 5
  0x23,          // 6
  0x24,          // 7
  0x25,          // 8
  0x26,          // 9
  0x2c,          // SPACE
  0x2a,          // BACKSPACE
  0x2b,          // TAB
  0x28,          // ENTER
};

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
  ptr += "Filename: <input type=\"text\" id=\"filerun\" name=\"filerun\">";
  ptr += "<input type=\"submit\" value=\"Run from file\">";
  ptr+= "</form><br>";
  ptr += "<form action=\"/get\">";
  ptr += "<textarea id=\"code\" name=\"code\" id=\"code\" rows=\"10\" cols=\"30\"></textarea><br>";
  ptr += "Filename: <input type=\"text\" id=\"filesave\" name=\"filesave\">";
  ptr += "<input type=\"submit\" value=\"Save\">";
  ptr += "</form><br>";
  ptr += "<form action=\"/get\">";
  ptr += "Filename: <input type=\"text\" id=\"filedel\" name=\"filedel\">";
  ptr += "<input type=\"submit\" value=\"Delete\">";
  ptr += "</form><br>";
  ptr += "Files on SD<br>";
  ptr += "-----------<br>";
  ptr += listDir(SD_MMC, "/", 0);
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
  Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
}

void parse(String text) {
  char *txt = (char *)text.c_str();
  const char *delimiter = "\n";
  char* ptr = strtok(txt, delimiter);
  while(ptr != NULL) {
      String com = String(ptr);
      if (com.startsWith("GUI ")) {
        com.remove(0, 4);
        Keyboard.pressRaw(0xe3);
        for (int i=0; i<62; i++) {
          if (chars[i] == com){
            Keyboard.pressRaw(char_bytes[i]);
            break;
          }
        }
        Keyboard.releaseAll();
      }
      else if (com.startsWith("GUI")) {
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
      else if (com.startsWith("HOLD ")) {
        com.remove(0, 5);
        for (int i=0; i<62; i++) {
          if (chars[i] == com){
            Keyboard.pressRaw(char_bytes[i]);
            break;
          }
        }
      }
      else if (com.startsWith("RELEASE")) {
        Keyboard.releaseAll();
      }
      ptr = strtok(NULL, delimiter);
  }
}

String listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    String all_files = "";
    File root = fs.open(dirname);
    if(!root){
        return "None";
    }
    if(!root.isDirectory()){
        return "None";
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
          all_files += "/" + String(file.name()) + "<br>";
        }
        file = root.openNextFile();
    }
    return all_files;
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

void deleteFile(fs::FS &fs, const char * path){
    fs.remove(path);
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
    else if (request->hasParam("filedel")) {
      inputMessage = request->getParam("filedel")->value();
      inputParam = "filedel";
      char *txt = (char *)inputMessage.c_str();
      deleteFile(SD_MMC, txt);
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
