#include "FhemStatusDisplayWebServer.h"
#include <ESP8266mDNS.h>

FhemStatusDisplayWebServer::FhemStatusDisplayWebServer(FhemStatusDisplayConfig& config)
:
m_server(80),
m_config(config)
{
  m_updateServer.setup(&m_server);
}

void FhemStatusDisplayWebServer::begin()
{
  Serial.println("");
  Serial.println("Starting WebServer.");

  MDNS.begin(m_config.getHost());
  m_server.begin();
  MDNS.addService("http", "tcp", 80);
  m_server.on("/", std::bind(&FhemStatusDisplayWebServer::deliverRootPage, this));
  m_server.on("/colormapping", std::bind(&FhemStatusDisplayWebServer::deliverColorMappingPage, this));
  m_server.onNotFound(std::bind(&FhemStatusDisplayWebServer::deliverNotFoundPage, this));
}

void FhemStatusDisplayWebServer::handleClient()
{
  m_server.handleClient();
}

void FhemStatusDisplayWebServer::deliverRootPage()
{
  bool needSave = updateMainConfig();
  
  String html = ""
  "<!doctype html> <html>"
  "<head> <meta charset='utf-8'>"
  "<title>" + String(m_config.getHost()) + " main configuration</title>"
  "</head>"
  "<body bgcolor='#F0F0F0'><font face='Verdana,Arial,Helvetica'>"
  "<b><h1>" + m_config.getHost() + " main configuration</h1></b>"
  "<h4>Software version: " + m_config.getVersion() + "</h4>";
 
  if (WiFi.status() == WL_CONNECTED)
  {
    html += "Device is connected to WLAN <b>" + WiFi.SSID() + "</b> and has IP <b>" + ip2String(WiFi.localIP()) + "</b>.<br/><br/>";
  }
  else
  {
    html += "Device is not connected to local network yet.<br/><br/>";
  }

  html += ""
  "<a href='/'>Main</a> | <a href='/colormapping'>Color Mapping</a> | <a href='/devicemapping'>Device Mapping</a><br/>";

  html += ""
  "<form><font face='Verdana,Arial,Helvetica'>";

  html += ""
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>General</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Host name</td>"
  "  <td><input type='text' id='host' name='host' value='" + String(m_config.getHost()) + "' size='30' maxlength='40' placeholder='SSID'></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td><b><font size='+1'>WiFi</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>SSID</td>"
  "  <td><input type='text' id='wifiSSID' name='wifiSSID' value='" + String(m_config.getWifiSSID()) + "' size='30' maxlength='40' placeholder='SSID'></td>"
  " </tr>"
  " <tr>"
  "  <td>Password</td>"
  "  <td><input type='text' id='wifiPSK' name='wifiPSK' value='" + String(m_config.getWifiPSK()) + "' size='30' maxlength='40' placeholder='Password'></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td><b><font size='+1'>MQTT</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Server</td>"
  "  <td><input type='text' id='mqttServer' name='mqttServer' value='" + String(m_config.getMqttServer()) + "' size='30' maxlength='40' placeholder='IP Address'></td>"
  " </tr>"
  " <tr>"
  "  <td>Status topic</td>"
  "  <td><input type='text' id='mqttStatusTopic' name='mqttStatusTopic' value='" + String(m_config.getMqttStatusTopic()) + "' size='30' maxlength='40' placeholder='#'></td>"
  " </tr>"
  " <tr>"
  "  <td>Test topic</td>"
  "  <td><input type='text' id='mqttTestTopic' name='mqttTestTopic' value='" + String(m_config.getMqttTestTopic()) + "' size='30' maxlength='40' placeholder='#'></td>"
  " </tr>"
  " <tr>"
  "  <td>Will topic</td>"
  "  <td><input type='text' id='mqttWillTopic' name='mqttWillTopic' value='" + String(m_config.getMqttWillTopic()) + "' size='30' maxlength='40' placeholder='#'></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td><b><font size='+1'>LEDs</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Number of LEDs</td>"
  "  <td><input type='text' id='ledCount' name='ledCount' value='" + String(m_config.getNumberOfLeds()) + "' size='30' maxlength='40' placeholder='0'></td>"
  " </tr>"
  " <tr>"
  "  <td>LED pin</td>"
  "  <td><input type='text' id='ledPin' name='ledPin' value='" + String(m_config.getLedDataPin()) + "' size='30' maxlength='40' placeholder='0'></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='submit' value='Save' style='height:30px; width:200px' ></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='submit' value='Reboot' id='reset' name='reset' value='' style='height:30px; width:200px'></td>"
  " </tr>";

  html += ""
  " <tr>"
  "  <td></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='button' onclick=\"location.href='./update'\"  value='Update Firmware' style='height:30px; width:200px' ></td>"
  " </tr>";
  
  html += ""
  "</table>";

  html += ""
  "</form>";
  
  html += ""
  "</font></body></html>";

  m_server.send(200, "text/html", html);

  if(needSave)
  {
    Serial.println("Main config has changed, storing it.");
    m_config.saveMain();
  }

  if(m_server.hasArg("reset")) 
  {
    Serial.println("Rebooting ESP.");
    ESP.restart();
  }
}

void FhemStatusDisplayWebServer::deliverColorMappingPage()
{
  bool needSave = updateColorMappingConfig();
  
  String html = ""
  "<!doctype html> <html>"
  "<head> <meta charset='utf-8'>"
  "<title>" + String(m_config.getHost()) + " color mapping configuration</title>"
  "</head>"
  "<body bgcolor='#F0F0F0'><font face='Verdana,Arial,Helvetica'>"
  "<b><h1>" + m_config.getHost() + " color mapping configuration</h1></b>"
  "<h4>Software version: " + m_config.getVersion() + "</h4>";

  html += ""
  "<a href='/'>Main</a> | <a href='/colormapping'>Color Mapping</a> | <a href='/devicemapping'>Device Mapping</a><br/>";
  
  html += ""
  "<form><font face='Verdana,Arial,Helvetica'>";

  html += ""
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>Message</font></b></td>"
  "  <td><b><font size='+1'>Type</font></b></td>"
  "  <td><b><font size='+1'>Color</font></b></td>"
  "  <td><b><font size='+1'>Behavior</font></b></td>"
  " </tr>"
  " <tr>"
  "  <td><input type='text' id='name' name='name' value='" + String(m_config.getHost()) + "' size='30' maxlength='40' placeholder='name'></td>"
  "  <td><input type='text' id='type' name='type' value='" + String(m_config.getHost()) + "' size='30' maxlength='40' placeholder='type'></td>"
  "  <td><input type='text' id='color' name='color' value='" + String(m_config.getHost()) + "' size='30' maxlength='40' placeholder='color'></td>"
  "  <td><input type='text' id='behavior' name='behavior' value='" + String(m_config.getHost()) + "' size='30' maxlength='40' placeholder='behavior'></td>"
  " </tr>";
  
  html += ""
  " <tr>"
  "  <td></td>"
  "  <td></td>"
  "  <td></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td></td>"
  "  <td></td>"
  "  <td><input type='submit' value='Save' style='height:30px; width:200px' ></td>"
  " </tr>";

  html += ""
  "</table>";

  html += ""
  "</form>";
  
  html += ""
  "</font></body></html>";

  m_server.send(200, "text/html", html);

  if(needSave)
  {
    Serial.println("Config has changed, storing it.");
    m_config.saveColorMapping();
  }
}

void FhemStatusDisplayWebServer::deliverNotFoundPage()
{
  String html = "File Not Found\n\n";
  html += "URI: ";
  html += m_server.uri();
  html += "\nMethod: ";
  html += (m_server.method() == HTTP_GET)?"GET":"POST";
  html += "\nArguments: ";
  html += m_server.args();
  html += "\n";
  
  for (uint8_t i = 0; i < m_server.args(); i++)
  {
    html += " " + m_server.argName(i) + ": " + m_server.arg(i) + "\n";
  }
  
  m_server.send(404, "text/plain", html);
}

String FhemStatusDisplayWebServer::ip2String(IPAddress ip)
{
  char buffer[20];
  memset(buffer, 0, sizeof(buffer));

  sprintf(buffer,"%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  
  return String(buffer);
}

bool FhemStatusDisplayWebServer::updateMainConfig()
{
  bool needSave = false;

  if (m_server.hasArg("host"))
  {
    needSave |= m_config.setHost(m_server.arg("host").c_str());
  }
  
  if (m_server.hasArg("wifiSSID"))
  {
    needSave |= m_config.setWifiSSID(m_server.arg("wifiSSID").c_str());
  }
  
  if (m_server.hasArg("wifiPSK")) 
  {
    needSave |= m_config.setWifiPSK(m_server.arg("wifiPSK").c_str());
  }

  if (m_server.hasArg("mqttServer"))
  {
    needSave |= m_config.setMqttServer(m_server.arg("mqttServer").c_str());
  }
  
  if (m_server.hasArg("mqttStatusTopic"))
  {
    needSave |= m_config.setMqttStatusTopic(m_server.arg("mqttStatusTopic").c_str());
  }
  
  if (m_server.hasArg("mqttTestTopic")) 
  {
    needSave |= m_config.setMqttTestTopic(m_server.arg("mqttTestTopic").c_str());
  }

  if (m_server.hasArg("mqttWillTopic")) 
  {
    needSave |= m_config.setMqttWillTopic(m_server.arg("mqttWillTopic").c_str());
  }

  if (m_server.hasArg("ledCount"))
  {
    uint32_t ledCount = m_server.arg("ledCount").toInt();
    
    if(ledCount > 0)
    {
      needSave |= m_config.setNumberOfLeds(ledCount);
    }
  }
  
  if (m_server.hasArg("ledPin")) 
  {
    uint32_t ledPin = m_server.arg("ledPin").toInt();
    
    if(ledPin > 0)
    {
      needSave |= m_config.setLedDataPin(ledPin);
    }
  }

  return needSave;
}

bool FhemStatusDisplayWebServer::updateColorMappingConfig()
{
  return true;
}

