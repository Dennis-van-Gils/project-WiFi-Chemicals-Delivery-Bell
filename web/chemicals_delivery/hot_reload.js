///-----------------------------------------------------------------
///
///   hot_reload.js : Simple Javascript include file to poll server (PHP)
///   file checker code
///
///   Description: XHTTP Request polls for file changes
///   Author: abrandao29@gmail.com
///   Date  : 4/28/2019
///   Notes : <script src="hotreload.js">
///
///   MODIFIED BY: Dennis van Gils
///
///-----------------------------------------------------------------

// Hot-reload the webpage whenever the file pointed to by variable
// `\Globals\FILE_ARDUINO_STATUS` changes on the server. Checks every N seconds.
var url = "hot_reload.php";
var poll_interval = 5000; // [ms]

setInterval(
  function() {
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
      if (xmlhttp.readyState == XMLHttpRequest.DONE) {
        if (xmlhttp.status == 200) {
          // console.log(xmlhttp.responseText);
          j = JSON.parse(xmlhttp.responseText);
          // console.log(j);

          if (j.hasChanged) {
            // console.log("Changed!");
            window.location.reload(false); // Reload the page
          } else {
            // console.log("No Change.");
          }

        } else {
          alert('ERROR '+ xmlhttp.status);
        }
      }
    };

    xmlhttp.open("GET", url, true);
    xmlhttp.send();
  },

  poll_interval
);
