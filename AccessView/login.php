<?php
	require "config.php";
	// Define variables and initialize with empty values
	$username = $password = "";
	$username_err = $password_err = "";
 
	// Processing form data when form is submitted
	if($_SERVER["REQUEST_METHOD"] == "POST"){
		// Check if username is empty
		if(empty(trim($_POST["username"]))){
			$username_err = 'Please enter username.';
		} 
		else{
			$username = trim($_POST["username"]);
		}
		// Check if password is empty
		if(empty(trim($_POST['password']))){
			$password_err = 'Please enter your password.';
		}
		else{
			$password = trim($_POST['password']);
		}
		// Validate credentials
		if(empty($username_err) && empty($password_err)){
			// Prepare a select statement
			$sql = "SELECT username, password FROM $admindb WHERE username = ?";
			if($stmt = $mysqli->prepare($sql)){
				// Bind variables to the prepared statement as parameters
				$stmt->bind_param("s", $param_username);
				// Set parameters
				$param_username = $username;
				// Attempt  to execute the prepared statement
				if($stmt->execute()){
					// Store result
					$stmt->store_result();
                
					// Check if username exists, if yes then verify password
					if($stmt->num_rows == 1){                    
						// Bind result variables
						$stmt->bind_result($username, $hashed_password);
						if($stmt->fetch()){
							if(password_verify($password, $hashed_password)){
								// If password is correct; start new session with username
								session_start();
								$_SESSION['username'] = $username;
								$actionby = $username;
								header("location: dashboard.php");
								mysqli_query($mysqli,("INSERT INTO $loggerdb (EventType,ActionBy) VALUES ('LOGIN', '$actionby')"));							} 
								
							else{
								// Display an error message if password is not valid
								$password_err = 'The password you entered was not valid.';
							}
						}
					} 
					else{
						// Display an error message if username doesn't exist
						$username_err = 'No account found with that username.';
					}
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
	//Handling logout message
	if ($_GET['msg'])
	{
       echo '<div class="success_message">' . base64_decode(urldecode($_GET['msg'])) . '</div>';
	}
?>

<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>Login</title>
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css">
		<style type="text/css">
			body{ font: 14px sans-serif; }
			.wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }
		</style>
	</head>
	<body>
		<div class="wrapper">
			<h2>Login</h2>
			<p>Please fill in your credentials to login.</p>
			<form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
				<div class="form-group <?php echo (!empty($username_err)) ? 'has-error' : ''; ?>">
					<label>Username</label>
					<input type="text" name="username" class="form-control" value="<?php echo $username; ?>">
					<span class="help-block"><?php echo $username_err; ?></span>
				</div>    
				<div class="form-group <?php echo (!empty($password_err)) ? 'has-error' : ''; ?>">
					<label>Password</label>
					<input type="password" name="password" class="form-control">
					<span class="help-block"><?php echo $password_err; ?></span>
				</div>
				<div class="form-group">
					<input type="submit" class="btn btn-primary" value="Login">
				</div>
			</form>
		</div>    
	</body>
</html>