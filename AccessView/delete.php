<?php
	require "config.php";
	session_start();
	if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
		header("location: login.php");
		exit;
	}
	$uid=base64_decode(urldecode($_GET['uid']));
	$query = "DELETE FROM $generaldb WHERE UserID='$uid'"; 
	$result = mysqli_query($mysqli,$query) or die ( mysqli_error());
	header("Location: view.php");
	mysqli_query($mysqli,("INSERT INTO $loggerdb (EventType,ActionBy,AffectedID) VALUES ('DELETE', '$actionby', '$uid')"));
?>