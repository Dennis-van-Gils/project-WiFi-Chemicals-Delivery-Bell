<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * This script will update the status of the white and blue LED buttons at the
 * server side, to be shown in the web interface.
 *
 * POST arguments:
 *   - key       | str          , mandatory
 *   - white     | int (boolean), mandatory
 *   - blue      | int (boolean), mandatory
 *
 * Possible plain-text return values:
 *   Success:
 *   - '[white button status] [blue button status]'
 *
 *   No success:
 *   - 'Invalid key'
 *   - 'Missing arguments'
 *   - 'SERVER ERROR: While saving file ..."
 *
 */
header("Content-Type: text/plain");
require_once '../globals.php';

function exceptions_error_handler($severity, $message, $filename, $lineno) {
  throw new ErrorException($message, 0, $severity, $filename, $lineno);
}

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

  $white = filter_var($_POST['white'], FILTER_VALIDATE_INT);
  $blue = filter_var($_POST['blue'], FILTER_VALIDATE_INT);
  $data = array(
    'date' => date("D j M, H:i:s"),
    'white' => $white,
    'blue' => $blue,
    'starting_up' => 0,
  );
  $status = json_encode($data);

  // Save Arduino status to file
  set_error_handler('exceptions_error_handler');
  try {
    file_put_contents(\Globals\FILE_ARDUINO_STATUS, $status);
  } catch (Exception $e) {
    echo "SERVER ERROR: While saving file `".\Globals\FILE_ARDUINO_STATUS."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }
  restore_error_handler();

  echo $white." ".$blue;
}
