#include "HSDHtmlHelper.hpp"

#define SELECTED_STRING (F("selected='selected'"))
#define CHECKED_STRING  (F("checked='checked'")) 
#define EMPTY_STRING    (F(""))

HSDHtmlHelper::HSDHtmlHelper()
{
  
}

String HSDHtmlHelper::getHeader(const char* title, const char* host, const char* version) const
{
  String header;
  header.reserve(1500);

  header  = F("<!doctype html> <html>");
  header += F("<head><meta charset='utf-8'>");
  header += F("<title>");
  header += String(host);
  header += F("</title>");
  header += F("<style>.button {border-radius:0;height:30px;width:100px;border:0;background-color:black;color:#fff;margin:5px;cursor:pointer;}</style>");
  header += F("<style>.hsdcolor {width:15px;height:15px;border:1px black solid;float:left;margin-right:5px';}</style>");
  header += F("<style>.rdark {background-color:#f9f9f9;}</style>");
  header += F("<style>.rlight {background-color:#e5e5e5;}</style>");
  header += F("</head>");
  header += F("<body bgcolor='#e5e5e5'><font face='Verdana,Arial,Helvetica'>");
  header += F("<font size='+3'>");
  header += String(host);
  header += F("</font><font size='-3'>V");
  header += String(version);
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

  Serial.print(F("Header size: "));
  Serial.println(header.length());
  
  return header;  
}

String HSDHtmlHelper::getFooter() const
{
  return F("</font></body></html>");
}

String HSDHtmlHelper::getColorMappingTableHeader() const
{
  return F(""
  "<table width='40%' border='1' cellpadding='1' cellspacing='2'>"
  " <tr style='background-color:#828282'>"
  "  <td><b><font size='+1'>Nr</font></b></td>"
  "  <td><b><font size='+1'>Message</font></b></td>"
  "  <td><b><font size='+1'>Type</font></b></td>"
  "  <td><b><font size='+1'>Color</font></b></td>"
  "  <td><b><font size='+1'>Behavior</font></b></td>"
  " </tr>");
}

String HSDHtmlHelper::getColorMappingTableEntry(int entryNum, const HSDConfig::colorMapping* mapping) const
{
  String html;
  if(entryNum % 2 == 0)
  {
    html += F("<tr class='rlight'><td>");
  }
  else
  {
    html += F("<tr class='rdark'><td>");
  }
  html += entryNum;
  html += F("</td><td>");
  html += mapping->msg;
  html += F("</td><td>");
  html += type2String(mapping->type);
  html += F("</td><td>");
  html += F("<div class='hsdcolor' style='background-color:");
  html += color2htmlColor(mapping->color);
  html += F("';></div></td><td>"); 
  html += behavior2String(mapping->behavior);
  html += F("</td></tr>");
  
  return html;
}

String HSDHtmlHelper::getColorMappingTableFooter() const
{
  return F("</table>");
}

String HSDHtmlHelper::getColorMappingTableAddEntryForm(int newEntryNum) const
{
  String html;
  
  html += F("<form><table><tr>");
  html += F("<td><input type='text' id='number' name='i' value='");
  html += newEntryNum;
  html += F("' size='5' maxlength='3' placeholder='Nr'</td>");
  html += F("<td><input type='text' id='name' name='n' value='' size='20' maxlength='15' placeholder='name'></td>");
  html += F("<td><select name='t'>");
  html += getTypeOptions(HSDConfig::TYPE_WINDOW);
  html += F("</select></td>");
  html += F("<td><select name='c'>");
  html += getColorOptions(HSDConfig::WHITE);
  html += F("</select></td>");
  html += F("<td><select name='b'>");
  html += getBehaviorOptions(HSDConfig::ON);
  html += F("</select></td></tr></table>");  
  html += F("<input type='submit' class='button' value='Add/Edit' id='add' name='add'></form>");

  return html;
}

String HSDHtmlHelper::getDeviceMappingTableHeader() const
{
  return F(""
  "<table width='30%' border='1' cellpadding='1' cellspacing='2'>"
  " <tr style='background-color:#828282'>"
  "  <td><b><font size='+1'>Nr</font></b></td>"  
  "  <td><b><font size='+1'>Device</font></b></td>"
  "  <td><b><font size='+1'>Type</font></b></td>"
  "  <td><b><font size='+1'>Led</font></b></td>"
  " </tr>");
}

String HSDHtmlHelper::getDeviceMappingTableEntry(int entryNum, const HSDConfig::deviceMapping* mapping) const
{
  String html;
    
  if(entryNum % 2 == 0)
  {
    html += F("<tr class='rlight'><td>");
  }
  else
  {
    html += F("<tr class='rdark'><td>");
  }
  html += entryNum;
  html += F("</td><td>");
  html += mapping->name;
  html += F("</td><td>");
  html += type2String(mapping->type);
  html += F("</td><td>");
  html += mapping->ledNumber;
  html += F("</td></tr>");

  return html;
}

String HSDHtmlHelper::getDeviceMappingTableFooter() const
{
  return F("</table>");
}

String HSDHtmlHelper::getDeviceMappingTableAddEntryForm(int entryNum) const
{
  String html;
  
  html += F("<form><table><tr>");
  html += F("<td><input type='text' id='name' name='n' value='' size='30' maxlength='25' placeholder='name'></td>");
  html += F("<td><select name='t'>");
  html += getTypeOptions(HSDConfig::TYPE_WINDOW);
  html += F("</select></td>");
  html += F("<td><input type='text' id='led' name='l' value='");
  html += entryNum;
  html += F("' size='6' maxlength='3' placeholder='led nr'></td></tr></table>");
  html += F("<input type='submit' class='button' value='Add' id='add' name='add'></form>");

  return html;
}

String HSDHtmlHelper::getDeleteEntryForm() const
{
  String html;
  
  html += F("<form><input type='text' id='number' name='i' value='' size='5' maxlength='3' placeholder='Nr'><br/>");
  html += F("<input type='submit' class='button' value='Delete' id='delete' name='delete'></form>");

  return html;  
}

String HSDHtmlHelper::getColorOptions(HSDConfig::Color selectedColor) const
{
  String greenSelect  = (selectedColor == HSDConfig::GREEN)  ? SELECTED_STRING : EMPTY_STRING;
  String yellowSelect = (selectedColor == HSDConfig::YELLOW) ? SELECTED_STRING : EMPTY_STRING;
  String orangeSelect = (selectedColor == HSDConfig::ORANGE) ? SELECTED_STRING : EMPTY_STRING;
  String redSelect    = (selectedColor == HSDConfig::RED)    ? SELECTED_STRING : EMPTY_STRING;
  String purpleSelect = (selectedColor == HSDConfig::PURPLE) ? SELECTED_STRING : EMPTY_STRING;
  String blueSelect   = (selectedColor == HSDConfig::BLUE)   ? SELECTED_STRING : EMPTY_STRING;
  String whiteSelect  = (selectedColor == HSDConfig::WHITE)  ? SELECTED_STRING : EMPTY_STRING;

  String html;

  html += F("<option "); html += greenSelect;  html += F(" value='"); html += HSDConfig::GREEN;  html += F("'>Green</option>");
  html += F("<option "); html += yellowSelect; html += F(" value='"); html += HSDConfig::YELLOW; html += F("'>Yellow</option>");
  html += F("<option "); html += orangeSelect; html += F(" value='"); html += HSDConfig::ORANGE; html += F("'>Orange</option>");
  html += F("<option "); html += redSelect;    html += F(" value='"); html += HSDConfig::RED;    html += F("'>Red</option>");
  html += F("<option "); html += purpleSelect; html += F(" value='"); html += HSDConfig::PURPLE; html += F("'>Purple</option>");
  html += F("<option "); html += blueSelect;   html += F(" value='"); html += HSDConfig::BLUE;   html += F("'>Blue</option>");
  html += F("<option "); html += whiteSelect;  html += F(" value='"); html += HSDConfig::WHITE;  html += F("'>White</option>");

  return html;
}

String HSDHtmlHelper::getBehaviorOptions(HSDConfig::Behavior selectedBehavior) const
{
  String onSelect       =   (selectedBehavior == HSDConfig::ON)         ? SELECTED_STRING : EMPTY_STRING;
  String blinkingSelect =   (selectedBehavior == HSDConfig::BLINKING)   ? SELECTED_STRING : EMPTY_STRING;
  String flashingSelect =   (selectedBehavior == HSDConfig::FLASHING)   ? SELECTED_STRING : EMPTY_STRING;
  String flickeringSelect = (selectedBehavior == HSDConfig::FLICKERING) ? SELECTED_STRING : EMPTY_STRING;
  
  String html;

  html += F("<option "); html += onSelect;         html += F(" value='"); html += HSDConfig::ON;         html += F("'>On</option>");
  html += F("<option "); html += blinkingSelect;   html += F(" value='"); html += HSDConfig::BLINKING;   html += F("'>Blinking</option>");
  html += F("<option "); html += flashingSelect;   html += F(" value='"); html += HSDConfig::FLASHING;   html += F("'>Flashing</option>");
  html += F("<option "); html += flickeringSelect; html += F(" value='"); html += HSDConfig::FLICKERING; html += F("'>Flickering</option>");
  
  return html;
}

String HSDHtmlHelper::getTypeOptions(HSDConfig::deviceType selectedType) const
{
  String windowSelect = (selectedType == HSDConfig::TYPE_WINDOW) ? SELECTED_STRING : EMPTY_STRING;
  String doorSelect   = (selectedType == HSDConfig::TYPE_DOOR)   ? SELECTED_STRING : EMPTY_STRING;
  String lightSelect  = (selectedType == HSDConfig::TYPE_LIGHT)  ? SELECTED_STRING : EMPTY_STRING;
  String alarmSelect  = (selectedType == HSDConfig::TYPE_ALARM)  ? SELECTED_STRING : EMPTY_STRING;

  String html;
  
  html += F("<option "); html += windowSelect; html += F("value='"); html += HSDConfig::TYPE_WINDOW; html += F("'>Window</option>");
  html += F("<option "); html += doorSelect;   html += F("value='"); html += HSDConfig::TYPE_DOOR;   html += F("'>Door</option>");
  html += F("<option "); html += lightSelect;  html += F("value='"); html += HSDConfig::TYPE_LIGHT;  html += F("'>Light</option>");
  html += F("<option "); html += alarmSelect;  html += F("value='"); html += HSDConfig::TYPE_ALARM;  html += F("'>Alarm</option>");
  
  return html;
}

String HSDHtmlHelper::ip2String(IPAddress ip) const
{
  char buffer[20];
  memset(buffer, 0, sizeof(buffer));

  sprintf(buffer, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
  
  return String(buffer);
}

String HSDHtmlHelper::color2String(HSDConfig::Color color) const
{
  String colorString = F("none");

  switch(color)
  {
    case HSDConfig::GREEN:  colorString = F("green"); break;
    case HSDConfig::YELLOW: colorString = F("yellow"); break;
    case HSDConfig::ORANGE: colorString = F("orange"); break;
    case HSDConfig::RED:    colorString = F("red"); break;
    case HSDConfig::PURPLE: colorString = F("purple"); break;
    case HSDConfig::BLUE:   colorString = F("blue"); break;
    case HSDConfig::WHITE:  colorString = F("white"); break;
    default: break;
  }

  return colorString;
}

String HSDHtmlHelper::color2htmlColor(HSDConfig::Color color) const
{
  String htmlcolor = F("#000000");

  switch(color)
  {
    case HSDConfig::GREEN:  htmlcolor = F("#00FF00"); break;
    case HSDConfig::YELLOW: htmlcolor = F("#FFFF00"); break;
    case HSDConfig::ORANGE: htmlcolor = F("#FF5500"); break;
    case HSDConfig::RED:    htmlcolor = F("#FF0000"); break;
    case HSDConfig::PURPLE: htmlcolor = F("#FF00FF"); break;
    case HSDConfig::BLUE:   htmlcolor = F("#0000FF"); break;
    case HSDConfig::WHITE:  htmlcolor = F("#FFFFFF"); break;
    default: break;
  }

  return htmlcolor;
}

String HSDHtmlHelper::behavior2String(HSDConfig::Behavior behavior) const
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

String HSDHtmlHelper::type2String(HSDConfig::deviceType type) const
{
  String typeString = F("Window");

  switch(type)
  {
    case HSDConfig::TYPE_WINDOW: typeString = F("Window"); break;
    case HSDConfig::TYPE_DOOR:   typeString = F("Door"); break;
    case HSDConfig::TYPE_LIGHT:  typeString = F("Light"); break;
    case HSDConfig::TYPE_ALARM:  typeString = F("Alarm"); break;
    default: break;
  }

  return typeString; 
}


String HSDHtmlHelper::minutes2Uptime(unsigned long minutes) const
{
  char buffer[50];
  memset(buffer, 0, sizeof(buffer));
  
  unsigned long days  = minutes / 60 / 24;
  unsigned long hours = (minutes / 60) % 24;
  unsigned long mins  = minutes % 60;

  sprintf(buffer, "%lu days, %lu hours, %lu minutes", days, hours, mins);
  
  return String(buffer);
}

