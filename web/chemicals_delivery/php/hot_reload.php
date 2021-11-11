<?php
/**
 * hotreload.php
 *
 * Simple file checker and returns back information about the state of the file
 *
 * @copyright  2019 abrandao.com
 * @license    http://www.php.net/license/3_0.txt  PHP License 3.0
 * @author Tony Brandao <abrandao29@gmail.com>
 *
 * MODIFIED BY: Dennis van Gils
 */
error_reporting(E_ALL);
ini_set('display_errors', 'On');
header('Content-Type: application/json');
require_once 'globals.php';
session_start();

$changed = false;

if (@file_exists(\Globals\FILE_BUTTON_STATUS)) {
  $md5 = md5_file(\Globals\FILE_BUTTON_STATUS);
} else {
  die(json_encode("File ".\Globals\FILE_BUTTON_STATUS." does not exist."));
}

if (isset($_SESSION[\Globals\FILE_BUTTON_STATUS])) { // Do we have this session set?
  if ($md5 != $_SESSION[\Globals\FILE_BUTTON_STATUS]) {
    $changed = true;
    $_SESSION[\Globals\FILE_BUTTON_STATUS] = $md5;
  }
} else {
  $_SESSION[\Globals\FILE_BUTTON_STATUS] = $md5;
}

echo json_encode([
  'filename' => \Globals\FILE_BUTTON_STATUS,
 	'hasChanged' => $changed
]);