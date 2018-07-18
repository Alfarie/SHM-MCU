#include <Task.h>
#include <SHT1x.h>
extern TaskManager taskManager;

class TRHsht1x : public Task
{
  public:
    static TRHsht1x *s_instance;
    TRHsht1x() : Task(MsToTaskTime(1000)) {
        clk = 10;
        dta = 9;
        temperature = 0;
        humidity = 0;
        _sensor = new SHT1x(clk,dta);
    }
    static TRHsht1x *instance()
    {
        if (!s_instance)
            s_instance = new TRHsht1x;
        return s_instance;
    }
  private:
    SHT1x *_sensor;
    int clk,dta;
    float temperature, humidity;
    virtual bool OnStart()
    {
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time) {
        temperature = _sensor->readTemperatureC();
        humidity = _sensor->readHumidity();
        debugCom.println( String(temperature) + "," + String(humidity));
    }
};
TRHsht1x *TRHsht1x::s_instance = 0;