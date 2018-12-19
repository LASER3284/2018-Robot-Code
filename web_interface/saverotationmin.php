<?php
    $file = fopen("/home/pi/Desktop/Values/rotationmin.txt", "w");
    $value = $_POST["rotationMinTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>
