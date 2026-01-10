#include "HSDConfigFile.hpp"

#include <LittleFS.h>

HSDConfigFile::HSDConfigFile(std::string_view fileName) noexcept
    : m_fileName(fileName.data())
{
}

bool HSDConfigFile::read(char* buffer, size_t bufferSize) const noexcept
{
    if(buffer == nullptr || bufferSize < 2)
    {
        Serial.println(F("Invalid buffer"));
        return false;
    }

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

    const size_t fileSize = configFile.size();
    Serial.print(F("File size is "));
    Serial.print(fileSize);
    Serial.println(F(" bytes"));

    if(fileSize == 0 || fileSize >= bufferSize)
    {
        Serial.println(F("File is empty or too big for buffer"));
        configFile.close();
        return false;
    }

    const size_t bytesRead = configFile.readBytes(buffer, fileSize);
    buffer[bytesRead] = '\0';

    configFile.close();
    return true;
}

bool HSDConfigFile::write(JsonObjectConst data) const noexcept
{
    Serial.print(F("Writing config file "));
    Serial.println(m_fileName);

    File configFile = LittleFS.open(m_fileName, "w");
    if(!configFile)
    {
        Serial.println(F("File open failed"));
        return false;
    }

    const size_t written = serializeJson(data, configFile);
    configFile.close();

    if(written == 0)
    {
        Serial.println(F("serializeJson failed"));
        return false;
    }

    return true;
}

const String& HSDConfigFile::getName() const noexcept
{
    return m_fileName;
}
