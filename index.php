<!-- Last Change: Mon Aug  6 18:22:02 CEST 2007 -->
<?php
$posted=$_SERVER['REQUEST_METHOD'] == 'POST';
$notposted = ! $posted;
$debug=false;
$dryrun=false;
$form_engine="pdftex";
$doc_engine="pdftex"; # xetex
$doc_output="pdftex"; # xetex
#
if ($posted) {
	$form_engine = $_POST['engine'];
	if ($form_engine=='xetex') {
		$doc_engine='xetex';
		$doc_output='xetex';
#	} else if ($form_engine=='luatex') {
#		$doc_engine='luatex';
#		$doc_output='luatex';
#		$runtexexec = "/var/www/contextgarden.net/live/runtexexec4mkiv" ;
#		if($debug) { echo "Setting \$runtexexec to " . $runtexexec . "\n" ; }
	} else {
		$doc_engine='pdftex';
		$doc_output='pdftex';
	}
}
#
include_once( "DefaultSettings.php" );

# this is set in the apache config, but it shomehow doesn't get passed to the environment

if (!file_exists("/tmp/texweb")) { 
   mkdir("/tmp/texweb", 0777); 
}

// error_log("foobar");
if ($dryrun) {
  $tempdir="none.123456";
} else {
  $tempdir=rtrim(`$mktemp`) ;
  $filename = $tempdir . "/texweb.tex";
}
if ($posted) {
  // magic_quotes_gpc = Off is in php.ini (default)
  // so we must replace
  // two consecutive backslashes by one backslash. PHP sucks. :-(
  //  $texcontents=str_replace('\\\\', '\\', $_POST ['text']);
  // Now I have set magic_quotes_gpc = On in php.ini (/etc/php5/apache2/php.ini)
  $texcontents=$_POST['text'];
}
$suffix=substr($tempdir, -6);
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html> 
<link rel="stylesheet" type="text/css" href="<?php echo $css ?>">
  <head> 
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<script type="text/javascript">
<!--
var outputwindow;
function popUp() {
  <?php if ($dryrun) { 
     ?> var Quelle = "dummy.html"; <?php } else {
     ?> var Quelle = "/output.cgi?id=<?php echo $suffix ?>"; <?php } ?>
  outputwindow = window.open('', "OutputWindow", 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=500,height=700');
  outputwindow.location.href=Quelle;
}

function activate() {
popUp();
// outputwindow.location.reload();

document.getElementById("openoutput").setAttribute("href","javascript:popUp();"); 
document.getElementById("openoutput").removeAttribute("target");
}
//-->
</script>
    <title>ConTeXt live</title> 
   </head> 
   <?php if ($notposted) { ?>
  <body>
    <?php } else { ?>
    <body onLoad="javascript:activate()">
    <?php  } ?>
    <h1>context live @ contextgarden.net</h1>
<?php if ($posted) { ?>
    <a id="openoutput" href="/output.cgi?id=<?php echo $suffix ?>" target="texwebout">View transcript</a>
       or <a target="result" 
     href="/result.cgi?id=<?php echo $suffix?>">download the pdf-file</a>
        (wait for <code>texexec</code> to finish). 
<?php } else { ?>
    <!--<p><strong>Warning: </strong>The Live Garden is currently under maintenance. Expect the unexpectable and watch out for the droseras! <em>The Assistant Gardener</em></p>-->
    <p>Try out ConTeXt without installing it! Enter the source code
    in the form below and click on typeset. Once the output is 
     complete, you can download the pdf file.</p>
<?php }  ?>
<!-- <p><b>TeX on contextgarden is currently disabled. We try to bring it up as soon as possible. </b></p> -->
   <p>Versions used: ConTeXt: <?php 
     echo readlink("/opt/context/current"); 
      echo ", TeXlive: 2007"; 
?>   </p>
   <p>Please report problems to <em>gardeners <em>'at' </em> contextgarden.net</em></p>
   <form action="<?php $_SERVER["PHP_SELF"] ?>" method="post" target="_self">
    <table><tr><td><textarea name="text" cols="80" rows="20"><?php
if ($posted) {
  echo htmlspecialchars ($texcontents);
} else {
  ?>
\starttext
Hello world!
\stoptext
<?php } ?></textarea></td></tr><tr><td>
	<input type="radio" name="engine" value="pdftex" <?php if($form_engine=='pdftex') echo 'checked';?> />pdfTeX
	<input type="radio" name="engine" value="xetex"  <?php if($form_engine=='xetex' ) echo 'checked';?> />XeTeX
<?php #<input type="radio" name="engine" value="luatex"
	#if($form_engine=='luatex' ) echo 'checked';
	#/>LuaTeX (<?php echo readlink("/opt/luatex/inst/current") ;// (revision --> 
?><?php
  // $svn_entries = file_get_contents("/opt/luatex/.svn/entries") ;
  // $revs = array() ;
  // preg_match("/^[^\n]*\n[^\n]*\n[^\n]*\n([0-9]*)/", $svn_entries, &$revs) ;
  // echo $revs[1] ; ?>
	<br />
    <input type="submit" value="typeset">
    <input type="reset"  value="reset form"></td></tr></table>
  </form>
    <?php
if ($debug) {
  ?><pre><?php
  echo "tempdir= $tempdir\n";
  echo "CTX_TEXUTIL_EXTRAS = " . getenv("CTX_TEXUTIL_EXTRAS") . "\n";
  echo "filename = $filename\n";
  echo "suffix = $suffix\n";
  echo "posted = " . ($posted ? "true" : "false") . "\n";
  echo "notposted = " . ($notposted ? "true" : "false") . "\n";
  echo "engine = " . $doc_engine . "\n" ;
}

if ($posted and is_writable ($tempdir )) {

  if (! $handle =fopen ($filename ,'a' )) { 
    error_log("Cannot open file ($filename)") ;
    exit;
  }

  #if ( fwrite ($handle , $texcontents ) === FALSE ) {
  if ( fwrite ($handle , "% output=$doc_output engine=$doc_engine\n" . "\\setupexternalfigures[location={local,global,default}]\n" . $texcontents ) === FALSE ) {
     error_log ("Cannot write to file ($filename)") ;
     exit;
  }
  fclose ($handle ); 
  if($doc_engine == 'luatex') { $runtexexec = "/var/www/contextgarden.net/live/runtexexec4mkiv" ; }
  $cmdline=$runtexexec . " " . $suffix;
  if ($debug) { echo "$cmdline \n"; }
  if (! $dryrun) { exec($cmdline); }
} else { 
  if (! is_writable ($tempdir )) {
    error_log ("The file $filename is not writable") ;
  }
}

if ($debug) {
 ?></pre><?php
    }
?>

</body> 
</html> 
<!--  <body onLoad="javascript:popUp('/output.cgi?id=<?php echo $suffix ?>')"> -->

