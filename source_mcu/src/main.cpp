/*
WiFi Chemicals Delivery Bell

Hardware:
  Adafruit Feather HUZZAH with ESP8266 (Adafruit #3046)
  Adafruit FeatherWing OLED - 128x32 OLED  (Adafruit #2900)

This project uses the following code editor:
  Microsoft Visual Studio Code with plugin PLATFORMIO

NOTE:
  You must create a file called `wifi_settings.h` that should contain the user
  variables specific to your WiFi network, password and webpage script
  locations. You can use the provided template `wifi_settings_template.h` which
  you fill out and save as `wifi_settings.h`. You need to compile and upload
  this source code to the Arduino whenever you make changes to it.

GitHub: https://github.com/Dennis-van-Gils/project-WiFi-Chemicals-Delivery-Bell
Author: Dennis van Gils
Date  : 12-11-2021
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <avdweb_Switch.h>

#include <DvG_SerialCommand.h>
#include <StringReserveCheck.h>

#if __has_include("wifi_settings.h")
#  include <wifi_settings.h>
#else
#  include <wifi_settings_template.h>
#endif

// Serial monitor
#define Ser Serial
DvG_SerialCommand sc(Ser);

// String buffer for outgoing message
#define MSG_LEN 128
char msg[MSG_LEN] = {"\0"};

// Network vars
#define WIFI_BEGIN_TIMEOUT 30 // Stop trying to connect after N seconds [s]
char wifi_status_descr[16] = {"\0"};
wl_status_t wifi_status;

// Prevent String() from fragmenting the heap (too much):
// https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html#reboot
// https://www.instructables.com/Taming-Arduino-Strings-How-to-Avoid-Memory-Issues/
String http_payload;
StringReserveCheck http_payload_SRC;
String http_request;
StringReserveCheck http_request_SRC;
String ip_address;
StringReserveCheck ip_address_SRC;
String mac_address;
StringReserveCheck mac_address_SRC;

// LEDs
#define PIN_LED_ONBOARD_RED 0  // Inverted. Will mimick the white button
#define PIN_LED_ONBOARD_BLUE 2 // Inverted. Will mimick the blue button
#define PIN_LED_BTN_WHITE 14
#define PIN_LED_BTN_BLUE 16

bool white_LED_is_on = false;
bool white_LED_was_on = false;
bool blue_LED_is_on = false;
bool blue_LED_was_on = false;

// Switches
#define PIN_SWITCH_BTN_WHITE 12
#define PIN_SWITCH_BTN_BLUE 13
Switch white_switch = Switch(PIN_SWITCH_BTN_WHITE, INPUT_PULLUP);
Switch blue_switch = Switch(PIN_SWITCH_BTN_BLUE, INPUT_PULLUP);

// OLED display
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// Bitmap: 'Flask', 32x32px
// https://diyusthad.com/image2cpp
const unsigned char img_flask[] PROGMEM = {
    0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00,
    0x00, 0x1d, 0x80, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x3f, 0xb8, 0x00,
    0x00, 0x3f, 0xfc, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x07, 0xfc, 0x00,
    0x00, 0x07, 0xfc, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00,
    0x00, 0x38, 0x1c, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x1c, 0x38, 0x00,
    0x00, 0x0c, 0x30, 0x00, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x0c, 0x30, 0x00,
    0x00, 0x0c, 0x30, 0x00, 0x00, 0x1c, 0x38, 0x00, 0x00, 0x18, 0x18, 0x00,
    0x00, 0x38, 0x1c, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00,
    0x00, 0xe7, 0x9f, 0x00, 0x01, 0xff, 0xff, 0x80, 0x01, 0xff, 0xf7, 0x80,
    0x03, 0x98, 0x61, 0xc0, 0x03, 0x80, 0x01, 0xc0, 0x03, 0x00, 0x00, 0xc0,
    0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xc0};

// Timers
uint32_t now = 0;
uint32_t tick_0 = 0;
uint32_t tick_1 = 0;
uint32_t tick_2 = 0;

/*------------------------------------------------------------------------------
  Screensaver
------------------------------------------------------------------------------*/

class Screensaver {
private:
  uint32_t T_wait;        // Screensaver shows after this period         [ms]
  uint16_t T_frame;       // Display period for each frame               [ms]
  uint32_t last_activity; // millis() value since last call to `reset()` [ms]
  uint32_t last_draw;     // millis() value since last frame draw        [ms]
  int16_t x_pos;          // Screensaver animation x-pixel position
  uint32_t now;

public:
  Screensaver(uint32_t wait_period_ms, uint16_t frame_period_ms) {
    // Args:
    //    wait_period_ms  (uint32_t): Show screensaver after this time [ms]
    //    frame_period_ms (uint16_t): Screensaver animation speed [ms]
    T_wait = wait_period_ms;
    T_frame = frame_period_ms;
    reset();
  }

  void reset() {
    // Reset the inactivity timer of the screensaver
    now = millis();
    last_activity = now;
    last_draw = now;
    x_pos = 0;
  }

  void update() {
    // Draw screensaver to OLED screen when inactivity timer has fired,
    // otherwise leave the screen alone and continue waiting.
    now = millis();

    if ((now - last_activity > T_wait) & (now - last_draw > T_frame)) {
      display.clearDisplay();
      display.drawBitmap(x_pos, 0, img_flask, 32, 32, WHITE);
      display.display();
      last_draw = now;

      // Scroll animation left to right
      x_pos++;
      if (x_pos > 121) {
        x_pos = -25;
      }
    }
  }
};

Screensaver screensaver(10000, 200);

/*------------------------------------------------------------------------------
  get_wifi_status_descr
------------------------------------------------------------------------------*/

// clang-format off
void get_wifi_status_descr(wl_status_t status, char descr[16]) {
  switch (status) {
    case WL_IDLE_STATUS    : snprintf(descr, 16, "%s", "IDLE_STATUS")    ; break;
    case WL_NO_SSID_AVAIL  : snprintf(descr, 16, "%s", "NO_SSID_AVAIL")  ; break;
    case WL_SCAN_COMPLETED : snprintf(descr, 16, "%s", "SCAN_COMPLETED") ; break;
    case WL_CONNECTED      : snprintf(descr, 16, "%s", "CONNECTED")      ; break;
    case WL_CONNECT_FAILED : snprintf(descr, 16, "%s", "CONNECT_FAILED") ; break;
    case WL_CONNECTION_LOST: snprintf(descr, 16, "%s", "CONNECTION_LOST"); break;
    case WL_WRONG_PASSWORD : snprintf(descr, 16, "%s", "WRONG_PASSWORD") ; break;
    case WL_DISCONNECTED   : snprintf(descr, 16, "%s", "DISCONNECTED")   ; break;
    default                : snprintf(descr, 16, "%s", "")               ; break;
  }
}
// clang-format on

/*------------------------------------------------------------------------------
  infinite_loop
------------------------------------------------------------------------------*/

void infinite_loop(const char *final_msg) {
  // Infinite loop without escape, while displaying a final error message on the
  // OLED screen flashing on and off to prevent OLED burn-in.
  bool toggle = true;
  Ser.println(final_msg);

  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    if (toggle) {
      display.print(final_msg);
      display.display();
      delay(2000);
    } else {
      display.display();
      delay(1000);
    }
    toggle = !toggle;
  }
}

/*------------------------------------------------------------------------------
  http_post
------------------------------------------------------------------------------*/

bool http_post(const String &url, const String &post_http_request,
               const String &post_expected_reply) {
  /* Send a HTTP POST request to the web server. Will handle errors and also
  checks for a correct POST reply. Shows info on the serial monitor and OLED
  display.

  Returns true when successful, otherwise false.
  */
  bool success = false;
  HTTPClient http;
  WiFiClientSecure client;
  int http_code;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("Sending..."));
  display.display();

  // yield();
  delay(1);

  wifi_status = WiFi.status();
  if (wifi_status == WL_CONNECTED) {
    Ser.println(F("Request"));
    Ser.print(F("  "));
    Ser.println(url);
    Ser.print(F("  "));
    Ser.println(post_http_request);

    client.setInsecure(); // Secure is overkill for our project
    http.begin(client, url);
    http.addHeader(F("Content-Type"), F("application/x-www-form-urlencoded"));

    http_code = http.POST(post_http_request);
    http_payload = http.getString();

    // yield();
    delay(1);

    Ser.println(F("Reply"));
    Ser.println(F("  \"\"\""));
    Ser.println(http_payload);
    Ser.println(F("  \"\"\""));

    if (http_code == 200) {
      if (strcmp(http_payload.c_str(), post_expected_reply.c_str()) == 0) {
        Ser.println(F("Success"));
        display.println(F("Success"));
        success = true;

      } else if (strcmp(http_payload.c_str(), "Invalid key") == 0) {
        Ser.println(F("SERVER ERROR: Invalid key received by web server."));
        Ser.println(F("See 'Globals.php' on the web server."));
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(F("SERVER ERR\nINVALIDKEY"));

      } else {
        Ser.println(F("SERVER ERROR: Unexpected reply from web server."));
        Ser.print(F("Was expecting: "));
        Ser.println(post_expected_reply);
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(F("SERVER ERR\nUNEXPREPLY"));
      }

    } else { // if (http_code == 200)
      Ser.print(F("HTTP ERROR "));
      Ser.println(http_code);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("HTTP ERROR"));
      display.println(http_code);
    }

    http.end();

  } else { // if (wifi_status == WL_CONNECTED)
    // WiFi got disconnected
    // TODO: Try to reconnect?
    display.setTextSize(1);
    get_wifi_status_descr(wifi_status, wifi_status_descr);
    snprintf(msg, MSG_LEN, "WiFi disconnected:\n%s\nPress reset.",
             wifi_status_descr);
    infinite_loop(msg);
  }

  // yield();
  delay(1);

  Ser.println("");
  display.display();

  return success;
}

/*------------------------------------------------------------------------------
  send_starting_up
------------------------------------------------------------------------------*/

void send_starting_up() {
  /* Signal the web server that the Arduino has (re)started.
   */
  Ser.println(F("STARTING UP\n-----------"));
  http_request = F("key=");
  http_request += mac_address;
  http_post(url_starting_up, http_request, "1");
  screensaver.reset();
}

/*------------------------------------------------------------------------------
  send_email
------------------------------------------------------------------------------*/

void send_email(bool restarted = false) {
  /* Signal the web server to send out emails.
   */
  Ser.println(F("EMAIL\n-----"));
  http_request = F("key=");
  http_request += mac_address;
  http_post(url_send_email, http_request, "1");
  screensaver.reset();
}

/*------------------------------------------------------------------------------
  send_buttons
------------------------------------------------------------------------------*/

void send_buttons(bool white_state, bool blue_state) {
  /* Send out new button states to the web server.
   */
  char expected_reply[4];
  snprintf(expected_reply, 4, "%d %d", white_LED_is_on, blue_LED_is_on);

  http_request = F("key=");
  http_request += mac_address;
  http_request += F("&white=");
  http_request += white_LED_is_on;
  http_request += F("&blue=");
  http_request += blue_LED_is_on;
  http_post(url_button_pressed, http_request, expected_reply);
  screensaver.reset();
}

/*------------------------------------------------------------------------------
  setup
------------------------------------------------------------------------------*/

void setup() {
  Ser.begin(9600);

  // Prevent String() from fragmenting the heap (too much)
  http_payload.reserve(512); // Large enough to hold potential 404/503 HTML page
  http_payload_SRC.init(http_payload, Ser);
  http_request.reserve(40); // Largest: 'key=00:AA:00:AA:00:AA&white=0&blue=0'
  http_request_SRC.init(http_request, Ser);
  ip_address.reserve(46); // Large enough for IPv6
  ip_address_SRC.init(ip_address, Ser);
  // mac_address.reserve(18);
  // mac_address_SRC.init(mac_address, Ser);
  if (!mac_address.reserve(18)) {
    while (1) {
      Ser.println(F("Strings out-of-memory"));
      delay(3000);
    }
  }
  if (!mac_address_SRC.init(mac_address, Ser)) {
    Ser.println(F("Memory Low after reserves()"));
  }

  // LEDs
  pinMode(PIN_LED_ONBOARD_RED, OUTPUT);
  pinMode(PIN_LED_ONBOARD_BLUE, OUTPUT);
  pinMode(PIN_LED_BTN_WHITE, OUTPUT);
  pinMode(PIN_LED_BTN_BLUE, OUTPUT);

  // Turn on all LEDs at boot
  digitalWrite(PIN_LED_ONBOARD_RED, LOW);
  digitalWrite(PIN_LED_ONBOARD_BLUE, LOW);
  digitalWrite(PIN_LED_BTN_WHITE, HIGH);
  digitalWrite(PIN_LED_BTN_BLUE, HIGH);

  // Show MAC address
  mac_address = WiFi.macAddress();
  Ser.print(F("\n\nMAC address: "));
  Ser.println(mac_address);

  // OLED display:  128 x 32 px
  // Textsize  1 :    6 x  8
  //           2 :   12 x 16
  delay(100); // OLED display needs init time when cold power-on
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("MAC "));
  display.println(mac_address);

  // Check WiFi config
  if (strcmp(ssid, "") == 0) {
    infinite_loop("No WiFi configured.\n"
                  "Edit wifi_settings.h,\n"
                  "recompile and flash.");
  }

  // Connect to WiFi
  Ser.print(F("Connecting to WiFi network `"));
  Ser.print(ssid);
  Ser.print(F("`."));
  display.println(F("Connecting to WiFi"));
  display.display();

  now = millis();
  tick_0 = now;
  tick_1 = now;
  tick_2 = now;

  // Activity animation on OLED screen
  const char activity[] = "|/-\\";
  byte idx_activity = 0;

  wifi_status = WiFi.begin(ssid, password);
  while (true) {
    now = millis();

    // Check WiFi status
    if (now - tick_1 > 1000) {
      tick_1 += 1000;
      wifi_status = WiFi.status();
      if (wifi_status == WL_CONNECTED) {
        break;
      }

      Ser.print(F("."));
      // Ser.println(wifi_status);

      if (now - tick_0 > WIFI_BEGIN_TIMEOUT * 1e3) {
        WiFi.disconnect();
        get_wifi_status_descr(wifi_status, wifi_status_descr);
        snprintf(msg, MSG_LEN, "Could not connect to WiFi:\n%s\nPress reset.",
                 wifi_status_descr);
        infinite_loop(msg);
      }
    }

    // Activity animation on OLED screen
    if (now - tick_2 > 250) {
      tick_2 += 250;
      display.fillRect(114, 8, 6, 8, 0);
      display.setCursor(114, 8);
      display.println(activity[idx_activity]);
      display.display();
      if (++idx_activity > 3) {
        idx_activity = 0;
      }
    }

    // yield();
    delay(1);
  }

  // Show obtained IP address
  ip_address = WiFi.localIP().toString();
  Ser.println(F(" Success!"));
  Ser.print(F("IP address: "));
  Ser.println(ip_address);
  display.print(F("IP  "));
  display.println(ip_address);

  // Count down
  Ser.print(F("Starting in 4 secs..."));
  display.println(F("Starting in   secs..."));
  for (int8_t count_down_secs = 4; count_down_secs > 0; count_down_secs--) {
    display.fillRect(72, 24, 6, 8, 0);
    display.setCursor(72, 24);
    display.print(count_down_secs);
    display.display();
    delay(1000);
  }
  Ser.println(F(" Done!\n"));

  // Turn off all LEDs
  digitalWrite(PIN_LED_ONBOARD_RED, HIGH);
  digitalWrite(PIN_LED_ONBOARD_BLUE, HIGH);
  digitalWrite(PIN_LED_BTN_WHITE, LOW);
  digitalWrite(PIN_LED_BTN_BLUE, LOW);

  // Larger OLED text
  display.setTextSize(2);

  // Signal the web server that the Arduino has (re)started
  send_starting_up();
}

/*------------------------------------------------------------------------------
  loop
------------------------------------------------------------------------------*/

void loop() {
  char *strCmd; // Incoming serial command string
  static bool states_have_changed = false;

  // Check physical buttons
  white_switch.poll();
  if (white_switch.pushed()) {
    white_LED_is_on = !white_LED_is_on;
  }

  blue_switch.poll();
  if (blue_switch.pushed()) {
    blue_LED_is_on = !blue_LED_is_on;
  }

  // Check for incoming serial commands used for debugging
  if (sc.available()) {
    strCmd = sc.getCmd();

    if (strcmp(strCmd, "w") == 0) {
      white_LED_is_on = !white_LED_is_on;

    } else if (strcmp(strCmd, "b") == 0) {
      blue_LED_is_on = !blue_LED_is_on;

    } else if (strcmp(strCmd, "e") == 0) {
      send_email();

    } else if (strcmp(strCmd, "s") == 0) {
      send_starting_up();

    } else if (strcmp(strCmd, "?") == 0) {
      Ser.print(white_LED_is_on);
      Ser.print(" ");
      Ser.print(blue_LED_is_on);
      Ser.println("\n");
    }
  }

  // Did anything change?
  if (white_LED_is_on != white_LED_was_on) {
    white_LED_was_on = white_LED_is_on;
    digitalWrite(PIN_LED_BTN_WHITE, white_LED_is_on);
    digitalWrite(PIN_LED_ONBOARD_RED, !white_LED_is_on);
    Ser.println(F("WHITE\n-----"));
    states_have_changed = true;
  }

  if (blue_LED_is_on != blue_LED_was_on) {
    blue_LED_was_on = blue_LED_is_on;
    digitalWrite(PIN_LED_BTN_BLUE, blue_LED_is_on);
    digitalWrite(PIN_LED_ONBOARD_BLUE, !blue_LED_is_on);
    Ser.println(F("BLUE\n-----"));
    states_have_changed = true;
  }

  if (states_have_changed) {
    // Send out new states to the web server
    send_buttons(white_LED_is_on, blue_LED_is_on);
    states_have_changed = false;
  }

  // The ESP8266 mcu has a lot of background processes (WiFi connectivity and
  // streams) that must get computing time without too much long delays in
  // between. This is critical to prevent a possible crash and reset of
  // the ESP8266. Hence, we must yield to these processes frequently.
  // yield();
  // EDIT: `delay()` gets redefined by the ESP8266 library to include a call to
  // `yield()` -- actually, `esp_yield()`. Several sources on the internet state
  // that their projects work better when calling `delay(1)` instead of straight
  // `yield()`.
  delay(1);

  screensaver.update();
}