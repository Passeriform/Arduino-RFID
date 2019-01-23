<?php
	session_start();
	if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
		header("location: login.php");
		exit;
	}
?>

<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>Dashboard - RNC</title>
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css">
	</head>
	<body>
		<div class="form">
			<p>Welcome to Dashboard.</p>

			<p><a href="dashboard.php">Home</a><p>
			<p><a href="insert.php">Insert New Record</a></p>
			<p><a href="view.php">View Records</a><p>
			<p><a href="edit.php">Edit Records</a><p>
			<p><a href="logout.php">Logout</a></p>
		
			<br /><br /><br /><br />
		</div>
	</body>
</html>
