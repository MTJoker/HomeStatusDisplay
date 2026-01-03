#pragma once

#include <ArduinoJson.h>

class HSDConfigFile
{
  public:
    explicit HSDConfigFile(const char* fileName);

    bool read(char* buffer, size_t bufferSize) const;
    bool write(const JsonObject& data) const;

  private:
    String m_fileName;
};
