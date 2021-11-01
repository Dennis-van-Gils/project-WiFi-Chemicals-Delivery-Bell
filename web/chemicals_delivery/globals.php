<?php
namespace Globals;

// Web root, e.g.
//   http://localhost/chemicals_delivery/   : Xampp local server (debug)
//   https://xxx.tnw.utwente.nl/            : Internet server    (release)
$protocol = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') ? 'https://' : 'http://';
define(__NAMESPACE__.'\WEB_ROOT', $protocol.$_SERVER['HTTP_HOST'].$_SERVER['CONTEXT_PREFIX'].'/');
define(__NAMESPACE__.'\WEB_CHEMICALS_DELIVERY', \Globals\WEB_ROOT.'chemicals_delivery/');
define(__NAMESPACE__.'\FILE_BUTTON_STATES', 'button_states.txt');

define(__NAMESPACE__.'\HASHED_MAC_ADDRESS', '$2y$10$Mven4dAvVq6UYpSTUqZWFeoV.Tk9BgEFHCZxykEfcXQL6lMhtfRuy');

// Email
// define(__NAMESPACE__.'\email_tag' , '[PoF Inv]');
// define(__NAMESPACE__.'\email_name', 'PoF Inventory');
// define(__NAMESPACE__.'\email_from', 'pof.inventory@'.gethostname());