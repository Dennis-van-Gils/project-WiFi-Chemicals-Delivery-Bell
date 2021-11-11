<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * This script will send out an email to inform the users that:
 *   1) Chemicals are delivered and awaiting to be processed. This is the case
 *      when either button status 'white' or 'blue' is active.
 *   2) No more chemicals are awaiting to be processed. This is the case when
 *      neither button status 'white' or 'blue' is active.
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
 *   - 'SERVER ERROR: While reading file ..."
 *   - 'SERVER ERROR: Could not parse file ..."
 *   - 'SERVER ERROR: Could not send email.'
 *
 * TODO: Send email when freshly booting/restarting informing user of such
 */
header("Content-Type: text/plain");
require_once 'globals.php';

function exceptions_error_handler($severity, $message, $filename, $lineno) {
  throw new ErrorException($message, 0, $severity, $filename, $lineno);
}

if ($_SERVER['REQUEST_METHOD'] == 'GET') {
  if (!isset($_GET['key'])) {
    exit;
  }

  if (!password_verify($_GET['key'], \Globals\HASHED_MAC_ADDRESS)) {
    echo "Invalid key";
    exit;
  }

  set_error_handler('exceptions_error_handler');

  // Read button status from file
  try {
    $data = file_get_contents(\Globals\FILE_BUTTON_STATUS);
  } catch (Exception $e) {
    echo "SERVER ERROR: While reading file `".\Globals\FILE_BUTTON_STATUS."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }

  // Parse button status from file
  try {
    $status = json_decode($data);
    $date = $status->{'date'};
    $white = $status->{'white'};
    $blue = $status->{'blue'};
  } catch (Exception $e) {
    echo "SERVER ERROR: Could not parse file `".\Globals\FILE_BUTTON_STATUS."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }

  restore_error_handler();

  // Send email
  $subject = 'Email test';
  $message = (
    'You have chemicals awaiting'.PHP_EOL.
    $date.PHP_EOL.
    $white.PHP_EOL.
    $blue.PHP_EOL);
  $headers = 'From: '.\Globals\email_from.PHP_EOL;

  if (mail(\Globals\email_to, $subject, $message, $headers)) {
    // Success
    echo "1";
  } else{
    // Error
    // There is no error message associated with the mail() function. There is
    // only a true or false returned on whether the email was accepted for
    // delivery. Not whether it ultimately gets delivered, but basically whether
    // the domain exists and the address is a validly formatted email address.
    echo "SERVER ERROR: Could not send email.".PHP_EOL."Check server logs.";
  }
}