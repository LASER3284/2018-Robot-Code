<?php
    $file = fopen("/home/pi/Desktop/Values/area.txt", "w");
    $value = $_POST["areaTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>
