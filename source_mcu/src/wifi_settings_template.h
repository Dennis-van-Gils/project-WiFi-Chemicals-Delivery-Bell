#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H

/*
`wifi_settings.h` should contain the user variables specific to your WiFi
network, password and webpage script locations. You can use the provided
template `wifi_settings_template.h` which you fill out and save as
`wifi_settings.h`. You need to compile and upload this source code to the
Arduino whenever you make changes to it.
*/

/*
The SSID of the WiFi network that the Arduino should connect to.
E.g.: "UThings" for the University of Twente.
*/
const char *ssid = "";

/*
Specific to the UThings network (and perhaps other IoT networks as well):
The Arduino should get a unique 'pre-shared key' password from the network
administrator (LISA). This is needed to allow the Arduino easy access to the
UThings network. You need to give LISA the MAC address of your Arduino. They
will then couple this MAC address with a unique key which they will send to you.
Keep this password secret.

You can upload this source code to the Arduino and leave the password blank.
The MAC address of the Arduino will be shown on the OLED display and in a serial
monitor (like PuTTY for Windows) during boot.
*/
const char *password = "";

// The URL to the PHP script on your webserver that will handle the button
// presses.
// E.g.: "https://[your web host]/chemicals_delivery/button_pressed.php"
const char *url_button_pressed = "";

#endif