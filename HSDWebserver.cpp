#include "HSDWebserver.h"

#define SELECTED_STRING (F("selected='selected'"))
#define CHECKED_STRING  (F("checked='checked'")) 
#define EMPTY_STRING    (F(""))

HSDWebserver::HSDWebserver(HSDConfig& config, const HSDLeds& leds, const HSDMqtt& mqtt)
:
m_server(80),
m_config(config),
m_leds(leds),
m_mqtt(mqtt)
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

void HSDWebserver::handleClient()
{
  m_server.handleClient();
}

String HSDWebserver::htmlHeader(const char* title)
{
  String header;
  header.reserve(1000);

  header  = F("<!doctype html> <html>");
  header += F("<head><meta charset='utf-8'>");
  header += F("<title>");
  header += String(m_config.getHost());
  header += F("</title>");
  header += F("<style>.button {border-radius:0;height:30px;width:100px;border:0;background-color:black;color:#fff;margin:5px;cursor:pointer;}</style>");
  header += F("</head>");
  header += F("<body bgcolor='#e5e5e5'><font face='Verdana,Arial,Helvetica'>");
  header += F("<font size='+3'>");
  header += String(m_config.getHost());
  header += F("</font><font size='-3'>V");
  header += String(m_config.getVersion());
  header += F("</font>");
  header += F("<form><p><input type='button' class='button' onclick=\"location.href='./'\"  value='Status'>");
  header += F("<input type='submit' class='button'value='Reboot' id='reset' name='reset'>");
  header += F("<input type='button' class='button'onclick=\"location.href='./update'\"  value='Update Firmware'></p>");
  
  header += F("<p><input type='button' class='button'onclick=\"location.href='./cfgmain'\"  value='General'>");
  header += F("<input type='button' class='button'onclick=\"location.href='./cfgcolormapping'\"  value='Color mapping'>");
  header += F("<input type='button' class='button'onclick=\"location.href='./cfgdevicemapping'\"  value='Device mapping'></p></form>");
  
  header += F("<h4>"); 
  header += title;
  header += F("</h4>");
  
  return header;
}

String HSDWebserver::htmlSaveButton()
{
  return F("<input type='submit' class='button' value='Save'>");
}

String HSDWebserver::htmlColorOptions(HSDConfig::Color selectedColor)
{
  String noneSelect   = (selectedColor == HSDConfig::NONE)   ? SELECTED_STRING : EMPTY_STRING;
  String redSelect    = (selectedColor == HSDConfig::RED)    ? SELECTED_STRING : EMPTY_STRING;
  String greenSelect  = (selectedColor == HSDConfig::GREEN)  ? SELECTED_STRING : EMPTY_STRING;
  String blueSelect   = (selectedColor == HSDConfig::BLUE)   ? SELECTED_STRING : EMPTY_STRING;
  String yellowSelect = (selectedColor == HSDConfig::YELLOW) ? SELECTED_STRING : EMPTY_STRING;
  String whiteSelect  = (selectedColor == HSDConfig::WHITE)  ? SELECTED_STRING : EMPTY_STRING;

  String html;
  
  html += F("<option "); html += noneSelect;   html += F(" value='"); html += HSDConfig::NONE;   html += F("'>None</option>");
  html += F("<option "); html += redSelect;    html += F(" value='"); html += HSDConfig::RED;    html += F("'>Red</option>");
  html += F("<option "); html += greenSelect;  html += F(" value='"); html += HSDConfig::GREEN;  html += F("'>Green</option>");
  html += F("<option "); html += blueSelect;   html += F(" value='"); html += HSDConfig::BLUE;   html += F("'>Blue</option>");
  html += F("<option "); html += yellowSelect; html += F(" value='"); html += HSDConfig::YELLOW; html += F("'>Yellow</option>");
  html += F("<option "); html += whiteSelect;  html += F(" value='"); html += HSDConfig::WHITE;  html += F("'>White</option>");

  return html;
}

String HSDWebserver::htmlTypeOptions(HSDConfig::deviceType selectedType)
{
  String windowSelect = (selectedType == HSDConfig::TYPE_WINDOW) ? SELECTED_STRING : EMPTY_STRING;
  String doorSelect   = (selectedType == HSDConfig::TYPE_DOOR)   ? SELECTED_STRING : EMPTY_STRING;
  String lightSelect  = (selectedType == HSDConfig::TYPE_LIGHT)  ? SELECTED_STRING : EMPTY_STRING;
  String alarmSelect  = (selectedType == HSDConfig::TYPE_ALARM)  ? SELECTED_STRING : EMPTY_STRING;

  String html;
  
  html += F("<option "); html += windowSelect; html += F(" value='"); html += HSDConfig::TYPE_WINDOW; html += F("'>Window</option>");
  html += F("<option "); html += doorSelect;   html += F("value='");  html += HSDConfig::TYPE_DOOR;   html += F("'>Door</option>");
  html += F("<option "); html += lightSelect;  html += F("value='");  html += HSDConfig::TYPE_LIGHT;  html += F("'>Light</option>");
  html += F("<option "); html += alarmSelect;  html += F("value='");  html += HSDConfig::TYPE_ALARM;  html += F("'>Alarm</option>");
  
  return html;
}

String HSDWebserver::htmlBehaviorOptions(HSDConfig::Behavior selectedBehavior)
{
  String onSelect       =   (selectedBehavior == HSDConfig::ON)         ? SELECTED_STRING : EMPTY_STRING;
  String offSelect      =   (selectedBehavior == HSDConfig::OFF)        ? SELECTED_STRING : EMPTY_STRING;
  String blinkingSelect =   (selectedBehavior == HSDConfig::BLINKING)   ? SELECTED_STRING : EMPTY_STRING;
  String flashingSelect =   (selectedBehavior == HSDConfig::FLASHING)   ? SELECTED_STRING : EMPTY_STRING;
  String flickeringSelect = (selectedBehavior == HSDConfig::FLICKERING) ? SELECTED_STRING : EMPTY_STRING;
  
  String html;

  html += F("<option "); html += onSelect;         html += F(" value='"); html += HSDConfig::ON;         html += F("'>On</option>");
  html += F("<option "); html += offSelect ;       html += F(" value='"); html += HSDConfig::OFF;        html += F("'>Off</option>");
  html += F("<option "); html += blinkingSelect;   html += F(" value='"); html += HSDConfig::BLINKING;   html += F("'>Blinking</option>");
  html += F("<option "); html += flashingSelect;   html += F(" value='"); html += HSDConfig::FLASHING;   html += F("'>Flashing</option>");
  html += F("<option "); html += flickeringSelect; html += F(" value='"); html += HSDConfig::FLICKERING; html += F("'>Flickering</option>");
  
  return html;
}

String HSDWebserver::htmlColorMappingEntry(int entryNum, const HSDConfig::colorMapping* mapping)
{
  String name     = "n" + String(entryNum);
  String type     = "t" + String(entryNum);
  String color    = "c" + String(entryNum);
  String behavior = "b" + String(entryNum);

  const HSDConfig::colorMapping* mappingInternal = mapping;
  HSDConfig::colorMapping mappingDefault = {"", HSDConfig::TYPE_WINDOW, HSDConfig::NONE, HSDConfig::OFF};

  if(!mapping)
  {
    mappingInternal = &mappingDefault;
  }
  
  String html;
  
  html += F("<tr>");
  html += F("<td><input type='text' id='name' name='");
  html += name;
  html += F("' value='");
  html += mappingInternal->msg;
  html += F("' size='20' maxlength='20' placeholder='name'></td>");
  html += F("<td><select name='");
  html += type;
  html += F("'>");
  html += htmlTypeOptions(mappingInternal->type);
  html += F("</select></td>");
  html += F("<td><select name='");
  html += color;
  html += F("'>");
  html += htmlColorOptions(mappingInternal->color);
  html += F("</select></td>");
  html += F("<td><select name='");
  html += behavior;
  html += F("'>");
  html += htmlBehaviorOptions(mappingInternal->behavior);
  html += F("</select></td></tr>");

  return html;
}

String HSDWebserver::htmlDeviceMappingEntry(int entryNum, const HSDConfig::deviceMapping* mapping)
{
  String name = "n" + String(entryNum);
  String type = "t" + String(entryNum);
  String led  = "l" + String(entryNum);

  const HSDConfig::deviceMapping* mappingInternal = mapping;
  HSDConfig::deviceMapping mappingDefault = {"", HSDConfig::TYPE_WINDOW, m_config.getNumberOfDeviceMappingEntries()};

  if(!mapping)
  {
    mappingInternal = &mappingDefault;
  }
  
  String html;
    
  html += F("<tr>");
  html += F("<td><input type='text' id='name' name='");
  html += name;
  html += F("' value='");
  html += mappingInternal->name;
  html += F("' size='30' maxlength='30' placeholder='name'></td>");
  html += F("<td><select name='");
  html += type;
  html += F("'>");
  html += htmlTypeOptions(mappingInternal->type);
  html += F("</select></td>");
  html += F("<td><input type='text' id='led' name='");
  html += led;
  html += F("' value='");
  html += mappingInternal->ledNumber;
  html += F("' size='5' maxlength='5' placeholder='nr'></td></tr>");

  return html;
}

void HSDWebserver::deliverRootPage()
{
  bool needSave = updateMainConfig();
  
  String html;
  html.reserve(3000);
  
  html = htmlHeader("General configuration");

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
  html += "  <td><input type='text' id='ledCount' name='ledCount' value='" + String(m_config.getNumberOfLeds()) + "' size='30' maxlength='40' placeholder='0'></td>";
  html += F(" </tr>"
  " <tr>"
  "  <td>LED pin</td>");
  html += F("  <td><input type='text' id='ledPin' name='ledPin' value='");
  html += String(m_config.getLedDataPin());
  html += F("' size='30' maxlength='40' placeholder='0'></td></tr></table>");

  html += htmlSaveButton();

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
  
  html = htmlHeader("Status");

  if (WiFi.status() == WL_CONNECTED)
  {
    html += F("<p>Device is connected to WLAN <b>");
    html += WiFi.SSID();
    html += F("</b><br/>IP address is <b>");
    html += ip2String(WiFi.localIP());
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
        html += color2htmlColor(color);
        html += F(";width:15px;height:15px;border:1px black solid;float:left;margin-right:5px'></div>"); 
        html += F("LED number <b>");
        html += ledNr;
        html += F("</b> is <b>");
        html += behavior2String(behavior);
        html += F("</b> with color <b>");
        html += color2String(color);      
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
  bool needSave = updateColorMappingConfig();
  
  String html;
  html.reserve(17000);
  
  html = htmlHeader("Color mapping configuration");

  html += F("<form><font face='Verdana,Arial,Helvetica'>");

  String checkBoxChecked = (m_config.isSwitchLedOffIfUnknownMessage()) ? CHECKED_STRING : EMPTY_STRING;

  html += F("<p><input type='checkbox' name='oiu' value='1' ");
  html += checkBoxChecked;
  html += F(">Behavior 'Off' if message is undefined</p>");

  html += F(""
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>Message</font></b></td>"
  "  <td><b><font size='+1'>Type</font></b></td>"
  "  <td><b><font size='+1'>Color</font></b></td>"
  "  <td><b><font size='+1'>Behavior</font></b></td>"
  " </tr>");

  for(uint32_t i = 0; i < m_config.getNumberOfColorMappingEntries(); i++)
  {
    const HSDConfig::colorMapping* mapping = m_config.getColorMapping(i);

    if(mapping)
    {
      html += htmlColorMappingEntry(i, mapping);
    }
  }
  
  // one additional for adding an entry
  html += htmlColorMappingEntry(m_config.getNumberOfColorMappingEntries(), NULL);
  html += F("</table>");
  
  html += htmlSaveButton();
  html += F("</form></font></body></html>");

  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);

  if(needSave)
  {
    Serial.println(F("Color mapping config has changed, storing it."));
    m_config.saveColorMapping();
  }

  checkReboot();

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

void HSDWebserver::deliverDeviceMappingPage()
{
  bool needSave = updateDeviceMappingConfig();
  
  String html = htmlHeader("Device mapping configuration");
  
  html += F("<form><font face='Verdana,Arial,Helvetica'>");

  html += F(""
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>Device</font></b></td>"
  "  <td><b><font size='+1'>Type</font></b></td>"
  "  <td><b><font size='+1'>Led</font></b></td>"
  " </tr>");
  
  for(uint32_t i = 0; i < m_config.getNumberOfDeviceMappingEntries(); i++)
  {
    const HSDConfig::deviceMapping* mapping = m_config.getDeviceMapping(i);

    if(mapping)
    {
      html += htmlDeviceMappingEntry(i, mapping);
    }
  }

  // one additional for adding an entry
  html += htmlDeviceMappingEntry(m_config.getNumberOfDeviceMappingEntries(), NULL);     
  html += F("</table>");
     
  html += htmlSaveButton();
  html += F("</form></font></body></html>");

  Serial.print(F("Page size: "));
  Serial.println(html.length());
  
  m_server.send(200, F("text/html"), html);

  if(needSave)
  {
    Serial.println(F("Device mapping config has changed, storing it."));
    m_config.saveDeviceMapping();
  }

  checkReboot();

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
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

String HSDWebserver::ip2String(IPAddress ip)
{
  char buffer[20];
  memset(buffer, 0, sizeof(buffer));

  sprintf(buffer, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  
  return String(buffer);
}

String HSDWebserver::color2String(HSDConfig::Color color)
{
  String colorString = F("none");

  switch(color)
  {
    case HSDConfig::RED:    colorString = F("red"); break;
    case HSDConfig::GREEN:  colorString = F("green"); break;
    case HSDConfig::BLUE:   colorString = F("blue"); break;
    case HSDConfig::YELLOW: colorString = F("yellow"); break;
    case HSDConfig::WHITE:  colorString = F("white"); break;
    default: break;
  }

  return colorString;
}

String HSDWebserver::color2htmlColor(HSDConfig::Color color)
{
  String htmlcolor = F("#000000");

  switch(color)
  {
    case HSDConfig::RED:    htmlcolor = F("#FF0000"); break;
    case HSDConfig::GREEN:  htmlcolor = F("#00FF00"); break;
    case HSDConfig::BLUE:   htmlcolor = F("#0000FF"); break;
    case HSDConfig::YELLOW: htmlcolor = F("#FFFF00"); break;
    case HSDConfig::WHITE:  htmlcolor = F("#FFFFFF"); break;
    default: break;
  }

  return htmlcolor;
}

String HSDWebserver::behavior2String(HSDConfig::Behavior behavior)
{
  String behaviorString = F("off");
  
  switch(behavior)
  {
    case HSDConfig::ON:         behaviorString = F("on"); break;
    case HSDConfig::BLINKING:   behaviorString = F("blinking"); break;
    case HSDConfig::FLASHING:   behaviorString = F("flashing"); break;
    case HSDConfig::FLICKERING: behaviorString = F("flickering"); break;
    default: break;
  }

  return behaviorString;
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

  return needSave;
}

bool HSDWebserver::updateColorMappingConfig()
{
  bool needSave = false;

  int numArgs = m_server.args();

  if(numArgs != 0)    // when page is initially loaded, do nothing because no args
  {
    if( (numArgs % 4 == 0) || ((numArgs % 4 == 1)  && m_server.hasArg(JSON_KEY_COLORMAPPING_OIU)) )   // one more if checkbox is checked
    {
      Serial.println(F("Number of arguments seems reasonable"));
      
      m_config.resetColorMappingConfigData();

      if(m_server.hasArg("oiu"))
      {
        m_config.setSwitchLedOffIfUnknownMessage(true);
      }
      else
      {
        m_config.setSwitchLedOffIfUnknownMessage(false);
      }
  
      for(int i = 0; i < (numArgs/4); i++)
      {
        String name     = "n" + String(i);
        String type     = "t" + String(i);
        String color    = "c" + String(i);
        String behavior = "b" + String(i);

        if(m_server.hasArg(name) && m_server.hasArg(type) && m_server.hasArg(color) && m_server.hasArg(behavior))
        {
          if(m_server.arg(name) != "")
          {
            m_config.addColorMappingEntry(m_server.arg(name), 
                                          (HSDConfig::deviceType)(m_server.arg(type).toInt()), 
                                          (HSDConfig::Color)(m_server.arg(color).toInt()), 
                                          (HSDConfig::Behavior)(m_server.arg(behavior).toInt()));
          }
          else
          {
            Serial.print(F("Skipping entry number ")); Serial.println(String(i));
          }
        }
      }
  
      needSave = true;
    }
    else
    {
      Serial.println(F("Number of Arguments seems wrong!"));
    }
  }

  return needSave;
}

bool HSDWebserver::updateDeviceMappingConfig()
{
  bool needSave = false;

  int numArgs = m_server.args();

  Serial.println("Got " + String(numArgs)  + " args");

  if(numArgs != 0)    // when page is initially loaded, do nothing because no args
  {
    if((numArgs % 3) == 0)
    {
      Serial.println(F("Number of arguments seems reasonable"));
  
      m_config.resetDeviceMappingConfigData();
  
      for(int i = 0; i < (numArgs/3); i++)
      {
        String name = "n" + String(i);
        String type = "t" + String(i);
        String led  = "l" + String(i);

        if(m_server.hasArg(name) && m_server.hasArg(type) && m_server.hasArg(led))
        {
          if(m_server.arg(name) != "")
          {
            m_config.addDeviceMappingEntry(m_server.arg(name), 
                                          (HSDConfig::deviceType)(m_server.arg(type).toInt()), 
                                          m_server.arg(led).toInt());
          }
          else
          {
            Serial.print(F("Skipping entry number ")); Serial.println(String(i));
          }
        }
      }
  
      needSave = true;
    }
    else
    {
      Serial.println(F("Number of Arguments seems wrong!"));
    }
  }

  return needSave;
}

