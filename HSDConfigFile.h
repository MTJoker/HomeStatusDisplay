#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class HSDConfigFile
{
  
public:

  HSDConfigFile(String fileName, int maxSize);
  virtual ~HSDConfigFile();

  bool read();
  bool write(JsonObject* data);
  const char* getData();
  
private:

  String m_fileName;
  int m_maxSize;
  char * m_buffer;
};

