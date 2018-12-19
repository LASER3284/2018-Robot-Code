<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/hmx.txt", "w");
    $value = $_POST["hmxTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>

