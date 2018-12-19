<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/smn.txt", "w");
    $value = $_POST["smnTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>

