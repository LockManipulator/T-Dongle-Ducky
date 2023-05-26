/*
USAGE
-----
Go to IP address on screen or 192.168.0.1 if in AP mode  
First input will type out the string in the box  
Second is to run a single command  
Third is to write code  
Fourth is to save above code  
Fift is to run code  
Sixth is to delete a file  
MUST save/run/delete a file with a SLASH IN FRONT and EXTENSION AFTER i.e. /example.txt  

COMMANDS
--------
PRINT: Types out the following string
PRINTLN: Types out the following string then hits enter
GUI: Presses and releases the left GUI key
GUI : Presses the left GUI and a single character. Single character can be a number or lowercase letter.
DELAY x: Waits x ms
ENTER: Hits enter
HOLD: Holds a single key. If letter, then it has to be lowercase.
Anything that doesn't match the above gets treated as a comment. Comments can not, at the time, be put on the same line as a command.

Non-alphanumeric Keys
---------------------
CTRL
ALT
SHIFT
DEL
RIGHT
LEFT
UP
DOWN
F1-F12
ENTER
TAB
BACKSPACE
SPACE
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
String loginname = "root"; // Login page username
String loginpassword = "toor"; // Login page password
IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
long randNumber;

//Server handling
AsyncWebServer server(80);
String header;

String chars[60] = {
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
  "ENTER",
  "CTRL",
  "ALT",
  "SHIFT",
  "DEL",
  "F1",
  "F2",
  "F3",
  "F4",
  "F5",
  "F6",
  "F7",
  "F8",
  "F9",
  "F10",
  "F11",
  "F12",
  "RIGHT",
  "LEFT",
  "DOWN",
  "UP"
};

const uint8_t char_bytes[60]{
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
  0xe0,          // CTRL (just here to keep lists the same size)
  0xe2,          // ALT (just here to keep lists the same size)
  0xe1,          // SHIFT (just here to keep lists the same size)
  0x4c,          // DEL
  0x3a,          // F1
  0x3b,          // F2
  0x3c,          // F3
  0x3d,          // F4
  0x3e,          // F5
  0x3f,          // F6
  0x40,          // F7
  0x41,          // F8
  0x42,          // F9
  0x43,          // F10
  0x44,          // F11
  0x45,          // F12
  0x4e,          // RIGHT
  0x4f,          // LEFT
  0x50,          // DOWN
  0x51,          // UP
};

#define PRINT_STR(str, x, y)                                                                                                                         \
  do {                                                                                                                                               \
    tft.drawString(str, x, y);                                                                                                                       \
    y = y + 8;                                                                                                                                          \
  } while (0);

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

String SendHTML(){
String ptr = "<!DOCTYPE html><html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">";
ptr += "<title>ESP Input Form</title>";
ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
ptr += '<style>body{margin:0;padding:0;background:linear-gradient(to bottom,purple,orange);color:#0f0;font-family:\"Courier New\",monospace;letter-spacing:1px;margin-top:2%}.container{display:flex;flex-direction:column;justify-content:center;align-items:center;height:100%}.panel{background-color:#222;padding:30px;border-radius:10px;max-width:600px;width:90%;margin-bottom:20px}.panel-heading{color:#0f0;font-size:24px;text-align:center;margin-bottom:20px}button{background-color:#0f0;color:#000;padding:10px 20px;border-radius:5px;border:none;cursor:pointer;font-family:\"Courier New\",monospace;letter-spacing:1px}button[type=\"submit\"]{width:100%}input[type=\"text\"],textarea{width:100%;padding:5px;border-radius:5px;border:1px solid #0f0;background-color:#000;color:#0f0;font-family:\"Courier New\",monospace;letter-spacing:1px}textarea{resize:vertical}label{color:#0f0;margin-bottom:5px;display:block}p{color:#0f0;font-size:14px;margin-bottom:10px}hr{border:none;height:1px;background-color:#0f0;margin:10px 0}.customize{margin-top:50px;padding:20px;background-color:#222;border-radius:10px}.customize h2{color:#0f0;font-size:18px;margin-bottom:10px}.customize label{color:#0f0;display:inline-block;margin-right:10px}.customize input[type=\"color\"]{-webkit-appearance:none;appearance:none;border:none;width:25px;height:25px;border-radius:50%;cursor:pointer}.customize input[type=\"range\"]{width:200px}.usage{background-color:#222;padding:20px;border-radius:10px;max-width:600px;width:90%;margin-bottom:20px}.usage h2{color:#0f0;font-size:20px;margin-bottom:10px}.usage p{color:#0f0;font-size:14px;margin-bottom:10px}.usage ul{padding-left:20px}@media only screen and (max-width: 600px){.container{margin:20px}.panel{padding:20px}.panel-heading{font-size:20px}.customize{margin-top:20px}}@media (max-width: 600px){.code-embed .ascii-font{display:none}}@media (min-width: 601px){.pc-hide{display:none}}</style>';
ptr += "</head><body><div class=\"container\"><div class=\"panel\"><h2 class=\"panel-heading pc-hide\">T-Dongle-Ducky</h2>";
ptr += "<div class=\"code-embed\"><pre><code class=\"ascii-font\" readonly style=\"background-color: #36393F; color: red;\">";
ptr += "    ___      ___                 _            ___            _        ";
ptr += "   |_ _|___ | . \\ ___ ._ _  ___ | | ___  ___ | . \\ _ _  ___ | |__ _ _ ";
ptr += "    | ||___|| | |/ . \\| ' |/ . || |/ ._>|___|| | || | |/ | '| / /| | |";
ptr += "    |_|     |___/\\___/|_|_|\\_. ||_|\\___.     |___/`___|\\_|_.|_\\_\\`_. |";
ptr += "                           &lt;___'                                 &lt;___'</code></pre></div>";
ptr += "<p>Created by: LockManipulator</p>";
ptr += "<p>Website refined: Chr0my</p></div>";

ptr += "<div class=\"panel\"><h2 class=\"panel-heading\">Inputs</h2><form action=\"/get\"><label for=\"input1\">String:</label><input type=\"text\" id=\"input1\" name=\"input1\"><br><br><button type=\"submit\">Send</button></form></div>";

ptr += "<div class=\"panel\"><h2 class=\"panel-heading\">Code Execution &amp; Files</h2><form action=\"/get\"><label for=\"input2\">Command:</label><input type=\"text\" id=\"input2\" name=\"input2\"><br><br><button type=\"submit\">Send</button></form><br><form action=\"/get\"><label for=\"code\">Code:</label><br><textarea id=\"code\" name=\"code\" rows=\"10\" cols=\"30\"></textarea><br><br><label for=\"filesave\">Filename:</label><input type=\"text\" id=\"filesave\" name=\"filesave\"><br><br><button type=\"submit\">Save</button></form><br><form action=\"/get\"><label for=\"filerun\">Filename:</label><input type=\"text\" id=\"filerun\" name=\"filerun\"><br><br><button type=\"submit\">Run from file</button></form><br><form action=\"/get\"><label for=\"filedel\">Filename:</label><input type=\"text\" id=\"filedel\" name=\"filedel\"><br><br><button type=\"submit\">Delete</button></form><br><p>Files on SD</p><hr>";
ptr += listDir(SD_MMC, "/", 0); 
ptr += "</div>";

ptr += "<div class=\"panel\"><h2 class=\"panel-heading\">Connectivity</h2><form action=\"/get\"><label for=\"wifiname\">Network:</label><input type=\"text\" id=\"wifiname\" name=\"wifiname\"><br><br><label for=\"wifipass\">Password:</label><input type=\"text\" id=\"wifipass\" name=\"wifinpass\"><br><br><button type=\"submit\">Connect</button></form><br><form action=\"/get\"><label for=\"apname\">Network:</label><input type=\"text\" id=\"apname\" name=\"apname\"><br><br><label for=\"appass\">Password:</label><input type=\"text\" id=\"appass\" name=\"appass\"><br><br><button type=\"submit\">Enable AP</button></form></div>";

ptr += "<div class=\"panel\"><h2>Usage</h2><p>Go to IP address on screen or 192.168.0.1 if in AP mode</p><p>First input will type out the string in the box</p><p>Second is to run a single command</p><p>Third is to write code</p><p>Fourth is to save above code</p><p>Fifth is to run code</p><p>Sixth is to delete a file</p><p>MUST save/run/delete a file with a SLASH IN FRONT and EXTENSION AFTER i.e. /example.txt</p></div></div></body></html>";

  return ptr;
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void typeout(String text) {
  Keyboard.print(text);
}

void RickRoll() {
  Keyboard.pressRaw(0xe3); // Left GUI
  Keyboard.releaseAll();
  delay(500);
  Keyboard.println("web");
  delay(1000);
  Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
}

void parse(String text) {
  int32_t x, y;
  x = 0;
  y = 0;
  char *txt = (char *)text.c_str();
  const char *delimiter = "\n";
  char* ptr = strtok(txt, delimiter);
  while(ptr != NULL) {
      String com = String(ptr);
      if (com.startsWith("GUI ")) {
        com.remove(0, 4);
        Keyboard.pressRaw(0xe3);
        for (int i=0; i<60; i++) {
          if (com.startsWith(chars[i])){
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
      else if (com.startsWith("DELAY ")) {
        com.remove(0, 6);
        delay(com.toInt());
      }
      else if (com.startsWith("HOLD ")) {
        com.remove(0, 5);
        for (int i=0; i<60; i++) {
          if (com.startsWith(chars[i])){
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

void add_ip_info(bool wifiap) {
  int32_t x, y;
  x = 0;
  y = 0;
  //Add text to screen
  tft.fillScreen(TFT_BLACK);
  PRINT_STR("Perfmorming network", x, y);
  PRINT_STR("diagnostics...", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("Do not remove", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  PRINT_STR("", x, y);
  if (wifiap) {
    PRINT_STR("192.168.0.1", x, y);
  }
  else {
    PRINT_STR(WiFi.localIP().toString(), x, y);
  }
}

void wifi_setup(const char* name, const char* pass) {
  if (apmode) {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    if (pass == "") {
      WiFi.softAP(name, NULL);
    }
    else {
      WiFi.softAP(name, pass);
      add_ip_info(true);
    }
  }
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(name, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
  }
}

void setup() {
  randomSeed(42069);
  randNumber = random(100000000, 999999999);
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
    wifi_setup(ssidap, passwordap);
  }
  else {
    wifi_setup(ssidnet, passwordnet);
  }

  add_ip_info(false);

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
  
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if(!request->authenticate((char *)loginname.c_str(), (char *)loginpassword.c_str()))
      return request->requestAuthentication();
    request->send_P(200, "text/html", (char *)SendHTML().c_str());
  });
/*
  server.on(secretpagechar(), HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", (char *)SendHTML().c_str());
  });
*/
  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    String inputFile;
    
    if(!request->authenticate((char *)loginname.c_str(), (char *)loginpassword.c_str()))
      return request->requestAuthentication();

    else if (request->hasParam("input1")) {
      inputMessage = request->getParam("input1")->value();
      inputParam = "input1";
      typeout(inputMessage);
    }
    else if (request->hasParam("input2")) {
      inputMessage = request->getParam("input2")->value();
      inputParam = "input2";
      parse(inputMessage);
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
    else if (request->hasParam("wifiname")) {
     ssidnet = (char *)request->getParam("wifiname")->value().c_str();
     passwordnet = (char *)request->getParam("wifipass")->value().c_str();
     apmode = false;
     wifi_setup(ssidnet, passwordnet);
     add_ip_info(false);
    }
    else if (request->hasParam("apname")) {
     ssidap = (const char *)request->getParam("apname")->value().c_str();
     passwordap = (const char *)request->getParam("appass")->value().c_str();
     apmode = true;
     wifi_setup(ssidap, passwordap);
     add_ip_info(true);
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
    RickRoll();
    
  }
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
}
#endif /* ARDUINO_USB_MODE */
