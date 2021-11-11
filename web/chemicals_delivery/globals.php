<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * Adjust this configuration to your needs.
 *
 */
namespace Globals;

// Figure out the web root, e.g.
//   http://localhost/chemicals_delivery/ : Xampp local server (debug)
//   https://xxx.tnw.utwente.nl/          : Internet server    (release)
// DO NOT MODIFY
$protocol = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') ? 'https://' : 'http://';
define(__NAMESPACE__.'\WEB_ROOT', $protocol.$_SERVER['HTTP_HOST'].$_SERVER['CONTEXT_PREFIX'].'/');

// URL of the web interface
define(__NAMESPACE__.'\WEB_CHEMICALS_DELIVERY', \Globals\WEB_ROOT.'chemicals_delivery/');

// Filename of the textfile on the server to store the button status to
define(__NAMESPACE__.'\DIR_ROOT', __DIR__.'/');
define(__NAMESPACE__.'\FILE_BUTTON_STATUS', \Globals\DIR_ROOT.'button_status.txt');

// The Arduino can only successfully communicate with the web server if it sends
// along the correct key. The key that is send by the Arduino is its MAC address
// in normal ASCII form, like '84:CC:A8:62:1C:F2'. The server will internally
// hash this key and compare this hash to the following string:
//   You can use tool `https://phppasswordhash.com/` to transform the MAC
//   address of the Arduino to its equivalent hash and paste it below.
define(__NAMESPACE__.'\HASHED_MAC_ADDRESS', '$2y$10$Mven4dAvVq6UYpSTUqZWFeoV.Tk9BgEFHCZxykEfcXQL6lMhtfRuy');

// Email
//   email_to: Comma-separated list of recipients
define(__NAMESPACE__.'\email_tag' , '[DingDong]');
define(__NAMESPACE__.'\email_from', 'PoF Chemicals Delivery Bell <noreply@noreply.nl>');
define(__NAMESPACE__.'\email_to', 'd.p.m.vangils@utwente.nl');