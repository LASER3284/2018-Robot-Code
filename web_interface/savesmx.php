<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/smx.txt", "w");
    $value = $_POST["smxTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>

