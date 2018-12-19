<?php
    $file = fopen("/home/pi/Desktop/Values/FreeMode/hmn.txt", "w");
    $value = $_POST["hmnTrackbarValue"];
    fwrite($file, $value);
    fclose($file);
?>
