Changelog
=========

4.0.0 (2025-02-13)
------------------
Keep track of the last sent email and print this info to the webpage.

3.0.0 (2025-02-13)
------------------
Symfony Framework is now used to send out notification emails, instead of using
the native PHP `mail()` method. You must now make sure to install Symfony using
Composer onto your PHP server.

2.0.0 (2024-11-06)
------------------
After the automated 'Chemicals delivered' email has been send to the user,
revert all buttons back to their idle off state. The user no longer has to
deactivate any lit buttons, and hence the 'All done' email message is now
suppressed.

1.0.0 (2021-10-28)
------------------
* Initial release
