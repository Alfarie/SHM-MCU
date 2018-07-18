#include <Task.h>
extern TaskManager taskManager;
class Solution : public Task
{
public:
  static Solution *s_instance;
  Solution() : Task(MsToTaskTime(50)){};
  static Solution *instance()
  {
    if (!s_instance)
      s_instance = new Solution;
    return s_instance;
  }

  float GetEC()
  {
    return ec;
  }
  float GetpH()
  {
    return ph;
  }
  float GetWaterTemp()
  {
    return water;
  }

  int GetFloating(){
    return floating;
  }

private:
  float ec=0, ph=0, water=0;
  int floating=0;
  String cmdStr;
  String sensorStr = "0.00,0.00,0.0,0,0";
  char res[100];
  int size, cmdNumber, cmdSize;
  virtual bool OnStart()
  {
    ec = 0;
    ph = 0;
    water = 0;
    floating = 0;
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    while (solCom.available())
    {
      if (solCom.read() == '{')
      {
        int size = 0;
        while (true)
        {
          if (solCom.available())
          {
            char ch = solCom.read();
            if (ch == '}')
            {
              res[size] = '\0';
              String resData = res;
              float s[5];
              ExtractDataFloat(s, 4, resData);
              //25.00 50.00 50.00 60.00 1583.54
              ec = s[0];
              ph = s[1];
              water = s[2];
              floating = s[3];
              // //debugCom.println(PrintSensor());
              break;
            }
            else
            {
              res[size] = ch;
              size++;
            }
          }
        }
      }
    }
  }
  void ExtractDataFloat(float *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toFloat();
      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }
  
  String PrintSensor()
  {
    String str = String(ec) + " " + 
    String(ph) + " " +
    String(water);
    return str;
  }
};
Solution *Solution::s_instance = 0;