#include "HSDWebserver.hpp"
#include "HSDEnumsToString.hpp"
#include "LittleFS.h"

HSDWebserver::HSDWebserver(HSDConfig& config, const HSDLeds& leds,
                           const HSDMqtt& mqtt)
    : m_server(80)
    , m_config(config)
    , m_leds(leds)
    , m_mqtt(mqtt)
    , m_deviceUptimeMinutes(0)
    , m_html()
{
    m_updateServer.setup(&m_server);
}

void HSDWebserver::begin()
{
    Serial.println();
    Serial.println(F("Starting WebServer."));

    m_server.on("/", [this]()
                { deliverStatusPage(); });
    m_server.on("/maintenance", [this]()
                { deliverMaintenancePage(); });
    m_server.on("/cfgmain", [this]()
                { deliverMainConfigPage(); });
    m_server.on("/cfgcolormapping", [this]()
                { deliverColorMappingConfigPage(); });
    m_server.on("/cfgdevicemapping", [this]()
                { deliverDeviceMappingConfigPage(); });
    m_server.on("/backup", HTTP_GET,
                [this]()
                { handleBackup(); });
    m_server.on("/restore", HTTP_POST, [this]()
                { handleRestoreFinish(); }, [this]()
                { handleRestoreUpload(); });

    m_server.onNotFound([this]()
                        { deliverNotFoundPage(); });

    m_server.begin();
}

void HSDWebserver::handleClient(unsigned long deviceUptime)
{
    m_deviceUptimeMinutes = deviceUptime;
    m_server.handleClient();
    yield();
}

void HSDWebserver::deliverStatusPage()
{
    String html;
    html.reserve(3000);

    html =
        m_html.getHeader("Status", m_config.getHost(), m_config.getVersion(), 10);

    html += F("<p>Device uptime: ");
    html += m_html.minutes2Uptime(m_deviceUptimeMinutes);
    html += F("</p>");

    if(WiFi.status() == WL_CONNECTED)
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
            auto color = m_leds.getColor(ledNr);
            auto behavior = m_leds.getBehavior(ledNr);
            auto deviceInfo = m_config.getDeviceInfo(ledNr);

            if((Color::None != color) && (Behavior::Off != behavior))
            {
                html += F("<p><div class='hsdcolor' style='background-color:");
                html += m_html.color2htmlColor(color);
                html += F("';></div>");
                html += F("LED number <b>");
                html += ledNr;
                html += F("</b>");

                if(deviceInfo.has_value())
                {
                    auto deviceName = deviceInfo->first;
                    auto deviceType = deviceInfo->second;

                    html += F(" (<b>");
                    html += toString(deviceType);
                    html += F("</b> with name <b>");
                    html += deviceName.data();
                    html += F("</b>)");
                }

                html += F(" has behavior <b>");
                html += toString(behavior);
                html += F("</b> with color <b>");
                html += toString(color);
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

void HSDWebserver::deliverMainConfigPage()
{
    bool needSave = updateMainConfig();

    String html;
    html.reserve(3500);

    html = m_html.getHeader("General configuration", m_config.getHost(),
                            m_config.getVersion(), 0);

    html += F("<form><font face='Verdana,Arial,Helvetica'>");

    html += F("<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
              " <tr>"
              "  <td><b><font size='+1'>General</font></b></td>"
              "  <td></td>"
              " </tr>"
              " <tr>"
              "  <td>Name</td>");

    html += F("  <td><input type='text' id='host' name='host' value='");
    html += String(m_config.getHost());
    html += F("' size='30' maxlength='40' placeholder='host'></td></tr>");

    html += F(" <tr>"
              "  <td><b><font size='+1'>WiFi</font></b></td>"
              "  <td></td>"
              " </tr>"
              " <tr>"
              "  <td>SSID</td>");

    html += F("<td><input type='text' id='wifiSSID' name='wifiSSID' value='");
    html += String(m_config.getWifiSSID());
    html += F("' size='30' maxlength='40' placeholder='SSID'></td>");
    html += F("</tr><tr><td>Password</td>");
    html += F("  <td><input type='password' id='wifiPSK' name='wifiPSK' value='");
    html += String(m_config.getWifiPSK());
    html += F("' size='30' maxlength='40' placeholder='Password'></td></tr>");

    html += F(" <tr>"
              "  <td><b><font size='+1'>MQTT</font></b></td>"
              "  <td></td>"
              " </tr>");

    html += F("<tr><td>Server</td>");
    html += F("  <td><input type='text' id='mqttServer' name='mqttServer' value='");
    html += String(m_config.getMqttServer());
    html += F("' size='30' maxlength='40' placeholder='IP or hostname'></td></tr>");

    html += F("<tr><td>Username</td>");
    html += F("  <td><input type='text' id='mqttUser' name='mqttUser' value='");
    html += String(m_config.getMqttUser());
    html += F("' size='30' maxlength='40' placeholder='optional'></td></tr>");

    html += F("<tr><td>Password</td>");
    html += F("  <td><input type='password' id='mqttPassword' name='mqttPassword' value='");
    html += String(m_config.getMqttPassword());
    html += F("' size='30' maxlength='40' placeholder='optional'></td></tr>");

    html += F("<tr><td>Status topic</td>");
    html += F("  <td><input type='text' id='mqttStatusTopic' name='mqttStatusTopic' value='");
    html += String(m_config.getMqttStatusTopic());
    html += F("' size='30' maxlength='40' placeholder='#'></td></tr>");

    html += F("<tr><td>Test topic</td>");
    html += F("  <td><input type='text' id='mqttTestTopic' name='mqttTestTopic' value='");
    html += String(m_config.getMqttTestTopic());
    html += F("' size='30' maxlength='40' placeholder='#'></td></tr>");

    html += F("</tr> <td>Will topic</td>");
    html += F("  <td><input type='text' id='mqttWillTopic' name='mqttWillTopic' value='");
    html += String(m_config.getMqttWillTopic());
    html += F("' size='30' maxlength='40' placeholder='#'></td></tr>");

    html += F(""
              " <tr>"
              "  <td><b><font size='+1'>LEDs</font></b></td>"
              "  <td></td>"
              " </tr>"
              " <tr>"
              "  <td>Number of LEDs</td>");
    html += "  <td><input type='text' id='ledCount' name='ledCount' value='" +
            String(m_config.getNumberOfLeds()) +
            "' size='30' maxlength='40' placeholder='0'></td></tr>";

    html += F("<tr><td>LED type</td>");
    html += F("<td><select id='ledType' name='ledType'>");

    if(m_config.getLedType() == 0)
    {
        html += F("<option value='0' selected>GRB</option>");
        html += F("<option value='1'>GRBW</option>");
    }
    else
    {
        html += F("<option value='0'>GRB</option>");
        html += F("<option value='1' selected>GRBW</option>");
    }

    html += F("</select></td></tr>");

    html += F("<tr><td>LED pin</td>");
    html += F("<td><input type='text' id='ledPin' name='ledPin' value='");
    html += String(m_config.getLedDataPin());
    html += F("' size='30' maxlength='40' placeholder='0'></td></tr>");

    html += F("<tr><td>Brightness</td>");
    html += F(
        "<td><input type='text' id='ledBrightness' name='ledBrightness' value='");
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

    Serial.print(F("Free RAM: "));
    Serial.println(ESP.getFreeHeap());
}

void HSDWebserver::deliverColorMappingConfigPage()
{
    if(needUndo())
    {
        Serial.println(F("Need to undo changes to color mapping config"));
        m_config.updateColorMapping();
    }
    else if(needAdd())
    {
        Serial.println(F("Need to add color mapping config entry"));
        addColorMappingEntry();
    }
    else if(needDelete())
    {
        Serial.println(F("Need to delete color mapping config entry"));
        deleteColorMappingEntry();
    }
    else if(needDeleteAll())
    {
        Serial.println(F("Need to delete all color mapping config entries"));
        m_config.deleteAllColorMappingEntries();
    }
    else if(needSave())
    {
        Serial.println(F("Need to save color mapping config"));
        m_config.saveColorMapping();
    }

    String html;
    html.reserve(8000);

    html = m_html.getHeader("Color mapping configuration", m_config.getHost(),
                            m_config.getVersion(), 0);

    html += m_html.getColorMappingTableHeader();

    for(int i = 0; i < m_config.getNumberOfColorMappingEntries(); i++)
    {
        const auto mapping = m_config.getColorMapping(i);
        html += m_html.getColorMappingTableEntry(i, mapping);
    }

    html += m_html.getColorMappingTableFooter();

    if(m_config.isColorMappingFull())
    {
        html +=
            F("</table><p>Edit entry (add not possible, entry limit reached):</p>");
        html += m_html.getColorMappingTableAddEntryForm(
            m_config.getNumberOfColorMappingEntries(), true);
    }
    else
    {
        html += F("</table><p>Add/edit entry:</p>");
        html += m_html.getColorMappingTableAddEntryForm(
            m_config.getNumberOfColorMappingEntries(), false);
    }

    html += F("<p>Delete Entry:</p>");
    html += m_html.getDeleteForm();

    if(m_config.isColorMappingDirty())
    {
        html += F("<p style='color:red'>Unsaved changes! Press Save to make them "
                  "permanent, <br/>or Undo to revert to last saved version!</p>");
        html += m_html.getSaveForm();
    }

    html += m_html.getFooter();

    Serial.print(F("Page size: "));
    Serial.println(html.length());

    m_server.send(200, F("text/html"), html);

    checkReboot();

    Serial.print(F("Free RAM: "));
    Serial.println(ESP.getFreeHeap());
}

bool HSDWebserver::deliverMaintenancePage()
{
    String html;
    html.reserve(2000);

    html = m_html.getHeader("Maintenance", m_config.getHost(), m_config.getVersion(), 0);

    html += F("<p>Last reset reason: <b>");
    html += ESP.getResetReason();
    html += F("</b><p>");

    auto freeHeap = ESP.getFreeHeap();

    html += F("<p>Free Heap: <b>");
    html += freeHeap;
    html += F(" Bytes</b></p>");

    html += F("<form><p><input type='submit' class='button'value='Reboot' id='reset' name='reset'> Reboot device</p>");
    html += F("<input type='button' class='button'onclick=\"location.href='./update'\"  value='Update Firmware'> Update Firmware of device</p>");
    html += F("<input type='button' class='button' onclick=\"location.href='./backup'\" value='Config Backup'> Create Backup of config and download</p></form>");
    html += F("<form action='./restore' method='post' enctype='multipart/form-data'><p>");
    html += F("<input type='submit' class='button' value='Config Restore'>");
    html += F("<input type='file' name='backupfile' style='vertical-align:middle;'> Select downloaded config backup and restore</p></form>");

    html += m_html.getFooter();

    Serial.print(F("Page size: "));
    Serial.println(html.length());

    m_server.send(200, F("text/html"), html);

    checkReboot();

    Serial.print(F("Free Heap: "));
    Serial.println(freeHeap);

    return true;
}

bool HSDWebserver::needAdd()
{
    return (m_server.hasArg("add"));
}

bool HSDWebserver::needDelete()
{
    return (m_server.hasArg("delete"));
}

bool HSDWebserver::needDeleteAll()
{
    return (m_server.hasArg("deleteall"));
}

bool HSDWebserver::needSave()
{
    return (m_server.hasArg("save"));
}

bool HSDWebserver::needUndo()
{
    return (m_server.hasArg("undo"));
}

bool HSDWebserver::addColorMappingEntry()
{
    bool success = false;

    if(m_server.hasArg("i") && m_server.hasArg("n") && m_server.hasArg("t") &&
       m_server.hasArg("c") && m_server.hasArg("b"))
    {
        if(!m_server.arg("n").isEmpty())
        {
            success = m_config.addColorMappingEntry(
                m_server.arg("i").toInt(),
                m_server.arg("n").c_str(),
                static_cast<DeviceType>(m_server.arg("t").toInt()),
                static_cast<Color>(HSDConfig::id2color(m_server.arg("c").toInt())),
                static_cast<Behavior>(m_server.arg("b").toInt()));
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
        // TODO check conversion status
        success = m_config.deleteColorMappingEntry(entryNum);
    }

    return success;
}

void HSDWebserver::deliverDeviceMappingConfigPage()
{
    if(needUndo())
    {
        Serial.println(F("Need to undo changes to device mapping config"));
        m_config.updateDeviceMapping();
    }
    else if(needAdd())
    {
        Serial.println(F("Need to add device mapping config entry"));
        addDeviceMappingEntry();
    }
    else if(needDelete())
    {
        Serial.println(F("Need to delete device mapping config entry"));
        deleteDeviceMappingEntry();
    }
    else if(needDeleteAll())
    {
        Serial.println(F("Need to delete all device mapping config entries"));
        m_config.deleteAllDeviceMappingEntries();
    }
    else if(needSave())
    {
        Serial.println(F("Need to save device mapping config"));
        m_config.saveDeviceMapping();
    }

    String html;
    html.reserve(8000);

    html = m_html.getHeader("Device mapping configuration", m_config.getHost(),
                            m_config.getVersion(), 0);

    html += m_html.getDeviceMappingTableHeader();

    for(int i = 0; i < m_config.getNumberOfDeviceMappingEntries(); i++)
    {
        const HSDConfig::DeviceMapping* mapping = m_config.getDeviceMapping(i);
        html += m_html.getDeviceMappingTableEntry(i, mapping);
    }

    html += m_html.getDeviceMappingTableFooter();

    if(m_config.isDeviceMappingFull())
    {
        html +=
            F("</table><p>Edit entry (add not possible, entry limit reached):</p>");
        html += m_html.getDeviceMappingTableAddEntryForm(
            m_config.getNumberOfDeviceMappingEntries(), true);
    }
    else
    {
        html += F("</table><p>Add/edit entry:</p>");
        html += m_html.getDeviceMappingTableAddEntryForm(
            m_config.getNumberOfDeviceMappingEntries(), false);
    }

    html += F("<br/>Delete Entry:<br/>");
    html += m_html.getDeleteForm();

    if(m_config.isDeviceMappingDirty())
    {
        html +=
            F("<p style='color:red'>Unsaved changes! Press "
              "Save"
              " to make them permanent, or they will be lost on next reboot!</p>");
        html += m_html.getSaveForm();
    }

    html += m_html.getFooter();

    Serial.print(F("Page size: "));
    Serial.println(html.length());

    m_server.send(200, F("text/html"), html);

    checkReboot();

    Serial.print(F("Free RAM: "));
    Serial.println(ESP.getFreeHeap());
}

bool HSDWebserver::addDeviceMappingEntry()
{
    bool success = false;

    if(m_server.hasArg("i") && m_server.hasArg("n") && m_server.hasArg("t") &&
       m_server.hasArg("l"))
    {
        if(!m_server.arg("n").isEmpty())
        {
            success = m_config.addDeviceMappingEntry(
                m_server.arg("i").toInt(),
                m_server.arg("n").c_str(),
                static_cast<DeviceType>(m_server.arg("t").toInt()),
                m_server.arg("l").toInt());
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
        // TODO check conversion status
        success = m_config.deleteDeviceMappingEntry(entryNum);
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

    for(uint8_t i = 0; i < m_server.args(); i++)
    {
        html += " " + m_server.argName(i) + ": " + m_server.arg(i) + "\n";
    }

    m_server.send(404, F("text/plain"), html);
}

void HSDWebserver::handleBackup()
{
    String backupJson;
    if(!m_config.createBackup(backupJson))
    {
        Serial.println(F("Backup creation failed"));
        m_server.send(500, "text/plain", "Backup creation failed");
        return;
    }

    m_server.sendHeader("Content-Disposition",
                        "attachment; filename=\"hsd-config-backup.json\"");
    m_server.send(200, "application/json", backupJson);
}

void HSDWebserver::handleRestoreUpload()
{
    HTTPUpload& upload = m_server.upload();

    if(upload.status == UPLOAD_FILE_START)
    {
        Serial.print(F("Restore upload start: "));
        Serial.println(upload.filename);

        m_configRestoreFile = LittleFS.open("/restore.tmp", "w");
        if(!m_configRestoreFile)
        {
            Serial.println(F("Cannot open temporary file for restore"));
        }
    }
    else if(upload.status == UPLOAD_FILE_WRITE)
    {
        if(m_configRestoreFile)
        {
            m_configRestoreFile.write(upload.buf, upload.currentSize);
        }
    }
    else if(upload.status == UPLOAD_FILE_END)
    {
        if(m_configRestoreFile)
        {
            m_configRestoreFile.close();
            Serial.print(F("Restore upload finished, size: "));
            Serial.println(upload.totalSize);
        }
    }
}

void HSDWebserver::handleRestoreFinish()
{
    if(!LittleFS.exists("/restore.tmp"))
    {
        m_server.send(400, "text/plain", "Restore file missing");
        return;
    }

    bool ok = m_config.restoreBackup("/restore.tmp");

    if(!LittleFS.remove("/restore.tmp"))
    {
        Serial.println(F("Warning: failed to remove temporary restore file"));
    }

    if(!ok)
    {
        m_server.send(500, "text/plain", "Restore failed");
        return;
    }

    m_server.send(200, "text/plain", "Restore completed, rebooting...");
    delay(100);
    ESP.restart();
}

void HSDWebserver::checkReboot()
{
    if(m_server.hasArg(F("reset")))
    {
        Serial.println(F("Rebooting ESP."));
        delay(100);
        ESP.restart();
    }
}

bool HSDWebserver::updateMainConfig()
{
    bool needSave = false;

    if(m_server.hasArg(JSON_KEY_HOST))
    {
        needSave |= m_config.setHost(m_server.arg(JSON_KEY_HOST).c_str());
    }

    if(m_server.hasArg(JSON_KEY_WIFI_SSID))
    {
        needSave |= m_config.setWifiSSID(m_server.arg(JSON_KEY_WIFI_SSID).c_str());
    }

    if(m_server.hasArg(JSON_KEY_WIFI_PSK))
    {
        needSave |= m_config.setWifiPSK(m_server.arg(JSON_KEY_WIFI_PSK).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_SERVER))
    {
        needSave |= m_config.setMqttServer(m_server.arg(JSON_KEY_MQTT_SERVER).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_USER))
    {
        needSave |= m_config.setMqttUser(m_server.arg(JSON_KEY_MQTT_USER).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_PASSWORD))
    {
        needSave |= m_config.setMqttPassword(m_server.arg(JSON_KEY_MQTT_PASSWORD).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_STATUS_TOPIC))
    {
        needSave |= m_config.setMqttStatusTopic(m_server.arg(JSON_KEY_MQTT_STATUS_TOPIC).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_TEST_TOPIC))
    {
        needSave |= m_config.setMqttTestTopic(m_server.arg(JSON_KEY_MQTT_TEST_TOPIC).c_str());
    }

    if(m_server.hasArg(JSON_KEY_MQTT_WILL_TOPIC))
    {
        needSave |= m_config.setMqttWillTopic(m_server.arg(JSON_KEY_MQTT_WILL_TOPIC).c_str());
    }

    if(m_server.hasArg(JSON_KEY_LED_COUNT))
    {
        int ledCount = m_server.arg(JSON_KEY_LED_COUNT).toInt();

        if(ledCount > 0)
        {
            needSave |= m_config.setNumberOfLeds(ledCount);
        }
    }

    if(m_server.hasArg(JSON_KEY_LED_PIN))
    {
        int ledPin = m_server.arg(JSON_KEY_LED_PIN).toInt();

        if(ledPin > 0)
        {
            needSave |= m_config.setLedDataPin(ledPin);
        }
    }

    if(m_server.hasArg(JSON_KEY_LED_TYPE))
    {
        needSave |= m_config.setLedType(m_server.arg(JSON_KEY_LED_TYPE).toInt());
    }

    if(m_server.hasArg(JSON_KEY_LED_BRIGHTNESS))
    {
        uint8_t ledBrightness = m_server.arg(JSON_KEY_LED_BRIGHTNESS).toInt();

        if(ledBrightness > 0)
        {
            needSave |= m_config.setLedBrightness(ledBrightness);
        }
    }

    return needSave;
}
