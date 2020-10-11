<h1> Working with FreeRtos and board ESP32 </h1>

<h3> Description: </h3>
<b>t00</b> - oled display brightness is getting changed depending on the state of photoresistor. More light on photoresistor - less bright oled display and vice versa.<br>

![ezgif com-gif-maker (1)](https://user-images.githubusercontent.com/11888485/95690748-8b466580-0c22-11eb-8706-24b9bb175011.gif)

<b>t01</b> - simple <b>CLI</b>, which supports commands for led manipulation.<br>
<b>t02</b> - tehu command added to <b>CLI</b>. tehu - prints <b>dht11</b> temperature/humidity sensor log. <b>dht11</b> sensor is getting monitored every 5 seconds. Last 60 values are stored in <b>RAM</b>.<br>
<b>t03</b> - time(prints current time) command added to <b>CLI</b>. Implemented digital clock. When user turns on board, he/she should set current time with help of `time set hh mm ss`. Current time is also displayed on <b>OLED</b> display.<br>
 <b>t04</b> - sound [on/off] command added. <b>sound on</b> - makes sound(noise:D) with help of <b>DAC</b>. Sound data is transmitted to <b>DAC</b> in <b>DMA</b> mode by <b>I2S</b><br>
 <b>creative</b> - temperature, humidity and time are displayed on oled. <b>CLI</b> supports the following commands:
	<ul>
		<li><b>help</b></li>
		<li><b>tehu</b> - temperature and humidity log.</li>
		<li><b>sound on/off</b> - enables/disables sound(noise).</li>
		<li><b>time set/alarm</b> - setting time/alarm.</li>
		<li><b>led on/off/pulse</b> - turning on/off led; make led(s) pulsing.</li>
	</ul> 

