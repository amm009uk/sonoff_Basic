const char mainPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<title>Configure Settings</title>
<meta http-equiv='Content-Type' content='text/html;charset=UTF-8'/>
</head>
<body> 
<table border="0" width="400" align="center">
<tr>
<td>
<h2>sonoff - <label>version</label>{{_version}}</h2>
<form action='/saveChanges' method='POST' target="_self">
<fieldset>
<legend><h4>&nbsp;Device Details&nbsp;</h4></legend>
<table>
<tr>
<td><label>Device ID</label></td>
<td><input type="text" name="deviceID" size="30" title="A Unique ID to help you find device" value={{_deviceID}}></td>
</tr>
</table>
</fieldset>
<fieldset>
<legend><h4>&nbsp;MQTT Broker Details&nbsp;</h4></legend>
<table>
<tr>
<td><label>Connected ?</label></td>
<td><label>{{_MQTT}}</label></td>
</tr>
<tr>
<td><label>Server&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="text" name="mqtt_server" size="30" title="Broker address" value={{_mqtt_server}}></td>
</tr>
<tr>
<td><label>Server Port&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="text" name="mqtt_port" size="30" title="Broker port typically 1883" value={{_mqtt_port}}></td>
</tr>
<tr>
<td><label>User&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="text" name="mqtt_user" size="30" title="Broker username" value={{_mqtt_user}}></td>
</tr>
<tr>
<td><label>Password&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="password" name="mqtt_password" size="30" title="Broker password" value={{_mqtt_password}}></td>
</tr>
</table>
</fieldset>
<fieldset>
<legend><h4>&nbsp;Message Details&nbsp;</h4></legend>
<table>
<tr>
<td><label>Inbound&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="text" name="mqtt_inTopic" size="30" title="Eg. cmnd/Power/LivingRoom" value={{_mqtt_inTopic}}></td>
</tr>
<tr>
<td><label>Outbound&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</label></td>
<td><input type="text" name="mqtt_outTopic6" size="30" title="Eg. stat/Power/LivingRoom" value={{_mqtt_outTopic}}></td>
</tr>
</table>
</fieldset>
<br>
<table>
<tr>
<td><input type="submit" value="Save Changes" class="btn-save"></td>
<td><input type=button onClick="parent.location='/resetwifi'"   value='Reset WiFi'      class="btn-update"></td>
</tr>
</table>
</form>
</td>
</tr>
</table>
</body>
</html>
)=====";