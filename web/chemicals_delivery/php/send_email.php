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
 *   3) The Arduino has (re)started. This is the case when POST argument
 *      `restarted` has been defined, regardless of its value.
 *
 * POST arguments:
 *   - key       | str          , mandatory
 *   - restarted | any          , optional
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

  if (!isset($_POST['restarted'])) {
    $restarted = false;
  } else {
    $restarted = true;
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

  // Construct email message
  $headers = 'From: '.\Globals\email_from.PHP_EOL;
  $subject = \Globals\email_tag." ";

  if ($restarted) {
      $subject = $subject."Arduino has restarted";
      $message = "The WiFi Arduino `Chemicals Delivery Bell` has (re)started";
  } else {
    if ($blue) {
      $subject = $subject."REFRIGERATED chemicals delivered";
      $message = "REFRIGERATED chemicals delivered";
    } else if ($white) {
      $subject = $subject."Chemicals delivered";
      $message = "Chemicals delivered";
    } else {
      $subject = $subject."No chemicals awaiting anymore";
      $message = "No chemicals awaiting anymore";
    }
  }

  $message = (
    $message.PHP_EOL.PHP_EOL.
    "  Date : ".$date.PHP_EOL.
    "  White: ".$white.PHP_EOL.
    "  Blue : ".$blue.PHP_EOL.PHP_EOL.
    "  Remote IP: ".$_SERVER['REMOTE_ADDR'].PHP_EOL.
    "  ".\Globals\WEB_CHEMICALS_DELIVERY
  );

  // Send email
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