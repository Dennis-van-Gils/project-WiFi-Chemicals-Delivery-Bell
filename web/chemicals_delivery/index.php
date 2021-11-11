<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` Bell.
 *
 */
header("Content-Type: text/html;charset=UTF-8");
require_once 'globals.php';
session_start();

// Instead of having a MySQL database to keep track of the status of the white
// and blue LED buttons of the Arduino, we will use a simple text file on the
// server instead.
if (!file_exists(\Globals\FILE_ARDUINO_STATUS)) {
  // File does not exist. Create one.
  $data = array(
    'date' => 'Unknown',  // String: Date of last registered button press
    'white' => 0,         // Int [bool]: Is the white LED button turned on?
    'blue' => 0,          // Int [bool]: Is the blue LED button turned on?
    'starting_up' => 0,   // Int [bool]: Has the Arduino just started up?
  );
  $status = json_encode($data);
  file_put_contents(\Globals\FILE_ARDUINO_STATUS, $status);
} else {
  // File exists --> read file contents
  $status = json_decode(file_get_contents(\Globals\FILE_ARDUINO_STATUS));
}

?>

<!----------------------------------------------------------------
    HTML
------------------------------------------------------------------>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>
<title>PoF Chemicals Delivery</title>

<link rel="shortcut icon" type="image/x-icon" href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."favicon.ico?"; ?>>
<link rel="apple-touch-icon" sizes="180x180" href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."apple-touch-icon.png"; ?>>
<link rel="icon" type="image/png" sizes="32x32" href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."favicon-32x32.png"; ?>>
<link rel="icon" type="image/png" sizes="16x16" href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."favicon-16x16.png"; ?>>
<link rel="manifest" href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."site.webmanifest"; ?>>
<link href=<?php echo \Globals\WEB_CHEMICALS_DELIVERY.'stylesheet.css'; ?> rel="stylesheet" type="text/css">

<meta charset="UTF-8">
<meta name="robots" content="noindex, nofollow">
<meta name="viewport" id="viewport" content="width=device-width, initial-scale=1.0">
<meta name="author" content="Dennis van Gils">

<!-- <meta http-equiv="refresh" content="60"> -->
<!-- NOT NECESSARY. We use a Javascript to hot reload the webpage when needed. -->
<script src=<?php echo \Globals\WEB_CHEMICALS_DELIVERY."hot_reload.js"?>></script>

</head>

<body>

<!----------------------------------------------------------------
    BODY CONTENT
------------------------------------------------------------------>

<div class="container">
  <div class="row">
    <h1>
      <?php
        if ($status->white == true |
            $status->blue == true) {
          echo "Chemicals delivered";
        } elseif ($status->starting_up == true) {
          echo "Arduino has (re)started";
        } else {
          echo "No chemicals awaiting";
        }
      ?>
    </h1>
  </div>

  <div class="row">
    <div class="led-box">
      <div class="led-white"
        <?php
          echo ($status->white == true ? "blinking" : "off");
        ?>
      ></div>
      <div style="text-align:center">REGULAR</div>
    </div>

    <div class="led-box">
      <div class="led-blue"
        <?php
          echo ($status->blue == true ? "blinking" : "off");
        ?>
      ></div>
      <div style="text-align:center">COOLED</div>
    </div>
  </div>

  <div class="row">
    <div>
      <small>Last activity was registered at:</small><br>
      <?php echo $status->date ?>
    </div>
  </div>

  <!--
  <div class="row">
    <div>
      <?php
        echo "<small>Page updated at:</small><br>" . date("D j M, H:i:s");
      ?>
    </div>
  </div>
  -->

  <div class="row">
    <div><small>The project source can be found <a
    href="https://github.com/Dennis-van-Gils/project-WiFi-Chemicals-Delivery-Bell"
    target="_blank">here</a>.</small></div>
  </div>

</div>

</body>
</html>