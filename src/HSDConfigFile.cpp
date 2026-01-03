#include "HSDConfigFile.hpp"
#include <LittleFS.h>

HSDConfigFile::HSDConfigFile(const char* fileName)
    : m_fileName(fileName)
{
}

bool HSDConfigFile::read(char* buffer, size_t bufferSize) const
{
    Serial.print(F("Reading config file "));
    Serial.println(m_fileName);

    if(!LittleFS.exists(m_fileName))
    {
        Serial.println(F("File does not exist"));
        return false;
    }

    File configFile = LittleFS.open(m_fileName, "r");
    if(!configFile)
    {
        Serial.println(F("File open failed"));
        return false;
    }

    const size_t size = configFile.size();
    Serial.print(F("File size is "));
    Serial.print(size);
    Serial.println(F(" bytes"));

    if(size == 0 || size >= bufferSize)
    {
        Serial.println(F("File is empty or too big"));
        configFile.close();
        return false;
    }

    const size_t bytesRead = configFile.readBytes(buffer, size);
    buffer[bytesRead] = '\0';

    configFile.close();
    return true;
}

bool HSDConfigFile::write(const JsonObject& data) const
{
    Serial.print(F("Writing config file "));
    Serial.println(m_fileName);

    File configFile = LittleFS.open(m_fileName, "w");
    if(!configFile)
    {
        Serial.println(F("File open failed"));
        return false;
    }

    serializeJson(data, configFile);
    configFile.close();
    return true;
}

const String& HSDConfigFile::getName() const
{
    return m_fileName;
}
