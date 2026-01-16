#pragma once
#include <cstdint>

class IHSDDebug
{
  public:
    enum class LastStep : uint32_t
    {
        None = 0,
        CalcUpTime,
        CheckConnections,
        Wifi,
        Web,
        Mqtt,
        Leds,
        Delay
    };

    struct DebugData
    {
        static constexpr uint32_t MAGIC = 0xDEADBEEF;

        uint32_t magic = 0;
        uint32_t bootCount = 0;
        LastStep lastStep = IHSDDebug::LastStep::None;
        uint32_t lastHeap = 0;
        uint32_t lastHeapFrag = 0;

        bool isValid() const
        {
            return magic == MAGIC;
        }
    };

    virtual ~IHSDDebug() = default;

    virtual void begin() = 0;
    virtual void setStep(LastStep step) = 0;
    virtual DebugData snapShot() = 0;
};

class HSDDebug final : public IHSDDebug
{
  public:
    void begin() override;
    void setStep(IHSDDebug::LastStep step) override;
    IHSDDebug::DebugData snapShot() override;
};

class HSDDebugNull final : public IHSDDebug
{
  public:
    void begin() override {};
    void setStep(IHSDDebug::LastStep step) override {};
    IHSDDebug::DebugData snapShot() override
    {
        return IHSDDebug::DebugData();
    };
};