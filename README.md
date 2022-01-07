
# Aquarium Light

To use the <a href="#/pages/projects/wifi_controller/readme.html">WiFi Controller</a> project for aquarium lights I slightly modified the [wifi_controller main git branch ](https://github.com/lm4552/wifi_controller/tree/master) and placed it in a new [aquarium_light git branch ](https://github.com/lm4552/wifi_controller/tree/aquarium_light).
For aquarium lights each GPIO channel of the controller gets attached to a specific LED channel by an XY-MOS module. The hardware setup, schematic, customized webinterface and more are shown below. 

[See project on GitHub](https://github.com/lm4552/wifi_controller/tree/aquarium_light)

## Webinterface

The webinterface allows to click or hover on a timeline graph to show the daily intervals and their purpose (e.g. dimming the light down, lunch break etc. ).
By the indices shown in the graph it is possible to adjust the settings in the table below the graph.
The brightness can be adjusted on a per LED channel basis to control the color temperature and spectrum. 

<div>
<div class="img_frame">

|![](./img/img4.jpg)|
|---|
|Webinterface on a smartphone|
</div>
<div class="img_frame">

|![](./img/img3.png)|
|---|
|Webinterface on a linux computer|
</div>
</div>

## Schematic 

<img alt="Schematic" src="./img/wifi_controller.svg" width="900px"/> 

## Images

<div>
<div class="img_frame">

|![](./img/img1.jpg)|
|---|
|Hardware inside the case|
</div>
<div class="img_frame">

|![](./img/img2.jpg)|
|---|
|Circuit with all modules wired up|
</div>
</div>
