# NodeMCU-Home-Automation-Sensor (a.k.a. The Kube)
Code for Thingiverse item https://www.thingiverse.com/thing:2539897 (The Kube - a NodeMCU/DHT22 based MQTT temp/humidity sensor with local OLED display)

Note: The parts list I posted on the Thingiverse site (and below) contains affiliate links to Amazon. These are the parts I've used/tested in building my sensors, and if you wish to support my designs, please go ahead and use them to order parts. Thank you and enjoy!

# Parts List
-3D printed enclosure (https://www.thingiverse.com/thing:2539897)

-OLED screen (http://amzn.to/2xR4iQP)

-NodeMCU breakout board (https://www.oshpark.com/shared_projects/jVBUOF0z)

-NodeMCU ESP8266 dev board v0.9 (http://amzn.to/2xd8Hvt) or 1.0 (http://amzn.to/2ymAkak).

-DHT-22 sensor (http://amzn.to/2ymmODK)

-headers (http://amzn.to/2fNe81C)

-solid hook-up wire, 22awg (http://amzn.to/2xbTaAW)

# Build Procedure

Initial Build

Print enclosure and cover

Solder left/right inner headers onto the OSHPark carrier board (15 pins each)

Solder DHT22 sensor onto board, with openings facing out

Stick NodeMCU board onto carrier board, USB connector facing opposite from DHT22

De-pin OLED screen (cut the black plastic holding pins together, remove pins one by one) <- alternatively, you can use breadboard wires (http://amzn.to/2fDGqeJ) with connectors on each end if you don't want to de-pin, but you'll have to bend the OLED screen pins to the side.

Cut seven (7) short (~2 inch) lengths of hook-up wire and strip on each end.

Connect/solder OLED screen to carrier board as follows:

For SPI OLED:

Board Wire Color OLED

3v3 Red VCC

Gnd Black Gnd

D10 Orange Res

D9 Gray DC

D8 White CS

D7 Yellow D1

D5 Brown D0


For I2C OLED:

Board Wire Color OLED

3v3 Red VCC

Gnd Black Gnd

D10 Yellow SCL

D9 Brown SDA


Final Assembly:

Stick OLED screen onto mounting posts (ensuring the screen is within the enclosure opening) and use a soldering iron to melt the posts over it.

Put NodeMCU carrier board in, ensuring it fits snugly (DHT sensor is tightly within its opening, USB connector is visible)

Put enclosure cover on, and secure with small screws (or glue in place)

Program sensor.

