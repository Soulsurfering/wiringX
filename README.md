wiringX
========

wiringX is a modular approach to several GPIO interfaces.
wiringX will export all common GPIO functions also found libraries such as wiringPi
and wiringHB, but when using wiringX it will automatically determine what device 
your program is running on and use the appropriate GPIO functions. So when using 
wiringX, your program will just work in regard of GPIO functionality.

The wiringPi and wiringHB are almost a direct copy of their initial library.
However, wiringX currently does not yet support all features of the
Hummingboard and Raspberry Pi I/O. Therefore, wiringPi has been
stripped so it only supports those features also supported by wiringX.

Those features currently are:
- GPIO reading, writing, and interrupts.
- IC2 reading and writing.

The supported devices are:
- Raspberry Pi
- Hummingboard
- BananaPi
- Radxa Rock 
- MIPS CI20 Creator
- ODROID
- CHIP Computer

###Donations

donate@pilight.org

###Installation:

* Let it automatically build and generate a deb or rpm package:
```
#Make sure you have prerequisites
#For Debian based linuxes
sudo apt-get install build-essential
#For Red-Hat based linuxes
yum groupinstall "Development tools"

mkdir build
cd build
cmake ..
make
#From here you can also just do make install
#Or for Debian based linuxes
cpack -G DEB
#Or for Red-Hat based linuxes
cpack -G RPM
```
* To install the final package run:
```
#For Debian based linuxes
dpkg -i libwiringx*.deb
#For Red-Hat based linuxes
dpkg -i libwiringx*.rpm
```

wiringX is available in the Arch Linux ARM repository. To install, simply:
```
pacman -S wiringx-git
```
Pin numbering of the Raspberry Pi, Hummingboard, BananaPi, Radxa Rock and chip can be found here:
http://wiringx.org/

You can also see pin numbering with the example code wiringx-pinlist

```
root@ttn-gateway:~/wiringX/Soulsurfering/wiringX/build# ./wiringx-pinlist chip
GPIO mapping for chip
GPIO 099 => PD3
GPIO 100 => PD4
GPIO 101 => PD5
GPIO 102 => PD6
GPIO 103 => PD7
GPIO 106 => PD10
GPIO 107 => PD11
GPIO 108 => PD12
GPIO 109 => PD13
GPIO 110 => PD14
GPIO 111 => PD15
GPIO 114 => PD18
GPIO 115 => PD19
GPIO 116 => PD20
GPIO 117 => PD21
GPIO 118 => PD22
GPIO 119 => PD23
GPIO 120 => PD24
GPIO 121 => PD25
GPIO 122 => PD26
GPIO 123 => PD27
GPIO 128 => PE0
GPIO 129 => PE1
GPIO 130 => PE2
GPIO 131 => PE3
GPIO 132 => PE4
GPIO 133 => PE5
GPIO 134 => PE6
GPIO 135 => PE7
GPIO 136 => PE8
GPIO 137 => PE9
GPIO 138 => PE10
GPIO 139 => PE11
GPIO 408 => Valid but unknown
GPIO 409 => Valid but unknown
GPIO 410 => Valid but unknown
GPIO 411 => Valid but unknown
GPIO 412 => Valid but unknown
GPIO 413 => Valid but unknown
GPIO 414 => Valid but unknown
GPIO 415 => Valid but unknown
root@ttn-gateway:~/wiringX/Soulsurfering/wiringX/build#

```