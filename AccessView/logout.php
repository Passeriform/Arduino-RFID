<?php
// Initialize the session
session_start();
 
// Unset all of the session variables
$_SESSION = array();
 
// Destroy the session.
session_destroy();
 
// Redirect to login page
header('Location: login.php?msg=' . urlencode(base64_encode("You have been successfully logged out!")));
exit;
?>