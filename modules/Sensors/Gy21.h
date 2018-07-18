#include <Task.h>
#include <LibHumidity.h>
LibHumidity gy21 = LibHumidity(0);
class GY21 : public Task
{
  public:
    static GY21 *s_instance;
    GY21() : Task(MsToTaskTime(1000)){};
    static GY21 *instance()
    {
      if (!s_instance)
      s_instance = new GY21;
      return s_instance;
    }
    float GetTemperature(){
      return temperature;
    }
    float GetHumidity(){
      return humidity;
    }
    float GetVpd(){
      return vpd;
    }
    
  private:
    float temperature=0, humidity=0,vpd=0;
    virtual bool OnStart()
    {
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
      temperature = gy21.GetTemperatureC();
      humidity = gy21.GetHumidity();
      vpd = VpdCal(temperature, humidity);
    }

    float VpdCal(float t , float rh) {
      float spv = (610.7) * pow(10, ( (7.5 * t) / (237.3 + t)));
      return (1 - (rh / 100)) * spv;
    }
};

GY21 *GY21::s_instance = 0;
