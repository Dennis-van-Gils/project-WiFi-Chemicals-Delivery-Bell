# WiFi Chemicals Delivery Bell

This project concerns an automated delivery notification system for when chemicals get delivered to our laboratory. Simply put, it is a box containing two huge arcade buttons which should get pressed by the deliverer: One for regular chemicals, and the other for temperature-critical (cooled) chemicals. All the deliverer has to do is push the appropriate button when having delivered chemicals. Once pushed, the buttons will light up and an email will be send to a configurable mailing list to inform everyone that chemicals are awaiting. There is also a web interface showing the current status, which you could check on your smartphone for instance. The buttons can be reset by pressing them again. This will revert the status to 'No chemicals awaiting' once more. 

The `Bell` is powered by a WiFi-enabled microprocessor board from Adafruit, namely an [Adafruit Feather HUZZAH ESP8266](https://www.adafruit.com/product/3046). This board sends out the button status over WiFi to a web server hosting the web interface. The web server also takes care of the emailing. An handy OLED screen on the `Bell` shows information regarding the WiFi connectivity, possible network errors, or when all is fine, an animation to inform the user about what it is doing. A 'flask' symbol indicates a happy idle state and an 'email' symbol indicates that an email will be send out within the coming minute. Repeatedly pressing the buttons will not cause massive email spam as there is this delay of one minute.

Detailed plans are included to build one yourself. The total parts cost of the enclosure plus electronics is around € 130. The shopping lists are included (see below). A small amount of straight-forward soldering is required. A few parts have to be 3D-printed, namely the OLED screen holder and the microcontroller board mounting plate (STL files included), but you could also [MacGyver](https://en.wikipedia.org/wiki/MacGyver) your own construction here.

I will gladly assist my colleagues at the University if they want to go live with the `Bell`. Setting up the web server is straight-forward. Simply copy the `web` folder to your server (presumably, the server that already is hosting your group's internet page) and edit the single configuration file `globals.php` to set up the email list and the hashed password used between the microcontroller and the server. The microcontroller itself needs to be flashed with firmware which has to be compiled. The source code for this is included in folder `source_mcu` and it has one configuration file you should edit: `source_mcu/src/wifi_settings_template.h`. If you send me your configuration file, I can also easily compile the firmware for you. 

Specific to the University of Twente: Each WiFi microcontroller has a unique MAC address which has to get registrered by LISA in order to get access to the Internet-of-Things network of the University. The microcontroller itself is firewalled by LISA from any incoming traffic.

![Photo](/docs/photos/01_chemicals_bell__front.jpg)
![Photo](/docs/photos/04_chemicals_bell__opened.jpg)

## Web interface

![Web interface](/docs/screenshots_web_interface/1_web__no_chemicals_awaiting.PNG)
![Web interface](/docs/screenshots_web_interface/4_web__chemicals_delivered_blue&white.PNG)

## Design

[Solidworks 3D PDF](/Solidworks/_WiFi_Chemicals_Delivery_Bell.pdf)

[Electronic diagram](/docs/electronics/electronic_diagram.pdf)

## Soldering templates

![Soldering template 3D](/docs/electronics/circuit.PNG)
![Soldering template top](/docs/electronics/circuit_top.PNG)
![Soldering template back](/docs/electronics/circuit_back.PNG)
![Photo](/docs/photos/08_detail__solder_front.jpg)
![Photo](/docs/photos/09_detail__solder_back.jpg)

## Shopping cart

[Shopping cart Farnell](/docs/shopping/_shopping_cart_Farnell.pdf)

[Shopping cart Kiwi Electronics](/docs/shopping/_shopping_cart_Kiwi_Electronics.pdf)
