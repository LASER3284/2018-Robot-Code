# 2018 Robot Code
Robot Code from the 2018 season, Power Up, for 3284's Howitzer.
## Branches
**"master" branch** contains most development and changes. All other branches should be forked from master.
<br>
**"release" branch** contains the most current _stable_ code. This code should be tested thoroughly and working.
<br>
**"vision" branch** contains our Vision code for running on the Raspberry Pi for 2018.
<br>
### Getting it to run
Generally, you can compile it however you'd like using FRC Toolchains.
<br>
<br>
# Vision Processing
Current working version of object tracking with the Raspberry Pi 3. Also includes new version equiped
with multi-threading. Built with Python 2.7 using OpenCV 3.3 libraries and [pynetworktables](https://github.com/robotpy/pynetworktables) with [monotonic](https://pypi.org/project/monotonic/#files).
## Other Folders:
###  Values:
  This folder is meant to be placed on the desktop of the Raspberry Pi allowing the program
  to organize and store track-bar values.
###  Overclocking:
  This folder contains files that need to replace other files on the filesystem of the Raspberry Pi.
  Replace the config.txt in the `/boot/` directory and place the other files on the desktop(this is the overclocking settings).
  Make sure to add cpu_governor.sh to autostart in `/etc/rc.local` file (remove CPU throttling).
    
Also note, if you want the vision program to autostart add the directory and command to start it to the rc.local in `/etc/`.

### Running Vision Tracking:
  To run the program make sure to `chmod a+x` the entire directory containing the vision files. This allows the program to access 
  the files storing the track-bar values. If you need help with the arguments to pass in just type: `sudo python VisionMain.py -h` (output is below.)
    
```console
usage: VisionMain.py [-h] [--source SOURCE] [--thread THREAD] [--gui GUI]

optional arguments:
    -h, --help            show this help message and exit
    --source SOURCE, -s SOURCE
                            Path to video file or integer representing webcam
                            index (default 0).
    --thread THREAD, -t THREAD
                            Threading mode: both (video read and video show in
                            their own threads), none (default--no multithreading)
    --gui GUI, -g GUI     Graphical User Interface: yes (show video windows for 
                            vision tuning), no (default, run supa fast)
```
