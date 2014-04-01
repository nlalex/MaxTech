MaxTech - enCORE Setup
=======

##How to Use
* Download zip with button on right
* Extract folder
* Open Arduino IDE
* Within Arduino IDE, set working folder as *.../libraries*
  * File -> Preferences -> Sketchbook Location
* Open Arduino sketch within IDE
  * File -> Open
* Modify code as needed
  * Many settings found under *.../libraries/Node/Config_enCORE.h*
  * Many sub-routines found under *.../libraries/Node/Node.cpp*
  * *CurrentArduino* sketch needs *SPI*, *WiFi*, *XBee*, and *Node* libraries included (should be by default)
  
##File Structure
* **CurrentArduino** - Main Arduino sketch

* **TestHeaters** - Arduino sketch used for testing heater on/off actuation

* **TestHubTemp** - Arduino sketch used for reading and debugging temperature sensor problems

* **libraries** - Contains all Arduino support files

* **xctu_config** - XBee configuration files


