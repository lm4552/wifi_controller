
# WiFi Controller

This a generic project to control GPIOs of an ESP32 development board from a webinterface. It allows to schedule time intervals per GPIO channel in different operating modes. The current operating modes allow to dimm and control devices via PWM, as well as disabling and enabling channels completely. Intervals can be repeated at arbitrary time intervals. Its also possible to define maximum values for PWM channels. The settings are always saved to the flash of the ESP32 and will be recovered in case of a power failure. In addition to that, the WiFI configuration of the ESP32 will by default be configured in Access Point(AP) mode. After connecting to the WiFI Controller AP and setting the SSID and password, it will automatically reboot and make the webinterface available to the local network. The project uses a simple REST API which uses HTTP POST and GET requests to exchange JSON objects.   

The project is an old project which I completely refactored. Therefore, I removed some functionality but also added cetain new features. I mainly used it for controlling different LED channels of aquarium lights via WiFi. However the code can be used for various applications and will be extended for a doser pump in the future, too. In case of the aquarium lights, each GPIO channel is attached to a specific LED channel. Besides a warm white and a cold white channel, there are often seperate red, green and blue channels. 

### Illustration
<div>
<div class="img_frame">

|<img src="./gif/configWiFi.gif" class="gif">|
|---|
|Configuration of the ESP32 WiFi settings in AP mode|
</div>
<div class="img_frame">

|<img src="./gif/configChannels.gif" class="gif">|
|---|
|Configuration of intervals and channels via the WiFi that was configured in AP mode|
</div>
</div></br>
