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
Date  : 02-11-2021
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

#if __has_include("wifi_settings.h")
#  include <wifi_settings.h>
#else
#  include <wifi_settings_template.h>
#endif

// Network vars
String MAC_address;
String IP_address;

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

// 'Flask', 32x32px
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
      x_pos++;
      if (x_pos > 121) {
        x_pos = -25;
      }
    }
  }
};

Screensaver screensaver(10000, 200);

/*------------------------------------------------------------------------------
  wifi_send_buttons
------------------------------------------------------------------------------*/

bool wifi_send_buttons(bool white_state, bool blue_state) {
  /* Send out new button states to the server hosting the web interface.

  Returns true when successful, otherwise false.
  */
  bool success = false;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Sending...");
  display.display();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClientSecure client;
    String request;
    String payload;
    char expected_reply[4];
    int http_code;

    client.setInsecure(); // Secure is overkill for our project
    http.begin(client, url_button_pressed);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // clang-format off
    // MAC_address = "00:00"; // DEBUG: Test
    request = (F("key=") + MAC_address +
               F("&white=") + String(white_LED_is_on) +
               F("&blue=") + String(blue_LED_is_on));
    // clang-format on
    sprintf(expected_reply, "%d %d", white_LED_is_on, blue_LED_is_on);

    Serial.print("HTTP request: ");
    Serial.println(url_button_pressed);
    Serial.println(request);
    Serial.print("Reply: ");

    http_code = http.POST(request);
    payload = http.getString();

    if (http_code == 200) {
      Serial.println(payload);

      if (strcmp(payload.c_str(), "Invalid key") == 0) {
        Serial.println(F("ERROR: Invalid key received by web server."));
        Serial.println(F("See 'Globals.php' on the web server for the key it "
                         "expects. It should be the hashed MAC address of this "
                         "Arduino."));
        display.println(F("INVALIDKEY"));

      } else if (strcmp(payload.c_str(), expected_reply) == 0) {
        Serial.println("Success");
        display.println("Success");
        success = true;

      } else {
        Serial.println(expected_reply);
        Serial.println("ERROR: Unexpected reply from web server.");
        display.println("UNEXPREPLY");
      }

    } else {
      Serial.print("ERROR: ");
      Serial.println(http_code);
      Serial.println(payload);

      display.println("ERROR " + String(http_code));
    }

    http.end();

  } else {
    Serial.println("ERROR: WiFi disconnected");
    Serial.println("Press the reset button to reconnect.");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("NO WIFI.");
    display.println("PRESS RESET.");
  }

  Serial.println("");
  display.display();
  screensaver.reset();

  return success;
}

/*------------------------------------------------------------------------------
  setup
------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(9600);

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
  MAC_address = WiFi.macAddress();
  Serial.println(F("\n\nMAC address: ") + MAC_address);

  // OLED display:  128 x 32 px
  // Textsize  1 :    6 x  8
  //           2 :   12 x 16
  delay(100); // OLED display needs init time when cold power-on
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("MAC ") + MAC_address);

  // Connect to wireless network
  if (strcmp(ssid, "") == 0) {
    String msg =
        F("No WiFi configured.\nEdit wifi_settings.h,\nrecompile and flash.");
    Serial.println(msg);
    display.println(msg);
    display.display();
    while (true) { // Infinite loop without escape
      delay(100);
    }
  }

  Serial.print(F("Connecting to WiFi network `"));
  Serial.print(ssid);
  Serial.print(F("`."));
  display.println(F("Connecting to WiFi"));
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // Activity animation on serial monitor
    Serial.print(".");
    // Serial.println(WiFi.status());

    // Activity animation on OLED screen
    const char activity[] = "|/-\\";
    static byte idx_activity = 0;
    display.fillRect(114, 8, 6, 8, 0);
    display.setCursor(114, 8);
    display.println(activity[idx_activity]);
    display.display();
    if (++idx_activity > 3) {
      idx_activity = 0;
    }

    delay(500);
  }

  // Show obtained IP address
  IP_address = WiFi.localIP().toString();
  Serial.println(F(" success!"));
  Serial.println(F("IP address: ") + IP_address);
  display.println(F("IP  ") + IP_address);

  // Count down
  Serial.print(F("Starting in 4 secs..."));
  display.println(F("Starting in   secs..."));
  for (int8_t count_down_secs = 4; count_down_secs > 0; count_down_secs--) {
    display.fillRect(72, 24, 6, 8, 0);
    display.setCursor(72, 24);
    display.print(count_down_secs);
    display.display();
    delay(1000);
  }
  Serial.println(" done!\n");

  // Turn off all LEDs
  digitalWrite(PIN_LED_ONBOARD_RED, HIGH);
  digitalWrite(PIN_LED_ONBOARD_BLUE, HIGH);
  digitalWrite(PIN_LED_BTN_WHITE, LOW);
  digitalWrite(PIN_LED_BTN_BLUE, LOW);

  display.setTextSize(2);
  display.clearDisplay();
  display.setCursor(0, 0);

  // Send out initial button states to the server hosting the web interface
  wifi_send_buttons(white_LED_is_on, blue_LED_is_on);
}

/*------------------------------------------------------------------------------
  loop
------------------------------------------------------------------------------*/

void loop() {
  static bool states_have_changed = false;

  white_switch.poll();
  if (white_switch.pushed()) {
    white_LED_is_on = !white_LED_is_on;
  }

  blue_switch.poll();
  if (blue_switch.pushed()) {
    blue_LED_is_on = !blue_LED_is_on;
  }

  if (white_LED_is_on != white_LED_was_on) {
    white_LED_was_on = white_LED_is_on;
    digitalWrite(PIN_LED_BTN_WHITE, white_LED_is_on);
    digitalWrite(PIN_LED_ONBOARD_RED, !white_LED_is_on);
    Serial.println("Button: white");
    states_have_changed = true;
  }

  if (blue_LED_is_on != blue_LED_was_on) {
    blue_LED_was_on = blue_LED_is_on;
    digitalWrite(PIN_LED_BTN_BLUE, blue_LED_is_on);
    digitalWrite(PIN_LED_ONBOARD_BLUE, !blue_LED_is_on);
    Serial.println("Button: blue");
    states_have_changed = true;
  }

  if (states_have_changed) {
    // Send out new states to the server hosting the web interface
    wifi_send_buttons(white_LED_is_on, blue_LED_is_on);
    states_have_changed = false;
  }

  screensaver.update();

  // The ESP8266 mcu has a lot of background processes (WiFi connectivity and
  // such) that must get computing time without too much long delays in
  // between. Hence, we must yield to these processes frequently.
  yield();
  delay(10); // [ms]
}