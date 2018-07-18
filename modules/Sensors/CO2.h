#include <Task.h>
extern TaskManager taskManager;
#include <K30_I2C.h>
K30_I2C k30_i2c = K30_I2C(0x34);

class Co2Sensor : public Task
{
public:
  static Co2Sensor *s_instance;
  Co2Sensor() : Task(MsToTaskTime(100)){};
  static Co2Sensor *instance()
  {
    if (!s_instance)
      s_instance = new Co2Sensor;
    return s_instance;
  }
  float GetCO2()
  {
    return _ppmCO2;
  }

private:
  int _ppmCO2=0, tempco2=0;
  int rc=0;
  virtual bool OnStart()
  {
    
    return true;
  }

  virtual void OnUpdate(uint32_t delta_time)
  {
    rc = k30_i2c.readCO2(_ppmCO2);
    if (rc == 0)
    {
      tempco2 = _ppmCO2;
    }
    else
    {
      _ppmCO2 = tempco2;
    }
    
    if(_ppmCO2 > 10000 || _ppmCO2 < 0 || rc == 0) {
      _ppmCO2 = -1;
    }
  }
};
Co2Sensor *Co2Sensor::s_instance = 0;