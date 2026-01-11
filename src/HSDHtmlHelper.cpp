#include "HSDHtmlHelper.hpp"
#include "HSDEnumsToString.hpp"

inline const __FlashStringHelper* selectedStr()
{
    return F("selected='selected'");
}
inline const __FlashStringHelper* emptyStr()
{
    return F("");
}

String HSDHtmlHelper::getHeader(const char* title, const char* host, const char* version, uint32_t reloadSeconds) const
{
    String header;
    header.reserve(1500);

    header = F("<!doctype html> <html>");
    header += F("<head><meta charset='utf-8'>");
    header += F("<title>");
    header += String(host);
    header += F("</title>");

    if(reloadSeconds > 0)
    {
        header += F("<script>");
        header += F("setInterval(function(){location.reload();},");
        header += String(reloadSeconds * 1000);
        header += F(");");
        header += F("</script>");
    }

    header += F("<style>.button {border-radius:0;height:30px;width:100px;border:0;background-color:black;color:#fff;margin:5px;cursor:pointer;}</style>");
    header += F("<style>.buttonr {border-radius:0;height:30px;width:100px;border:0;background-color:red;color:#fff;margin:5px;cursor:pointer;}</style>");
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
    header += F("<input type='button' class='button'onclick=\"location.href='./maintenance'\"  value='Maintenance'></p>");
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

String HSDHtmlHelper::getColorMappingTableEntry(int entryNum, const HSDConfig::ColorMapping& mapping) const
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
    html += mapping.msg.data();
    html += F("</td><td>");
    html += toString(mapping.type);
    html += F("</td><td>");
    html += F("<div class='hsdcolor' style='background-color:");
    html += color2htmlColor(mapping.color);
    html += F("';></div></td><td>");
    html += toString(mapping.behavior);
    html += F("</td></tr>");

    return html;
}

String HSDHtmlHelper::getColorMappingTableFooter() const
{
    return F("</table>");
}

String HSDHtmlHelper::getColorMappingTableAddEntryForm(int newEntryNum, bool isFull) const
{
    String html;

    html += F("<form><table><tr>");
    html += F("<td><input type='text' id='number' name='i' value='");
    html += isFull ? newEntryNum - 1 : newEntryNum;
    html += F("' size='5' maxlength='3' placeholder='Nr'</td>");
    html += F("<td><input type='text' id='name' name='n' value='' size='20' maxlength='15' placeholder='name'></td>");
    html += F("<td><select name='t'>");
    html += getTypeOptions(DeviceType::Window);
    html += F("</select></td>");
    html += F("<td><select name='c'>");
    html += getColorOptions(Color::White);
    html += F("</select></td>");
    html += F("<td><select name='b'>");
    html += getBehaviorOptions(Behavior::On);
    html += F("</select></td></tr></table>");
    html += F("<input type='submit' class='button' value='");
    html += isFull ? F("Edit") : F("Add/Edit");
    html += F("' id='add' name='add'></form>");

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

String HSDHtmlHelper::getDeviceMappingTableEntry(int entryNum, const HSDConfig::DeviceMapping& mapping) const
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
    html += mapping.name.data();
    html += F("</td><td>");
    html += toString(mapping.type);
    html += F("</td><td>");
    html += mapping.ledNumber;
    html += F("</td></tr>");

    return html;
}

String HSDHtmlHelper::getDeviceMappingTableFooter() const
{
    return F("</table>");
}

String HSDHtmlHelper::getDeviceMappingTableAddEntryForm(int newEntryNum, bool isFull) const
{
    String html;

    html += F("<form><table><tr>");
    html += F("<td><input type='text' id='number' name='i' value='");
    html += isFull ? newEntryNum - 1 : newEntryNum;
    html += F("' size='5' maxlength='3' placeholder='Nr'</td>");
    html += F("<td><input type='text' id='name' name='n' value='' size='30' maxlength='25' placeholder='name'></td>");
    html += F("<td><select name='t'>");
    html += getTypeOptions(DeviceType::Window);
    html += F("</select></td>");
    html += F("<td><input type='text' id='led' name='l' value='");
    html += isFull ? newEntryNum - 1 : newEntryNum;
    html += F("' size='6' maxlength='3' placeholder='led nr'></td></tr></table>");
    html += F("<input type='submit' class='button' value='");
    html += isFull ? F("Edit") : F("Add/Edit");
    html += F("' id='add' name='add'></form>");

    return html;
}

String HSDHtmlHelper::getDeleteForm() const
{
    String html;

    html += F("<form><input type='text' id='number' name='i' value='' size='5' maxlength='3' placeholder='Nr'><br/>");
    html += F("<input type='submit' class='button' value='Delete' id='delete' name='delete'>");
    html += F("<input type='submit' class='button' value='Delete all' id='deleteall' name='deleteall'></form>");

    return html;
}

String HSDHtmlHelper::getSaveForm() const
{
    String html;

    html += F("<form><input type='submit' class='buttonr' value='Save' id='save' name='save'>");
    html += F("<form><input type='submit' class='buttonr' value='Undo' id='undo' name='undo'></form>");

    return html;
}

String HSDHtmlHelper::getColorOptions(Color selectedColor) const
{
    String greenSelect = (selectedColor == Color::Green) ? selectedStr() : emptyStr();
    String yellowSelect = (selectedColor == Color::Yellow) ? selectedStr() : emptyStr();
    String orangeSelect = (selectedColor == Color::Orange) ? selectedStr() : emptyStr();
    String redSelect = (selectedColor == Color::Red) ? selectedStr() : emptyStr();
    String purpleSelect = (selectedColor == Color::Purple) ? selectedStr() : emptyStr();
    String blueSelect = (selectedColor == Color::Blue) ? selectedStr() : emptyStr();
    String whiteSelect = (selectedColor == Color::White) ? selectedStr() : emptyStr();

    String html;

    html += F("<option ");
    html += greenSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Green);
    html += F("'>Green</option>");
    html += F("<option ");
    html += yellowSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Yellow);
    html += F("'>Yellow</option>");
    html += F("<option ");
    html += orangeSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Orange);
    html += F("'>Orange</option>");
    html += F("<option ");
    html += redSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Red);
    html += F("'>Red</option>");
    html += F("<option ");
    html += purpleSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Purple);
    html += F("'>Purple</option>");
    html += F("<option ");
    html += blueSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::Blue);
    html += F("'>Blue</option>");
    html += F("<option ");
    html += whiteSelect;
    html += F(" value='");
    html += HSDConfig::color2id(Color::White);
    html += F("'>White</option>");

    return html;
}

String HSDHtmlHelper::getBehaviorOptions(Behavior selectedBehavior) const
{
    String onSelect = (selectedBehavior == Behavior::On) ? selectedStr() : emptyStr();
    String blinkingSelect = (selectedBehavior == Behavior::Blinking) ? selectedStr() : emptyStr();
    String flashingSelect = (selectedBehavior == Behavior::Flashing) ? selectedStr() : emptyStr();
    String flickeringSelect = (selectedBehavior == Behavior::Flickering) ? selectedStr() : emptyStr();

    String html;

    html += F("<option ");
    html += onSelect;
    html += F(" value='");
    html += static_cast<uint8_t>(Behavior::On);
    html += F("'>On</option>");
    html += F("<option ");
    html += blinkingSelect;
    html += F(" value='");
    html += static_cast<uint8_t>(Behavior::Blinking);
    html += F("'>Blink</option>");
    html += F("<option ");
    html += flashingSelect;
    html += F(" value='");
    html += static_cast<uint8_t>(Behavior::Flashing);
    html += F("'>Flash</option>");
    html += F("<option ");
    html += flickeringSelect;
    html += F(" value='");
    html += static_cast<uint8_t>(Behavior::Flickering);
    html += F("'>Flicker</option>");

    return html;
}

String HSDHtmlHelper::getTypeOptions(DeviceType selectedType) const
{
    String windowSelect = (selectedType == DeviceType::Window) ? selectedStr() : emptyStr();
    String doorSelect = (selectedType == DeviceType::Door) ? selectedStr() : emptyStr();
    String lightSelect = (selectedType == DeviceType::Light) ? selectedStr() : emptyStr();
    String alarmSelect = (selectedType == DeviceType::Alarm) ? selectedStr() : emptyStr();

    String html;

    html += F("<option ");
    html += windowSelect;
    html += F("value='");
    html += static_cast<uint8_t>(DeviceType::Window);
    html += F("'>Window</option>");
    html += F("<option ");
    html += doorSelect;
    html += F("value='");
    html += static_cast<uint8_t>(DeviceType::Door);
    html += F("'>Door</option>");
    html += F("<option ");
    html += lightSelect;
    html += F("value='");
    html += static_cast<uint8_t>(DeviceType::Light);
    html += F("'>Light</option>");
    html += F("<option ");
    html += alarmSelect;
    html += F("value='");
    html += static_cast<uint8_t>(DeviceType::Alarm);
    html += F("'>Alarm</option>");

    return html;
}

String HSDHtmlHelper::ip2String(IPAddress ip) const
{
    char buffer[20];
    memset(buffer, 0, sizeof(buffer));

    sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return String(buffer);
}

String HSDHtmlHelper::color2htmlColor(Color color) const
{
    String htmlcolor = F("#000000");

    switch(color)
    {
    case Color::Green:
        htmlcolor = F("#00FF00");
        break;
    case Color::Yellow:
        htmlcolor = F("#FFFF00");
        break;
    case Color::Orange:
        htmlcolor = F("#FF5500");
        break;
    case Color::Red:
        htmlcolor = F("#FF0000");
        break;
    case Color::Purple:
        htmlcolor = F("#FF00FF");
        break;
    case Color::Blue:
        htmlcolor = F("#0000FF");
        break;
    case Color::White:
        htmlcolor = F("#FFFFFF");
        break;
    default:
        break;
    }

    return htmlcolor;
}

String HSDHtmlHelper::minutes2Uptime(unsigned long minutes) const
{
    char buffer[50];
    memset(buffer, 0, sizeof(buffer));

    unsigned long days = minutes / 60 / 24;
    unsigned long hours = (minutes / 60) % 24;
    unsigned long mins = minutes % 60;

    sprintf(buffer, "%lu days, %lu hours, %lu minutes", days, hours, mins);

    return String(buffer);
}
