#include "HSDWebserver.h"

HSDWebserver::HSDWebserver(HSDConfig& config, const HSDLeds& leds, const HSDMqtt& mqtt)
:
m_server(80),
m_config(config),
m_leds(leds),
m_mqtt(mqtt),
m_html(),
m_deviceUptimeMinutes(0)
{
  m_updateServer.setup(&m_server);
}

void HSDWebserver::begin()
{
  Serial.println(F(""));
  Serial.println(F("Starting WebServer."));

  m_server.begin();

  m_server.on("/", std::bind(&HSDWebserver::deliverStatusPage, this));
  m_server.on("/cfgmain", std::bind(&HSDWebserver::deliverRootPage, this));
  m_server.on("/cfgcolormapping", std::bind(&HSDWebserver::deliverColorMappingPage, this));
  m_server.on("/cfgdevicemapping", std::bind(&HSDWebserver::deliverDeviceMappingPage, this));
  m_server.onNotFound(std::bind(&HSDWebserver::deliverNotFoundPage, this));
}

void HSDWebserver::handleClient(unsigned long deviceUptime)
{
  m_deviceUptimeMinutes = deviceUptime;
  m_server.handleClient();
}

void HSDWebserver::deliverRootPage()
{
  bool needSave = updateMainConfig();
  
  String html;
  html.reserve(3000);
  
  html = m_html.getHeader("General configuration", m_config.getHost(), m_config.getVersion());

  html += F("<form><font face='Verdana,Arial,Helvetica'>");
  
  html += F(
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>General</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Name</td>");
  
  html += F("  <td><input type='text' id='host' name='host' value='");
  html += String(m_config.getHost());
  html += F("' size='30' maxlength='40' placeholder='host'></td></tr>");

  html += F(
  " <tr>"
  "  <td><b><font size='+1'>WiFi</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>SSID</td>");
  
  html += F("<td><input type='text' id='wifiSSID' name='wifiSSID' value='");
  html += String(m_config.getWifiSSID());
  html += F("' size='30' maxlength='40' placeholder='SSID'></td>");
  html += F("</tr><tr><td>Password</td>");
  html += F("  <td><input type='text' id='wifiPSK' name='wifiPSK' value='");
  html += String(m_config.getWifiPSK());
  html += F("' size='30' maxlength='40' placeholder='Password'></td></tr>");

  html += F(
  " <tr>"
  "  <td><b><font size='+1'>MQTT</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Server</td>");
  html += F("  <td><input type='text' id='mqttServer' name='mqttServer' value='");
  html += String(m_config.getMqttServer());
  html += F("' size='30' maxlength='40' placeholder='IP or hostname'></td></tr><tr><td>Status topic</td>");
  
  html += F("  <td><input type='text' id='mqttStatusTopic' name='mqttStatusTopic' value='");
  html += String(m_config.getMqttStatusTopic());
  html += F("' size='30' maxlength='40' placeholder='#'></td>"
  " </tr>"
  " <tr>"
  "  <td>Test topic</td>"
  "  <td><input type='text' id='mqttTestTopic' name='mqttTestTopic' value='");
  html += String(m_config.getMqttTestTopic());
  html += F("' size='30' maxlength='40' placeholder='#'></td>"
  " </tr>"
  " <tr>"
  "  <td>Will topic</td>"
  "  <td><input type='text' id='mqttWillTopic' name='mqttWillTopic' value='");
  html += String(m_config.getMqttWillTopic());
  html += F("' size='30' maxlength='40' placeholder='#'></td></tr>");

  html += F(""
  " <tr>"
  "  <td><b><font size='+1'>LEDs</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Number of LEDs</td>");
  html += "  <td><input type='text' id='ledCount' name='ledCount' value='" + String(m_config.getNumberOfLeds()) + "' size='30' maxlength='40' placeholder='0'></td></tr>";
  html += F("<tr><td>LED pin</td>");
  html += F("<td><input type='text' id='ledPin' name='ledPin' value='");
  html += String(m_config.getLedDataPin());
  html += F("' size='30' maxlength='40' placeholder='0'></td></tr>");

  html += F("<tr><td>Brightness</td>");
  html += F("<td><input type='text' id='ledBrightness' name='ledBrightness' value='");
  html += String(m_config.getLedBrightness());
  html += F("' size='30' maxlength='5' placeholder='0-255'></td></tr></table>"); 
  
  html += F("<input type='submit' class='button' value='Save'>");

  html += F("</form></font></body></html>");
  
  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);

  if(needSave)
  {
    Serial.println(F("Main config has changed, storing it."));
    m_config.saveMain();
  }

  checkReboot();

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

void HSDWebserver::deliverStatusPage()
{
  String html;
  html.reserve(3000);
  
  html = m_html.getHeader("Status", m_config.getHost(), m_config.getVersion());

  html += F("<p>Device uptime: ");
  html += m_html.minutes2Uptime(m_deviceUptimeMinutes);
  html += F("</p>");

  if (WiFi.status() == WL_CONNECTED)
  {
    html += F("<p>Device is connected to WLAN <b>");
    html += WiFi.SSID();
    html += F("</b><br/>IP address is <b>");
    html += m_html.ip2String(WiFi.localIP());
    html += F("</b><br/><p>");
  }
  else
  {
    html += F("<p>Device is not connected to local network<p>");
  }

  if(m_mqtt.connected())
  {
    html += F("<p>Device is connected to  MQTT broker <b>");
    html += m_config.getMqttServer();
    html += F("</b><p>");
  }
  else
  {
    html += F("<p>Device is not connected to an MQTT broker<p>");
  }

  if(m_config.getNumberOfLeds() == 0)
  {
    html += F("<p>No LEDs configured yet<p>");
  }
  else
  {
    int ledOnCount = 0;
    
    html += F("<p>");
    
    for(int ledNr = 0; ledNr < m_config.getNumberOfLeds(); ledNr++)
    {
      HSDConfig::Color color = m_leds.getColor(ledNr);
      HSDConfig::Behavior behavior = m_leds.getBehavior(ledNr);

      if( (HSDConfig::NONE != color) && (HSDConfig::OFF != behavior) )
      {
        html += F("<p><div style='background-color:");
        html += m_html.color2htmlColor(color);
        html += F(";width:15px;height:15px;border:1px black solid;float:left;margin-right:5px'></div>"); 
        html += F("LED number <b>");
        html += ledNr;
        html += F("</b> is <b>");
        html += m_html.behavior2String(behavior);
        html += F("</b> with color <b>");
        html += m_html.color2String(color);      
        html += F("</b><br/></p>");

        ledOnCount++;
      }
    }

    if(ledOnCount == 0)
    {
      html += F("<p>All LEDs are <b>off</b><p>");
    }

    html += F("</p>");
  }

  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);

  checkReboot();
}

void HSDWebserver::deliverColorMappingPage()
{
  bool dirty = false;
  
  if(needAdd())
  {
    Serial.println(F("Need to add color mapping config entry"));
    addColorMappingEntry();
    dirty = true;
  }
  else if(needDelete())
  {
    Serial.println(F("Need to delete color mapping config entry"));
    deleteColorMappingEntry();
    dirty = true;
  }

  //if(dirty)
  {
    Serial.println(F("Need to update color mapping config"));
    m_config.updateColorMapping();
  }
  
  String html;
  html.reserve(10000);
  
  html = m_html.getHeader("Color mapping configuration", m_config.getHost(), m_config.getVersion());

  html += m_html.getColorMappingTableHeader();

  for(uint32_t i = 0; i < m_config.getNumberOfColorMappingEntries(); i++)
  {
    const HSDConfig::colorMapping* mapping = m_config.getColorMapping(i);

    if(mapping)
    {
      html += m_html.getColorMappingTableEntry(i+1, mapping);
    }
  }

  html += m_html.getColorMappingTableFooter();

  html += F("</table><br/>Add new entry:<br/>");
  html += m_html.getColorMappingTableAddEntryForm(m_config.getNumberOfColorMappingEntries());

  html += F("<br/>Delete Entry:<br/>");
  html += m_html.getDeleteEntryForm();
  
  html += m_html.getFooter();

  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);

  checkReboot();

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

bool HSDWebserver::needAdd()
{
   return (m_server.hasArg("add"));
}

bool HSDWebserver::needDelete()
{
   return (m_server.hasArg("delete"));
}

bool HSDWebserver::addColorMappingEntry()
{
  bool success = false;
  
  if(m_server.hasArg("n") && m_server.hasArg("t") && m_server.hasArg("c") && m_server.hasArg("b"))
  {
    if(m_server.arg("n") != "")
    {
      success = m_config.addColorMappingEntry(m_server.arg("n"), 
                                              (HSDConfig::deviceType)(m_server.arg("t").toInt()), 
                                              (HSDConfig::Color)(m_server.arg("c").toInt()), 
                                              (HSDConfig::Behavior)(m_server.arg("b").toInt()));

      if(success)
      {
        m_config.saveColorMapping();                                        
      } 
    }
    else
    {
      Serial.print(F("Skipping empty entry"));
    }
  }

  return success;
}

bool HSDWebserver::deleteColorMappingEntry()
{
  bool success = false;
  int entryNum = 0;
  
  if(m_server.hasArg("i"))
  {
    entryNum = m_server.arg("i").toInt();
    if(entryNum != 0)
    {
      success = m_config.deleteColorMappingEntry(entryNum-1);

      if(success)
      {
        m_config.saveColorMapping();                                        
      }                                      
    }
  }

  return success;
}

void HSDWebserver::deliverDeviceMappingPage()
{
  bool dirty = false;
  
  if(needAdd())
  {
    Serial.println(F("Need to add device mapping config entry"));
    addDeviceMappingEntry();
    dirty = true;
  }
  else if(needDelete())
  {
    Serial.println(F("Need to delete device mapping config entry"));
    deleteDeviceMappingEntry();
    dirty = true;
  }

  //if(dirty)
  {
    Serial.println(F("Need to update device mapping config"));
    m_config.updateDeviceMapping();
  }

  String html;
  html.reserve(10000);
    
  html = m_html.getHeader("Device mapping configuration", m_config.getHost(), m_config.getVersion());

  html += m_html.getDeviceMappingTableHeader();
  
  for(uint32_t i = 0; i < m_config.getNumberOfDeviceMappingEntries(); i++)
  {
    const HSDConfig::deviceMapping* mapping = m_config.getDeviceMapping(i);

    if(mapping)
    {
      html += m_html.getDeviceMappingTableEntry(i+1, mapping);
    }
  }

  html += m_html.getDeviceMappingTableFooter();

  html += F("</table><br/>Add new entry:<br/>");
  html += m_html.getDeviceMappingTableAddEntryForm(m_config.getNumberOfDeviceMappingEntries());

  html += F("<br/>Delete Entry:<br/>");
  html += m_html.getDeleteEntryForm();
  
  html += m_html.getFooter();

  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);
  
  checkReboot();

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

bool HSDWebserver::addDeviceMappingEntry()
{
  bool success = false;

  if(m_server.hasArg("n") && m_server.hasArg("t") && m_server.hasArg("l"))
  {
    if(m_server.arg("n") != "")
    {
      success = m_config.addDeviceMappingEntry(m_server.arg("n"), 
                                               (HSDConfig::deviceType)(m_server.arg("t").toInt()), 
                                               m_server.arg("l").toInt());

      if(success)
      {
        m_config.saveDeviceMapping();                                        
      }                                    
    }
    else
    {
      Serial.print(F("Skipping empty entry"));
    }
  }
  
  return success;
}

bool HSDWebserver::deleteDeviceMappingEntry()
{
  bool success = false;
  int entryNum = 0;
  
  if(m_server.hasArg("i"))
  {
    entryNum = m_server.arg("i").toInt();
    if(entryNum != 0)
    {
      success = m_config.deleteDeviceMappingEntry(entryNum-1);

      if(success)
      {
        m_config.saveDeviceMapping();                                        
      }                                      
    }
  }

  return success;
}

void HSDWebserver::deliverNotFoundPage()
{
  String html = F("File Not Found\n\n");
  html += F("URI: ");
  html += m_server.uri();
  html += F("\nMethod: ");
  html += (m_server.method() == HTTP_GET) ? F("GET") : F("POST");
  html += F("\nArguments: ");
  html += m_server.args();
  html += F("\n");
  
  for (uint8_t i = 0; i < m_server.args(); i++)
  {
    html += " " + m_server.argName(i) + ": " + m_server.arg(i) + "\n";
  }
  
  m_server.send(404, F("text/plain"), html);
}

void HSDWebserver::checkReboot()
{
  if(m_server.hasArg(F("reset"))) 
  {
    Serial.println(F("Rebooting ESP."));
    ESP.restart();
  }
}

bool HSDWebserver::updateMainConfig()
{
  bool needSave = false;

  if (m_server.hasArg(JSON_KEY_HOST))
  {
    needSave |= m_config.setHost(m_server.arg(JSON_KEY_HOST).c_str());
  }
  
  if (m_server.hasArg(JSON_KEY_WIFI_SSID))
  {
    needSave |= m_config.setWifiSSID(m_server.arg(JSON_KEY_WIFI_SSID).c_str());
  }
  
  if (m_server.hasArg(JSON_KEY_WIFI_PSK)) 
  {
    needSave |= m_config.setWifiPSK(m_server.arg(JSON_KEY_WIFI_PSK).c_str());
  }

  if (m_server.hasArg(JSON_KEY_MQTT_SERVER))
  {
    needSave |= m_config.setMqttServer(m_server.arg(JSON_KEY_MQTT_SERVER).c_str());
  }
  
  if (m_server.hasArg(JSON_KEY_MQTT_STATUS_TOPIC))
  {
    needSave |= m_config.setMqttStatusTopic(m_server.arg(JSON_KEY_MQTT_STATUS_TOPIC).c_str());
  }
  
  if (m_server.hasArg(JSON_KEY_MQTT_TEST_TOPIC)) 
  {
    needSave |= m_config.setMqttTestTopic(m_server.arg(JSON_KEY_MQTT_TEST_TOPIC).c_str());
  }

  if (m_server.hasArg(JSON_KEY_MQTT_WILL_TOPIC)) 
  {
    needSave |= m_config.setMqttWillTopic(m_server.arg(JSON_KEY_MQTT_WILL_TOPIC).c_str());
  }

  if (m_server.hasArg(JSON_KEY_LED_COUNT))
  {
    int ledCount = m_server.arg(JSON_KEY_LED_COUNT).toInt();
    
    if(ledCount > 0)
    {
      needSave |= m_config.setNumberOfLeds(ledCount);
    }
  }
  
  if (m_server.hasArg(JSON_KEY_LED_PIN)) 
  {
    int ledPin = m_server.arg(JSON_KEY_LED_PIN).toInt();
    
    if(ledPin > 0)
    {
      needSave |= m_config.setLedDataPin(ledPin);
    }
  }

  if (m_server.hasArg(JSON_KEY_LED_BRIGHTNESS)) 
  {
    uint8_t ledBrightness = m_server.arg(JSON_KEY_LED_BRIGHTNESS).toInt();
    
    if(ledBrightness > 0)
    {
      needSave |= m_config.setLedBrightness(ledBrightness);
    }
  }

  return needSave;
}

