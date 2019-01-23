<?php
session_start();
if(!isset($_SESSION['username']) || empty($_SESSION['username'])){
  header("location: login.php");
  exit;
}
require('config.php');
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta name="viewport" content="width=device-width, initial-scale=1">
		<title>View Records</title>
		<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.css">
	</head>
	<body>
		<div class="form">
			<p><a href="dashboard.php">Home</a> | <a href="insert.php">Insert New Record</a> | <a href="logout.php">Logout</a></p>
			<h2>View Records</h2>
			<table class="table table-bordered table-hover">
				<thead>
					<tr>
						<th><strong>S.No</strong></th>
						<th><strong>UserID</strong></th>
						<th><strong>UserName</strong></th>
						<th><strong>Edit</strong></th><th>
						<strong>Delete</strong></th>
					</tr>
				</thead>
				<tbody>
					<?php
						$count=1;
						$sel_query="Select * from $generaldb;";
						$result = mysqli_query($mysqli,$sel_query);
						while($row = mysqli_fetch_assoc($result)) { ?>
							<tr>
								<td align="center"><?php echo $count; ?></td>
								<td align="center"><?php echo $row["UserID"]; ?></td>
								<td align="center"><?php echo $row["UserName"]; ?></td>
								<td align="center"><a href="edit.php?uid=<?php echo urlencode(base64_encode($row["UserID"])); ?>">Edit</a></td>
								<td align="center"><a href="delete.php?uid=<?php echo urlencode(base64_encode($row["UserID"])); ?>">Delete</a></td>
							</tr>
						<?php $count++; } ?>
				</tbody>
			</table>
			<br /><br /><br /><br />
		</div>
	</body>
</html>
