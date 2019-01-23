<?php 
	require('config.php');
	session_start();
	if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
		header("location: login.php");
		exit;
	}
	
	$status="";
	$uid = $uname = "";
	$uid_err = $uname_err = "";

	if (isset($_GET['uid']))
	{
		$uid=base64_decode(urldecode($_GET['uid']));
	}
	if($_SERVER["REQUEST_METHOD"] == "POST"){
	
		// Check if uname is empty
		if(empty(trim($_POST["uid"]))){
			$uid_err = 'Please enter UserID.';
		} else{
			$uid = trim($_POST["uid"]);
		}
		
		// Check if password is empty
		if(empty(trim($_POST["uname"]))){
        $uname_err = 'Please enter UserName.';
		} else{
			$uname = trim($_POST["uname"]);
		}
		// Validate credentials
		if(empty($uid_err) && empty($uname_err)){
			// Prepare a select statement
			$sql = "UPDATE $generaldb SET userid = ?, username = ? WHERE userid ='$uid'";
			
			if($stmt = $mysqli->prepare($sql)){
				// Bind variables to the prepared statement as parameters
				$stmt->bind_param("ss", $param_uid, $param_uname);
				
				// Set parameters
				$param_uid = $uid;
				$param_uname = $uname;
            // Attempt to execute the prepared statement
            if($stmt->execute()){
				$actionby = $_SESSION["username"];
				mysqli_query($mysqli,("INSERT INTO $loggerdb (EventType,ActionBy,AffectedID) VALUES ('UPDATE', '$actionby', '$uid')"));
                $status = "Record Updated Successfully.</br></br><a href='view.php'>View Inserted Record</a>";
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
		<title>Update Record</title>
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css">
		<style type="text/css">
			body{ font: 14px sans-serif; }
			.wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }
		</style>
	</head>
	<body>
		<div class="wrapper">
			<div>
				<h1>Update Record</h1>
				<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
					<div class="form-group <?php echo (!empty($uid_err)) ? 'has-error' : ''; ?>">
						<p><input type="text" name="uid" placeholder="UserID" class="form-control" required value="<?php echo $uid;?>" /></p>
						<span class="help-block"><?php echo $uid_err; ?></span>
					</div>
					<div class="form-group <?php echo (!empty($uname_err)) ? 'has-error' : ''; ?>">
						<p><input type="text" name="uname" placeholder="UserName" class="form-control" required value="<?php echo $uname;?>" /></p>
						<span class="help-block"><?php echo $uname_err; ?></span>
					</div>
					<div class="form-group">
						<p><input name="submit" type="submit" class="btn btn-primary" value="Update" /></p>
					</div>
				</form>
				<p style="color:#FF0000;"><?php echo $status; ?></p>
				<br /><br /><br /><br />
			</div
			<div class="links">
				<p><a href="dashboard.php">Dashboard</a> | <a href="insert.php">Insert New Record</a> | <a href="logout.php">Logout</a></p>
			</div>
		</div>
	</body>
</html>
