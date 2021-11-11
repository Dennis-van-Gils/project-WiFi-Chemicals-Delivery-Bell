<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * This script will update the states of the white or blue LED buttons of the
 * web interface.
 *
 * POST arguments:
 *   - key       | str          , mandatory
 *   - white     | int (boolean), mandatory
 *   - blue      | int (boolean), mandatory
 *
 * Possible plain-text return values:
 *   Success:
 *   - '[white button state] [blue button state]'
 *
 *   No success:
 *   - 'Invalid key'
 *   - 'Missing arguments'
 *
 */
header("Content-Type: text/plain");
require_once 'globals.php';
session_start(); // TODO: check if actually obsolete statement

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  if (!isset($_POST['key'])) {
    exit;
  }

  if (!password_verify($_POST['key'], \Globals\HASHED_MAC_ADDRESS)) {
    echo "Invalid key";
    exit;
  }

  if (!isset($_POST['white']) | !isset($_POST['blue'])) {
      echo "Missing arguments";
      exit;
  }

  $white_button_state = filter_var($_POST['white'], FILTER_VALIDATE_INT);
  $blue_button_state = filter_var($_POST['blue'], FILTER_VALIDATE_INT);

  $data = array(
    'date' => date("D j M, H:i:s"),
    'white' => $white_button_state,
    'blue' => $blue_button_state,
  );
  $states = json_encode($data);
  file_put_contents(\Globals\FILE_BUTTON_STATES, $states);

  echo $white_button_state." ".$blue_button_state;
}
