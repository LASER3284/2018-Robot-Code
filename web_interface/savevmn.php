<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/vmn.txt", "w");
    $value = $_POST["vmnTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>
