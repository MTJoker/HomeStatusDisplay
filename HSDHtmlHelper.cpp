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
  header.reserve(1000);

  header  = F("<!doctype html> <html>");
  header += F("<head><meta charset='utf-8'>");
  header += F("<title>");
  header += String(host);
  header += F("</title>");
  header += F("<style>.button {border-radius:0;height:30px;width:100px;border:0;background-color:black;color:#fff;margin:5px;cursor:pointer;}</style>");
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
  
  return header;  
}

String HSDHtmlHelper::getSaveButton() const
{
  return F("<input type='submit' class='button' value='Save'>");
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

String HSDHtmlHelper::getDeviceMappingEntry(int entryNum, const HSDConfig::deviceMapping* mapping) const
{
  String name = "n" + String(entryNum);
  String type = "t" + String(entryNum);
  String led  = "l" + String(entryNum);

  const HSDConfig::deviceMapping* mappingInternal = mapping;
  HSDConfig::deviceMapping mappingDefault = {"", HSDConfig::TYPE_WINDOW, entryNum};

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
  html += F("' size='30' maxlength='25' placeholder='name'></td>");
  html += F("<td><select name='");
  html += type;
  html += F("'>");
  html += getTypeOptions(mappingInternal->type);
  html += F("</select></td>");
  html += F("<td><input type='text' id='led' name='");
  html += led;
  html += F("' value='");
  html += mappingInternal->ledNumber;
  html += F("' size='5' maxlength='3' placeholder='nr'></td></tr>");

  return html;
}

String HSDHtmlHelper::getColorMappingEntry(int entryNum, const HSDConfig::colorMapping* mapping) const
{
  String name     = "n" + String(entryNum);
  String type     = "t" + String(entryNum);
  String color    = "c" + String(entryNum);
  String behavior = "b" + String(entryNum);

  const HSDConfig::colorMapping* mappingInternal = mapping;
  HSDConfig::colorMapping mappingDefault = {"", HSDConfig::TYPE_WINDOW, HSDConfig::WHITE, HSDConfig::ON};

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
  html += F("' size='20' maxlength='15' placeholder='name'></td>");
  html += F("<td><select name='");
  html += type;
  html += F("'>");
  html += getTypeOptions(mappingInternal->type);
  html += F("</select></td>");
  html += F("<td><select name='");
  html += color;
  html += F("'>");
  html += getColorOptions(mappingInternal->color);
  html += F("</select></td>");
  html += F("<td><select name='");
  html += behavior;
  html += F("'>");
  html += getBehaviorOptions(mappingInternal->behavior);
  html += F("</select></td></tr>");

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

