<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi Chemicals Delivery Bell.
 */
header("Content-Type: text/html;charset=UTF-8");
require_once 'globals.php';
session_start();

// Instead of having a MySQL database to keep track of the WiFi Arduino button
// states we will use a simple text file on the server to keep track of the
// button states.
if (!file_exists(\Globals\FILE_BUTTON_STATES)) {
  $data = array(
    'date' => 'Unknown',  // String: Date of last registered button press
    'white' => 0,         // Int [bool]: State of white button
    'blue' => 0,          // Int [bool]: State of blue button
  );
  $states = json_encode($data);
  file_put_contents(\Globals\FILE_BUTTON_STATES, $states);

} else {
  $states = json_decode(file_get_contents(\Globals\FILE_BUTTON_STATES));
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
<meta http-equiv="refresh" content="60">

</head>

<body>

<!----------------------------------------------------------------
    BODY CONTENT
------------------------------------------------------------------>

<div class="container">
  <div class="row">
    <h1>
      <?php
        if ($states->white == true |
            $states->blue == true) {
          echo "Chemicals delivered";
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
          echo ($states->white == true ? "blinking" : "off");
        ?>
      ></div>
      <div style="text-align:center">REGULAR</div>
    </div>

    <div class="led-box">
      <div class="led-blue"
        <?php
          echo ($states->blue == true ? "blinking" : "off");
        ?>
      ></div>
      <div style="text-align:center">COOLED</div>
    </div>
  </div>

  <div class="row">
    <div>
      <small>Last button press was registered at:</small><br>
      <?php echo $states->date ?>
    </div>
  </div>

  <div class="row">
    <div>
      <?php
        echo "<small>Page updated at:</small><br>" . date("D j M, H:i:s");
      ?>
    </div>
  </div>
</div>

</body>
</html>