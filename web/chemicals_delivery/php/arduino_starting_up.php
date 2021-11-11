<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * This script will initialize the status of the white and blue LED buttons at
 * the server side, to be shown in the web interface. And this script will send
 * out an email informing the users that the Arduino has (re)started.
 *
 * POST arguments:
 *   - key       | str          , mandatory
 *
 * Possible plain-text return values:
 *   Success:
 *   - '1'
 *
 *   No success:
 *   - 'Invalid key'
 *   - 'SERVER ERROR: While saving file ..."
 *   - 'SERVER ERROR: Could not send email.'
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

  $data = array(
    'date' => date("D j M, H:i:s"),
    'white' => 0,
    'blue' => 0,
    'starting_up' => 1,
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

  // Construct email message
  $headers = 'From: '.\Globals\email_from.PHP_EOL;
  $subject = \Globals\email_tag." "."Arduino has restarted";
  $message = "The WiFi Arduino `Chemicals Delivery Bell` has (re)started";
  $message = (
    $message.PHP_EOL.PHP_EOL.
    "  Date : ".$date.PHP_EOL.
    "  White: ".$white.PHP_EOL.
    "  Blue : ".$blue.PHP_EOL.PHP_EOL.
    "  Remote IP: ".$_SERVER['REMOTE_ADDR'].PHP_EOL.
    "  ".\Globals\WEB_CHEMICALS_DELIVERY
  );

  // Send email
  if (!mail(\Globals\email_to, $subject, $message, $headers)) {
    // Error
    // There is no error message associated with the mail() function. There is
    // only a true or false returned on whether the email was accepted for
    // delivery. Not whether it ultimately gets delivered, but basically whether
    // the domain exists and the address is a validly formatted email address.
    echo "SERVER ERROR: Could not send email.".PHP_EOL."Check server logs.";
    exit;
  }

  // Success
  echo "1";
}
