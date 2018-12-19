<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/vmx.txt", "w");
    $value = $_POST["vmxTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>

