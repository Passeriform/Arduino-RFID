<?php
	include "config.php";
	$uid=$uid_err="";
	if($_SERVER["REQUEST_METHOD"] == "POST")
	{
		if(empty(trim($_POST["uid"]))){
			$uid_err = "RFID didn't send any data";
		} 
		else{
			$uid = trim($_POST["uid"]);
		}
		if(empty($uid_err)) {
			$sql = "SELECT username FROM $generaldb WHERE userid=?";
			if($stmt = $mysqli->prepare($sql)) {
				$stmt->bind_param("s",$param_uid);
				$param_uid = $uid;
				$stmt->execute();
				$result = $stmt->get_result();
				if( $result->num_rows == 1 ) {
					$stmt->close();
					$row = $result->fetch_assoc();
					echo (1);
					echo $row["username"];
					exit();
				}
				
				else {
					$stmt->close();
					echo (0);
					echo ("Username doesnt exist");
					exit();
				}
			}
		}
		else
		{
			echo (-1);
			echo $uid_err;
		}
	}
	else {
		if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
			header("location: login.php");
			exit;
		}
		else {
			header("location: dashboard.php");
		}
	}
?>