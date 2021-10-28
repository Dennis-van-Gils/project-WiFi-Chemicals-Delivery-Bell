<?php
/**
 * @author Dennis van Gils
 * Web interface to the Arduino WiFi `Chemicals Delivery` bell.
 *
 * This script will update the states of the white or blue LED buttons of the
 * web interface.
 *
 * POST arguments:
 *  - white     | int (boolean), mandatory
 *  - blue      | int (boolean), mandatory
 *
 * Possible JSON return values:
 *  Success:
 *  - '[white button state] [blue button state]'
 *  No success:
 *  - 'Missing arguments'
 *
 */
header("Content-Type: application/json");
require_once 'globals.php';
session_start();

if ($_SERVER['REQUEST_METHOD'] == 'GET') { // TODO: Use POST instead of GET
    if (!isset($_GET['white']) | !isset($_GET['blue'])) {
        echo json_encode("Missing arguments");
        exit;
    }

    $white_button_state = filter_var($_GET['white'], FILTER_VALIDATE_INT);
    $blue_button_state = filter_var($_GET['blue'], FILTER_VALIDATE_INT);

    $data = array(
      'date' => date("D j M, H:i:s"),
      'white' => $white_button_state,
      'blue' => $blue_button_state,
    );
    $states = json_encode($data);
    file_put_contents(\Globals\FILE_BUTTON_STATES, $states);

    echo json_encode($white_button_state." ".$blue_button_state);
}
?>