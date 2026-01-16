#pragma once

#include <ArduinoJson.h>
#include <string_view>

class HSDConfigFile
{
  public:
    explicit HSDConfigFile(std::string_view fileName) noexcept;

    bool read(char* buffer, size_t bufferSize) const noexcept;
    bool write(JsonObjectConst data) const noexcept;

    [[nodiscard]] const String& getName() const noexcept;

  private:
    String m_fileName;
};