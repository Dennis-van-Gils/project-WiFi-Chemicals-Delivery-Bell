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
  $subject = \Globals\email_tag." "."Arduino has restarted";
  $message = "The WiFi Arduino `Chemicals Delivery Bell` has (re)started";
  $message = (
    $message.PHP_EOL.PHP_EOL.
    "  Date : ".$data['date'].PHP_EOL.
    "  White: ".$data['white'].PHP_EOL.
    "  Blue : ".$data['blue'].PHP_EOL.PHP_EOL.
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
    exit;
  }

  // Success
  echo "1";
}
