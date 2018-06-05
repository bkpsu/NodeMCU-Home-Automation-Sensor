# NodeMCU-Home-Automation-Sensor (a.k.a. The Kube)
Code for Thingiverse item https://www.thingiverse.com/thing:2539897 (The Kube - a NodeMCU/DHT22 based MQTT temp/humidity sensor with local OLED display)

Note: The parts list I posted on the Thingiverse site (and below) contains affiliate links to Amazon. These are the parts I've used/tested in building my sensors, and if you wish to support my designs, please go ahead and use them to order parts. Thank you and enjoy!

[![Demo & Overview](https://img.youtube.com/vi/VefbT6RuT-4/0.jpg)](https://www.youtube.com/watch?v=VefbT6RuT-4)

# Parts List
  *3D printed enclosure (https://www.thingiverse.com/thing:2539897)  
  *OLED screen (http://amzn.to/2xR4iQP)  
  *NodeMCU breakout board (https://goo.gl/U2fY7y)  
  *NodeMCU ESP8266 dev board v0.9 (http://amzn.to/2hd6RJk) or 1.0 (http://amzn.to/2ymAkak).  
  *DHT-22 sensor (http://amzn.to/2ymmODK)  
  *headers (http://amzn.to/2fNe81C)  
  *solid hook-up wire, 22awg (https://amzn.to/2voliAb)

# Build Procedure

Initial Build

1. Print enclosure and cover
2. Solder left/right inner headers onto the OSHPark carrier board (15 pins each)
3. Solder DHT22 sensor onto board, with openings facing out
4. Stick NodeMCU board onto carrier board, USB connector facing opposite from DHT22
5. De-pin OLED screen (cut the black plastic holding pins together, remove pins one by one) <- alternatively, you can use breadboard wires (http://amzn.to/2fDGqeJ) with connectors on each end if you don't want to de-pin, but you'll have to bend the OLED screen pins to the side.
6. Cut seven (7) short (~2 inch) lengths of hook-up wire and strip on each end.
7. Connect/solder OLED screen to carrier board as follows:

For SPI OLED:

| Board | Wire Color | OLED  |  
|:-------:|:-----:|:-----:|  
| 3v3 |  Red | VCC|    
| Gnd |  Black  | Gnd |  
| D10 |  Orange |  Res |  
| D9 |  Gray  |  DC  |  
| D8 |  White  |  CS  |  
| D7 | Yellow  | D1  |  
| D5 | Brown  |  D0  |  

For I2C OLED:

![Diagram](https://github.com/bkpsu/NodeMCU-Home-Automation-Sensor/raw/master/OLED%20I2C%20Hookup.png)

| Board | Wire Color | OLED  |  
|:-------:|:-----:|:-----:|  
| 3v3 |  Red | VCC|    
| Gnd |  Black  | Gnd | 
| D10 | Blue | SCL |
| D9 | Green | SDA |  


Final Assembly:

1. Stick OLED screen onto mounting posts (ensuring the screen is within the enclosure opening) and use a soldering iron to melt the posts over it.

2. Put NodeMCU carrier board in, ensuring it fits snugly (DHT sensor is tightly within its opening, USB connector is visible)

3. Put enclosure cover on, and secure with small screws (or glue in place)

[![Hardware Build Video](https://img.youtube.com/vi/fA91LcJRbhI/0.jpg)](https://www.youtube.com/watch?v=fA91LcJRbhI)

# Program sensor
Just follow the video below to program the sensor in Arduino IDE using the code provided in this repository.
[![Arduino Programming Video](https://img.youtube.com/vi/uDsnqi1Vl4U/0.jpg)](https://img.youtube.com/vi/uDsnqi1Vl4U)

# Connect to openHAB and Calibrate
Now that we have the sensor built and running, we have to connect it 

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTkyODQzMDY4NCwtNTQ3OTU4OTc3XX0=
-->