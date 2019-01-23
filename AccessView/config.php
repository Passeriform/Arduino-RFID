<?php
	include('vars.php');

	define('DB_SERVER', 'localhost');
	define('DB_USERNAME', $db_id);
	define('DB_PASSWORD', $db_pass);
	define('DB_NAME', $db_name);
	
	$mysqli = new mysqli(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

	if($mysqli === false){
		die("ERROR: Could not connect. " . $mysqli->connect_error);
}
?>