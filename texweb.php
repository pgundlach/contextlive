<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html> 
 <head> 
  <title>PHP Test</title> 
 </head> 
 <body> 
<?php
 include_once( "DefaultSettings.php" );
 $tempdir=`$mktemp`; 
 echo $_SERVER['REQUEST_METHOD'];
?>
<p>nothing</p>
</body> 
</html> 
