<?php
    $file = file_get_contents("/home/pi/Desktop/Values/Telemetry/mode.txt", "r");
    $mode = $_GET[$file];
    fclose($file);
?>