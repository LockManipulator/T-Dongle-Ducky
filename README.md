# T-Dongle-Ducky
LilyGo T-Dongle-S3 turned into a Rubber Ducky with wifi  

https://imgur.com/a/RgEbP6n

Board should be ESP32-S3-USB-OTG in Arduino IDE  
Use libraries from https://github.com/Xinyuan-LilyGO/T-Dongle-S3 under /lib if available  

# Usage

Go to IP address on screen or 192.168.0.1 if in AP mode  
First input will type out the string in the box  
Second is to write code  
Third is to save above code with supplied filename  
Fourth is to run code with supplied filename  
MUST save and run a file with a SLASH IN FRONT and EXTENSION AFTER i.e. "/example.txt"  

# Commands 
PRINT:  Types out the following string  
PRINTLN: Types out the following string then hits enter  
GUI: Presses and releases the left GUI key  
GUI <character>: Presses the left GUI and a single character i.e. GUI r
DELAY x: Waits x ms  
ENTER: Hits enter  
Anything that doesn't match the above gets treated as a comment. Comments can not, at the time, be put on the same line as a command.  
Please don't use trailing spaces in your code. I know Python not c++ so my input sanitization here is not great.  

# Example Rickroll
GUI  
DELAY 500  
PRINTLN web  
DELAY 1000  
PRINTLN www.youtube.com/watch?v=dQw4w9WgXcQ  
