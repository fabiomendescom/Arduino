# arduino-upnp
UPnP implementation for Arduino, and on ESP 8266

This is a library containing a couple of simple classes to implement a UPnP device on an ESP8266.

The demo is a MotionSensorService class which can be detected with SSDP :
<pre><code>
  dannylaptop% lsupnp
  ...
  Device #5 {UPnP Motion Sensor Kit}, url http://192.168.1.100:80/description.xml
    Service {urn:danny-backx-info:serviceId:sensor1}, url /scpd.xml
            type urn:danny-backx-info:service:sensor:1
</code></pre>
and that produces (debug) output such as this when queried
<pre><code>
  Boot version 31
  Flash chip Real size 4194304, size 4194304
  SDK version 1.3.0
  Starting WiFi... XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX IP address 192.168.1.100
  Mode: STA
  PHY mode: N
  Channel: 11
  AP id: 0
  Status: 5
  Auto connect: 1
  Starting HTTP...
  Starting SSDP...
  UPnPService::begin(), this 3ffed1e0, srv 3ffed1e0
  MotionSensorService::begin
  Ready!
  MotionSensorService::GetStateHandler
  MotionSensorService::GetStateHandler
</code></pre>

Sample queries are provided in examples/UPnP/scripts . Note that the getVersion script invokes a function that should return XML, but I'm a bit lazy : the output isn't valid XML.

This is the output of the example that does produce sensible XML :
<pre><code>
  Query 192.168.1.100 ...
  &lt;?xml version="1.0" encoding="utf-8"?&gt;
  &lt;s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"&gt;
  &lt;s:body&gt;
  &lt;u:GetStateResponse xmlns="urn:danny-backx-info:service:sensor:1"&gt;
  &lt;State&gt;&lt;/State&gt;
  &lt;/u:GetStateResponse&gt;
  &lt;/s:body&gt;
  &lt;/s:Envelope&gt;
</code></pre>
The only hardware you need for this is an ESP8266 device and a motion sensor.

You can build this application via the Makefiles supplied, or from the regular Arduino IDE.
Either way, you'll need to supply a file called <pre>mywifi.h</pre> in each application directory, with contents such as
<pre><code>
  #define MY_SSID "my-wifi-ssid"
  #define MY_WIFI_PASSWORD "secret"
</code></pre>
