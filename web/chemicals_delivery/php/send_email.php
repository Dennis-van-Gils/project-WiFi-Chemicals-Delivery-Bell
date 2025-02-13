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
 */
header("Content-Type: text/plain");
require_once '../globals.php';

use Symfony\Component\Mime\Email;
use Symfony\Component\Mime\Address;
use Symfony\Component\Mailer\Mailer;
use Symfony\Component\Mailer\Transport\Smtp\EsmtpTransport;

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

  // Read Arduino status from file
  try {
    $data = file_get_contents(\Globals\FILE_ARDUINO_STATUS);
  } catch (Exception $e) {
    echo "SERVER ERROR: While reading file `".\Globals\FILE_ARDUINO_STATUS."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }

  // Parse Arduino status from file
  try {
    $status = json_decode($data);
    $date = $status->{'date'};
    $white = $status->{'white'};
    $blue = $status->{'blue'};
  } catch (Exception $e) {
    echo "SERVER ERROR: Could not parse file `".\Globals\FILE_ARDUINO_STATUS."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }

  restore_error_handler();

  // Construct email message
  $subject = \Globals\email_tag." ";

  if ($blue) {
    $subject = $subject."REFRIGERATED chemicals delivered";
    $headline = "REFRIGERATED chemicals delivered";
  } else if ($white) {
    $subject = $subject."Chemicals delivered";
    $headline = "Chemicals delivered";
  } else {
    $subject = $subject."  --  All done  --";
    $headline = "No chemicals awaiting anymore";
  }

  $message = (
    $headline.PHP_EOL.PHP_EOL.
    "  Date : ".$date.PHP_EOL.
    "  White: ".$white.PHP_EOL.
    "  Blue : ".$blue.PHP_EOL.PHP_EOL.
    "  Remote IP: ".$_SERVER['REMOTE_ADDR'].PHP_EOL.
    "  ".\Globals\WEB_CHEMICALS_DELIVERY
  );

  $email = (new Email())
    ->from(new Address(\Globals\email_from, \Globals\email_name))
    ->to(...\Globals\email_to)
    ->subject($subject)
    ->text($message);

  // Send email
  $transport = new EsmtpTransport('localhost');
  $mailer = new Mailer($transport);
  $mailer->send($email);

  if (!$mailer) {
    // Error: Could not send email
    echo "SERVER ERROR: Could not send email.".PHP_EOL.error_get_last();
  }

  // Save last sent email to file on server
  $data = array(
    'date' => $date,
    'headline' => $headline,
  );
  $last_email = json_encode($data);

  set_error_handler('exceptions_error_handler');
  try {
    file_put_contents(\Globals\FILE_LAST_EMAIL, $last_email);
  } catch (Exception $e) {
    echo "SERVER ERROR: While saving file `".\Globals\FILE_LAST_EMAIL."`".PHP_EOL;
    echo $e->getMessage();
    restore_error_handler();
    exit;
  }
  restore_error_handler();

  // Success
  echo "1";
}