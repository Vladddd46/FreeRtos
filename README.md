<h1> Working with FreeRtos and ESP32 </h1>

<h3> Description: </h3>
<p> <b>t00</b> - oled display brightness is getting changed depending on the state of photoresistor. More light on photoresistor - less bright oled display and vice versa.</p> 
<p> <b>t01</b> - simple CLI, which supports commands to manipulate with leds.</p> 
<p> <b>t02</b> - tehu command added to CLI. tehu - prints dht11 temperature/humidity sensor log. dht11 sensor is getting monitored every 5 seconds. Last 60 values are stored in RAM.</p>
<p> <b>t03</b> - time(prints current time) command added to CLI. Implemented digital clock. When user turns on board, he/she should set current time with help of `time set hh mm ss`</p> 
