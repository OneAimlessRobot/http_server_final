// Source - https://stackoverflow.com/a/4561168

// Posted by St.Woland

// Retrieved 2026-02-14, License - CC BY-SA 2.5



<?php
$testGD = get_extension_funcs("gd"); // Grab function list 
if (!$testGD){ echo "GD not even installed."; exit; }
echo"<pre>".print_r($testGD,true)."</pre>";
