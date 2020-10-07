<h1> Working with FreeRtos and ESP32 </h1>

<h3> Description: </h3>
<p> <b>t00</b> - oled display brightness is getting changed depending on the state of photoresistor. More light on photoresistor - less bright oled display and vice versa.</p> 
<p> <b>t01</b> - simple <b>CLI</b>, which supports commands to manipulate with leds.</p> 
<p> <b>t02</b> - tehu command added to <b>CLI</b>. tehu - prints dht11 temperature/humidity sensor log. dht11 sensor is getting monitored every 5 seconds. Last 60 values are stored in RAM.</p>
<p> <b>t03</b> - time(prints current time) command added to <b>CLI</b>. Implemented digital clock. When user turns on board, he/she should set current time with help of `time set hh mm ss`. Current time is also displayed on OLED display.</p> 
<p> <b>t04</b> - sound [on/off] command added. <b>sound on</b> - makes sound(noise:D) with help of <b>DAC</b>. Sound data is transmitted to <b>DAC</b> in <b>DMA</b> mode by <b>I2S</b></p>
<p> <b>creative</b> - </p>