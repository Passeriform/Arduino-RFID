<?php
	require "config.php";
	session_start();
	if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
		header("location: login.php");
		exit;
	}
	$status = "";
	$uid = $uname = "";
	$uid_err = $uname_err = "";

	if($_SERVER["REQUEST_METHOD"] == "POST"){
	
		// Check if uname is empty
		if(empty(trim($_POST["uid"]))){
			$uid_err = 'Please enter UserID.';
		} 
		else{
			$uid = trim($_POST["uid"]);
		}
    
		// Check if password is empty
		if(empty(trim($_POST['uname']))){
			$uname_err = 'Please enter UserName.';
		} 
		else{
			$uname = trim($_POST['uname']);
		}
		
		// Validate credentials
		if(empty($uid_err) && empty($uname_err)){
			// Prepare a select statement
			$sql = "INSERT INTO $generaldb (`userid`,`username`) VALUES (?,?)";
        
			if($stmt = $mysqli->prepare($sql)){
				// Bind variables to the prepared statement as parameters
				$stmt->bind_param("ss", $param_uid, $param_uname);
            
				// Set parameters
				$param_uid = $uid;
				$param_uname = $uname;
			
				// Attempt to execute the prepared statement
				if($stmt->execute()){
					$actionby = $_SESSION["username"];
					$status = "New Record Inserted Successfully.</br></br><a href='view.php'>View Inserted Record</a>";
					mysqli_query($mysqli,("INSERT INTO $loggerdb (EventType,ActionBy,AffectedID) VALUES ('INSERT', '$actionby', '$uid')"));
				}
			else{
                echo "Oops! Something went wrong. Please try again later.";
            }
        }
        // Close statement
		$stmt->close();
    }
    // Close connection
    $mysqli->close();
}
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>Insert New Record</title>
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css">
		<style type="text/css">
			body{ font: 14px sans-serif; }
			.wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }
		</style>
	</head>
	<body>
		<div class="wrapper">
			<div>
				<h1>Insert New Record</h1>
				<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
					<div class="form-group <?php echo (!empty($uid_err)) ? 'has-error' : ''; ?>">
						<p><input type="text" name="uid" placeholder="UserID" class="form-control" required /></p>
						<span class="help-block"><?php echo $uid_err; ?></span>
					</div>
					<div class="form-group <?php echo (!empty($uname_err)) ? 'has-error' : ''; ?>">
						<p><input type="text" name="uname" placeholder="UserName" class="form-control" required /></p>
						<span class="help-block"><?php echo $uname_err; ?></span>
					</div>
					<div class="form-group">
						<p><input name="submit" type="submit" value="Submit" class="btn btn-primary"/></p>
					</div>
				</form>
				<p style="color:#FF0000;"><?php echo $status; ?></p>
				<br /><br /><br /><br />
			</div>
			<div class="links">
				<p><a href="dashboard.php">Dashboard</a> | <a href="view.php">View Records</a> | <a href="logout.php">Logout</a></p>
			</div>
		</div>
	</body>
</html>
