<?php
    $file = fopen("/home/pi/Desktop/Values/rotationmax.txt", "w");
    $value = $_POST["rotationMaxTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>
