<?php include_once( "DefaultSettings.php" ); ?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
  <head>
   <link rel="stylesheet" type="text/css" href="<?php echo $css ?>">
    <title>Output not found</title>
  </head>
  <body><h1>Output not found</h1>
 <p>Sorry, I can't find the pdf-file you are looking for. This can
 have one of following reasons:</p>
 <ul>
      <li><p>Output not finished. Please give TeX some time for
      processing the file. Check the output window if the TeX run is
      complete. If everything went ok, you should
        see <kbd>Output written on texweb.pdf</kbd>.</p></li>
      <li><p>Error in source file. Check the output window to see if
        there is an error message. On en error, there is usually a
        message like <kbd>! Emergency stop. </kbd></p></li>
      <li><p>File size limit exeeded. You cannot create very large
      files. If there is no good error message, and your input file
      is pretty large, this might be the problem.</p></li>
    </ul>
</body>
</html>
