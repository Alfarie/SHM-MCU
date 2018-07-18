#include <Task.h>
extern TaskManager taskManager;
#define pH_SENSOR A1
class pHSensor : public Task
{
public:
  static pHSensor *s_instance;
  pHSensor() : Task(MsToTaskTime(100)){
    aVal = 0;
    ph = 0;
    
  };
  static pHSensor *instance()
  {
    if (!s_instance)
      s_instance = new pHSensor;
    return s_instance;
  }

  float GetRaw(){
    return aVal;
  }

  float GetPH(){
    return ph;
  }

private:
  float aVal,ph;
  int analogReadVal[30];
  int readIndex = 0;
  virtual bool OnStart()
  {
    ph = 0;
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    int val = analogRead(pH_SENSOR);
    analogReadVal[readIndex] = val;
    readIndex++;
    if (readIndex >= 20)
    {
      aVal = 0;
      qsort(analogReadVal, readIndex, sizeof(analogReadVal[0]), sort_desc);
      int num = 0;
      for (int i = 2; i < readIndex - 2; i++)
      {
        aVal += analogReadVal[i];
        num++;
      }
      aVal = aVal / num;
      aVal = aVal * 5000 / 1024;
      readIndex = 0;

      ph = 3.5 * aVal;
      ph = ph / 1000;
    }
  }

  static int sort_desc(const void *cmp1, const void *cmp2)
  {
    // Need to cast the void * to int *
    int a = *((int *)cmp1);
    int b = *((int *)cmp2);
    // The comparison
    return a < b ? -1 : (a > b ? 1 : 0);
    // A simpler, probably faster way:
    //return b - a;
  }
};
pHSensor *pHSensor::s_instance = 0;