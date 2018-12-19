<html>
	<head>
		<title>Vision Tracking Webconsole</title>
		<style>
                    h1 {
                        font-family: arial;
			color:#5e3474;
                    }
                    h2 {
                        font-family: arial;
                        color:#3edf02;
                    }
                    h3 {
                        font-family: arial;
			color:#f68a23;
                    }
                    p {
                        font-family: times;
                        color: black;
                    }
                    .slidecontainer {
                        width: 50%;
                    }
                    div {
                        height: auto;
                    }
                    .slider {
                        -webkit-appearance: none;
                        width: 100%;
                        height: 15px;
                        border-radius: 5px;
                        background: #d3d3d3;
                        outline: none;
                        opacity: 0.7;
                        -webkit-transition: .2s;
                        transition: opacity .2s;
                    }

                    .slider:hover {
                        opacity: 1;
                    }

                    .slider::-webkit-slider-thumb {
                        -webkit-appearance: none;
                        appearance: none;
                        width: 25px;
                        height: 25px;
                        border-radius: 50%;
                        background: #5e3474;
                        cursor: pointer;
                    }

                    .slider::-moz-range-thumb {
                        width: 25px;
                        height: 25px;
                        border-radius: 50%;
                        background: #5e3474;
                        cursor: pointer;
                    }
                    
                    * {
                        box-sizing: border-box;
                    }

                    .column {
                        padding: 5px;
                    }

                    .row::after {
                        content: "";
                        clear: both;
                        display: table;
                    }
                </style>
	</head>
	<body style="background-color:#292929">
            <div style="padding:30px; text-align:center" class="row">
                <div style="float:left" class="column">
                    <img src="first_logo.png" alt="firstlogo" width="200">
                </div>
                <div style="float:right" class="column">
                    <img src="raspberrypi_logo.png" alt="raspberrypilogo" width="150">
                </div>
                <div style="display:inline-block; padding:5px">
                    <img src="2017-shirt-front-3284.png" alt="laserlogo" width="300">
                </div>
            </div> 
	    <center><h1><b>LASER 3284's Vision Tuning Dashboard</b></h1></center>	
				<br>
				<br>
		<fieldset style="margin:0"><legend><h2><b>Tuning</b></h2></legend>
			<div style="padding:30px;" class="row">
                        <fieldset style="float:left; width:50%; height:auto; vertical-align:center; margin:0; position:relative; box-sizing:border-box"><legend><h2><b>HSV Trackbars<b></h2></legend>
                            <div class="column">                
                                <left><b><h3>HueComp</h3></b></left>
                                <div class="slidecontainer">
                                    <p>HMIN Value: <span id="hmn"></span></p>
                                    <input type="range" min="0" max="179" value="50" class="slider" id="hmnValue" />
                                </div>
                                <script>
                                    var hmnSlider = document.getElementById("hmnValue");
                                    var hmnOutput = document.getElementById("hmn");
                                    hmnOutput.innerHTML = hmnSlider.value;

                                    hmnSlider.oninput = function() {
                                        hmnOutput.innerHTML = this.value;
                                        document.hmnForm.hmnTrackbarValue.value = hmnSlider.value;
                                        document.getElementById("hmnFormID").submit();
                                    }
                                </script>
                                <form action="savehmn.php" method="POST" id="hmnFormID" name="hmnForm" target="hidden-form">
                                    <input type="hidden" name="hmnTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>  
                                <div class="slidecontainer">
                                <p>HMAX Value: <span id="hmx"></span></p>
                                <input type="range" min="0" max="179" value="50" class="slider" id="hmxValue">
                                </div>
                                <script>
                                    var txtFile  = "";
                                    var hmxSlider = document.getElementById("hmxValue");
                                    var hmxOutput = document.getElementById("hmx");
                                    hmxOutput.innerHTML = hmxSlider.value;

                                    hmxSlider.oninput = function() {
                                        hmxOutput.innerHTML = this.value;
                                        document.hmxForm.hmxTrackbarValue.value = hmxSlider.value;
                                        document.getElementById("hmxFormID").submit();
                                    }
                                </script>
                                <form action="savehmx.php" method="POST" id="hmxFormID" name="hmxForm" target="hidden-form">
                                    <input type="hidden" name="hmxTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                        <br>
                                <left><b><h3>SatComp</h3></b></left>
                                <div class="slidecontainer">
                                    <p>SMIN Value: <span id="smn"></span></p>
                                    <input type="range" min="0" max="179" value="50" class="slider" id="smnValue">
                                </div>
                                <script>
                                    var smnSlider = document.getElementById("smnValue");
                                    var smnOutput = document.getElementById("smn");
                                    smnOutput.innerHTML = smnSlider.value;

                                    smnSlider.oninput = function() {
                                        smnOutput.innerHTML = this.value;
                                        document.smnForm.smnTrackbarValue.value = smnSlider.value;
                                        document.getElementById("smnFormID").submit();
                                    }
                                </script>
                                <form action="savesmn.php" method="POST" id="smnFormID" name="smnForm" target="hidden-form">
                                    <input type="hidden" name="smnTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                <div class="slidecontainer">
                                    <p>SMAX Value: <span id="smx"></span></p>
                                    <input type="range" min="0" max="179" value="50" class="slider" id="smxValue">
                                </div>
                                <script>
                                    var smxSlider = document.getElementById("smxValue");
                                    var smxOutput = document.getElementById("smx");
                                    smxOutput.innerHTML = smxSlider.value;

                                    smxSlider.oninput = function() {
                                        smxOutput.innerHTML = this.value;
                                        document.smxForm.smxTrackbarValue.value = smxSlider.value;
                                        document.getElementById("smxFormID").submit();
                                    }
                                </script>
                                <form action="savesmx.php" method="POST" id="smxFormID" name="smxForm" target="hidden-form">
                                    <input type="hidden" name="smxTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                <left><b><h3>ValComp</h3></b></left>
                                <div class="slidecontainer">
                                    <p>VMIN Value: <span id="vmn"></span></p>
                                    <input type="range" min="0" max="179" value="50" class="slider" id="vmnValue">
                                </div>
                                <script>
                                    var vmnSlider = document.getElementById("vmnValue");
                                    var vmnOutput = document.getElementById("vmn");
                                    vmnOutput.innerHTML = vmnSlider.value;

                                    vmnSlider.oninput = function() {
                                        vmnOutput.innerHTML = this.value;
                                        document.vmnForm.vmnTrackbarValue.value = vmnSlider.value;
                                        document.getElementById("vmnFormID").submit();
                                    }
                                </script>
                                <form action="savevmn.php" method="POST" id="vmnFormID" name="vmnForm" target="hidden-form">
                                    <input type="hidden" name="vmnTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                <div class="slidecontainer">
                                    <p>VMAX Value: <span id="vmx"></span></p>
                                    <input type="range" min="0" max="179" value="50" class="slider" id="vmxValue">
                                </div>
                                <script>
                                    var vmxSlider = document.getElementById("vmxValue");
                                    var vmxOutput = document.getElementById("vmx");
                                    vmxOutput.innerHTML = vmxSlider.value;

                                    vmxSlider.oninput = function() {
                                        vmxOutput.innerHTML = this.value;
                                        document.vmxForm.vmxTrackbarValue.value = vmxSlider.value;
                                        document.getElementById("vmxFormID").submit();
                                    }
                                </script>
                                <form action="savevmx.php" method="POST" id="vmxFormID" name="vmxForm" target="hidden-form">
                                    <input type="hidden" name="vmxTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                            </div>
                        </fieldset>
                        
                        <fieldset style="float:right; width:50%; height:auto; vertical-align:top; margin:0; box-sizing:border-box"><legend><b><h2>Object Filtering</h2><b></legend>
                            <div class="column">
                                <left><b><h3>Max Contour Area</h3></b></left>
                                <div class="slidecontainer">
                                    <p>Area Value: <span id="contourArea"></span></p>
                                    <input type="range" min="0" max="100" value="50" class="slider" id="contourAreaValue">
                                </div>
                                <script>
                                    var contourAreaSlider = document.getElementById("contourAreaValue");
                                    var contourAreaOutput = document.getElementById("contourArea");
                                    contourAreaOutput.innerHTML = contourAreaSlider.value;

                                    contourAreaSlider.oninput = function() {
                                        contourAreaOutput.innerHTML = this.value;
                                        document.areaForm.areaTrackbarValue.value = contourAreaSlider.value;
                                        document.getElementById("areaFormID").submit();
                                    }
                                </script>
                                <form action="savearea.php" method="POST" id="areaFormID" name="areaForm" target="hidden-form">
                                    <input type="hidden" name="areaTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                        <br>
                                <left><b><h3>Range of Rotation</h3></b></left>
                                <div class="slidecontainer">
                                    <p>Minimum Rotation Value: <span id="rotationMin"></span></p>
                                    <input type="range" min="0" max="360" value="50" class="slider" id="rotationMinValue">
                                </div>
                                <script>
                                    var rotationMinSlider = document.getElementById("rotationMinValue");
                                    var rotationMinOutput = document.getElementById("rotationMin");
                                    rotationMinOutput.innerHTML = rotationMinSlider.value;

                                    rotationMinSlider.oninput = function() {
                                        rotationMinOutput.innerHTML = this.value;
                                        document.rotationMinForm.rotationMinTrackbarValue.value = rotationMinSlider.value;
                                        document.getElementById("rotationMinFormID").submit();
                                    }
                                </script>
                                <form action="saverotationmin.php" method="POST" id="rotationMinFormID" name="rotationMinForm" target="hidden-form">
                                    <input type="hidden" name="rotationMinTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                <div class="slidecontainer">
                                    <p>Maximum Rotation Value: <span id="rotationMax"></span></p>
                                    <input type="range" min="0" max="360" value="50" class="slider" id="rotationMaxValue">
                                </div>
                                <script>
                                    var rotationMaxSlider = document.getElementById("rotationMaxValue");
                                    var rotationMaxOutput = document.getElementById("rotationMax");
                                    rotationMaxOutput.innerHTML = rotationMaxSlider.value;

                                    rotationMaxSlider.oninput = function() {
                                        rotationMaxOutput.innerHTML = this.value;
                                        document.rotationMaxForm.rotationMaxTrackbarValue.value = rotationMaxSlider.value;
                                        document.getElementById("rotationMaxFormID").submit();
                                    }
                                </script>
                                <form action="saverotationmax.php" method="POST" id="rotationMaxFormID" name="rotationMaxForm" target="hidden-form">
                                    <input type="hidden" name="rotationMaxTrackbarValue" value="" />
                                </form>
                                <iframe style="display:none" name="hidden-form"></IFRAME>
                                    <br>
                                    <br>
                            </div>
                        </fieldset>
                            <div class="column">
                                <fieldset style="height:48%; vertical-align:center"><legend><b><h2>Camera Feed</h2></b></legend>
                                    <br>
                                </fieldset>
                            </div>
                        </div>
                </fieldset>
                                    <br>
                                    <br>
                <fieldset style="vertical-align:bottom; width:100%"><legend><h2><b>Telemetry</b></h2></legend>
                    <h3>Mode: 
                    <?php
                        $file = file_get_contents("/home/pi/Desktop/Values/Telemetry/mode.txt", "r");
                        echo $file;
                        fclose($file);
                    ?>
                    </h3>
                    <h3>Frequency:
                    <?php
                        $file = file_get_contents("/home/pi/Desktop/Values/Telemetry/speed.txt", "r");
                        echo $file;
                        fclose($file);
                    ?>
                    </h3>
                    <h3>Object Point Array:
                    <?php
                        $file = file_get_contents("/home/pi/Desktop/Values/Telemetry/pointarray.txt", "r");
                        echo $file;
                        fclose($file);
                    ?>    
                    </h3>
                    <h3>Trackbar Values:
                    <?php
                        $file = file_get_contents("/home/pi/Desktop/Values/Telemetry/trackbarvalues.txt", "r");
                        echo $file;
                        fclose($file);
                    ?>    
                    </h3>
                </fieldset>
	</body>
</html>