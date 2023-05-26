# T-Dongle-Ducky
LilyGo T-Dongle-S3 turned into a Rubber Ducky with wifi, controllable through a web server

ducky.ino             |  pretty_ducky.ino
:-------------------------:|:---------------:
![alt text](https://i.imgur.com/A33v6q3.png)  | ![screenshot](https://i.imgur.com/9GkC9Se_d.webp?maxwidth=450&fidelity=grand)


Board should be ESP32-S3-USB-OTG in Arduino IDE.  
Use libraries from https://github.com/Xinyuan-LilyGO/T-Dongle-S3 under /lib if available.  
Change wifi/access point name/password in code to what you need.  
Tested on Linux but most keyboard codes should work in Windows too. Unsure about apple products unless someone wants to donate one.  

# Usage

Go to IP address on screen or 192.168.0.1 if in AP mode  
First input will type out the string in the box  
Second is to run a single command  
Third is to write code  
Fourth is to save above code  
Fift is to run code  
Sixth is to delete a file  
MUST save/run/delete a file with a SLASH IN FRONT and EXTENSION AFTER i.e. /example.txt  

# Commands 
PRINT: Types out the following string  
PRINTLN: Types out the following string then hits enter  
GUI: Presses and releases the left GUI key  
GUI : Presses the left GUI and a single character. Single character can be a number or lowercase letter.  
DELAY x: Waits x ms  
ENTER: Hits enter  
HOLD: Holds a single key. If letter, then it has to be lowercase.  
Anything that doesn't match the above gets treated as a comment. Comments can not, at the time, be put on the same line as a command.  

# Non-alphanumeric Keys
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

# Defaults
Access point web page IP: 192.168.0.1  
Access point SSID: Iphone13_Hotspot  
Web login user: root  
Web login password: toor  

# Example Rickroll
GUI  
DELAY 500  
PRINTLN web  
DELAY 1000  
PRINTLN www.youtube.com/watch?v=dQw4w9WgXcQ  

# Roadmap  
- Read keyboard led states  
- Add bluetooth command and control capabilities
- Add mouse control  
- Mount as USB storage device  
- Built in method to get USB storage drive
- Shell through USB port controllable through web server  
- OS detection  
- Preloaded scripts to automatically grab usernames and hashes, network credentials, chrome/firefox passwords, rick roll, etc. 
