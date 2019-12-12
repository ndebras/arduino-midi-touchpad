# arduino-midi-touchpad
An arduino based touchpad MIDI over USB device

Author : Nicolas Debras ([nicolas@debras.fr](email))

![Arduino MIDI Touchpad](https://raw.githubusercontent.com/ndebras/arduino-midi-touchpad/master/documentation/images/arduino_midi_touchpad.jpg)

This is a touchpad sketch turning an Arduino UNO and a Adafruit 2.8'' TFT Touchscreen into a  MIDI device able to send  control change message over USB.
  
It is based on this article : [https://www.instructables.com/id/Turn-your-Arduino-Uno-into-an-USB-HID-Mididevice/](url)

It uses the following libraries : 
  - [https://github.com/ddiakopoulos/hiduino](url)
  - [https://github.com/adafruit/Adafruit-GFX-Library](url)
  - [https://github.com/FortySevenEffects/arduino_midi_library](url)

Arduino UNO R3 is available here : 
 [https://shop.mchobby.be/fr/carte-arduino-officielle/10-arduino-uno-r3e-atmega328-assemble-3232100000100-arduino.html?search_query=arduino+uno&results=485](url)
 
TFT shield available here :
 [https://shop.mchobby.be/fr/shields/384-ecran-tactile-28-tft-pour-arduino-3232100003842-adafruit.html?search_query=arduino+TFT+shield&results=560](url)

## The Arduino Code

The code (src/arduino-midi-touchpad/arduino-midi-touchpad.ino) is, I think :-), well commented and self explanatory. If there is something unclear, if you have any suggestions, remarks or optimisations, feel free to contact me : nicolas@debras.fr

## Flashing the UNO USB controler

As I'm using a Macbook, the explanation here are based on the tools I'm using. The is an equivalent for the Linux and Windows world too, but can't test them for now (If someone can document this, please do!)

The tricky part of this small project is flashing the USB controller of the Arduino UNO R3 to turn it into a class compliant MIDI device.

### AVRDude

First you need to acquire a AVRISP programmer available for a few euros on all electronic e-shop (I bought mine here, it worked straight forward on my MacBook Pro : [https://www.electronic-shop.lu/product/135800](url)) 

Then you have to install `avrdude`, which is the tool you're going to use to properly flash the USB controler of the UNO. Just follow step by step the information given on this page : [http://macappstore.org/avrdude/](url), it's quite simple.

You need to know is that, once the USB controler of the Arduino will be acting as a MIDI device, you won't be able to send code updates using the Arduino  IDE anymore. So, for now, we will have to flash it to MIDI and revert it along the development process. I read that there is a better way of doing but haven't managed to do it for now.


### Flashing the USB controler to MIDI

![Arduino AVR ISP Flash](https://raw.githubusercontent.com/ndebras/arduino-midi-touchpad/master/documentation/images/arduino_avr_isp_flash.jpg)

What we wanna do know is to get the USB microcontroler code the will make it act as a MIDI class compliant controler. To do that git clone this repository : [https://github.com/ddiakopoulos/hiduino](url)
The go in the `hiduino/compiled_firmwares` directory.
Find out to which USB port your AVR ISP Programmer is plugged to. On MacOS, just have a look at the /dev/ directory with `ls /dev/`. In my case, it is `/dev/cu.SLAB_USBtoUART`. So, adapt the -P argument and execute the following command in the terminal :  

`avrdude -p at90usb82 -F -P /dev/cu.SLAB_USBtoUART -c avrispmkii -U flash:w:arduino_midi.hex -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xF4:m -U lock:w:0x0F:m`

Your Arduino should now be recognized as a MIDI device by computer. 
![Arduino as a MIDI USB Device](https://raw.githubusercontent.com/ndebras/arduino-midi-touchpad/master/documentation/images/arduino_midi_device_macos.jpg)

### Revert to USB controler to it's original state

The original USB microcontroler code can be found in the Arduino IDE package. On MacOS it is located here : `/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/firmwares/atmegaxxu2`

To revert the USB microcontroler of your Arduino UNO R3 back to the original, adapt the -P argument and execute : 
`avrdude -p at90usb82 -F -P /dev/cu.SLAB_USBtoUART -c avrispmkii -U flash:w:UNO-dfu_and_usbserial_combined.hex -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xF4:m -U lock:w:0x0F:m`

## 3D Printing the case (Optional)

(To do)