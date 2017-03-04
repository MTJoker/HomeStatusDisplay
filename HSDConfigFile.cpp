#include "HSDConfigFile.h"
#include <FS.h>

HSDConfigFile::HSDConfigFile(String fileName, int maxSize)
:
m_fileName(fileName),
m_maxSize(maxSize)
{
  m_buffer = new char[maxSize];  
}

HSDConfigFile::~HSDConfigFile()
{
  if(m_buffer)
  {
    delete[] m_buffer;
  }
}

bool HSDConfigFile::read()
{
  bool success = false;

  Serial.print(F("Reading config file ")); 
  Serial.println(m_fileName); 

  if(SPIFFS.exists(m_fileName)) 
  {
    File configFile = SPIFFS.open(m_fileName, "r");

    if(configFile)
    {
      size_t size = configFile.size();
      Serial.print(F("File size is ")); Serial.println(String(size) + " bytes"); 

      if(size <= m_maxSize)
      { 
        configFile.readBytes(m_buffer, size);
        success = true;
      }
      else
      {
        Serial.println(F("File is too big"));
      }
    }
    else
    {
      Serial.println(F("File open failed"));
    }
  }
  else
  {
    Serial.println(F("File does not exist"));
  }

  return success;
}

bool HSDConfigFile::write(JsonObject* data)
{
  bool success = false;

  Serial.print(F("Writing config file ")); 
  Serial.println(m_fileName); 

  if(SPIFFS.exists(m_fileName)) 
  {
    File configFile = SPIFFS.open(m_fileName, "w");

    if(configFile)
    {
      data->prettyPrintTo(Serial);
      Serial.println(F(""));
    
      data->printTo(configFile);
      configFile.close();
      
      success = true;
    }
    else
    {
      Serial.println(F("File open failed"));
    }
  }
  else
  {
    Serial.println(F("File does not exist"));
  }

  return success;
}

const char* HSDConfigFile::getData()
{
  return m_buffer;
}

