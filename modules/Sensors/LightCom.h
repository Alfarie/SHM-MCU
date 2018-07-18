#include <Task.h>
extern TaskManager taskManager;
class LightCom : public Task
{
  public:
    static LightCom *s_instance;
    LightCom() : Task(MsToTaskTime(50))
    {
        light = 0;
    }
    static LightCom *instance()
    {
        if (!s_instance)
            s_instance = new LightCom;
        return s_instance;
    }

    float GetLight(){
        return light;
    }


  private:
    float light;
    String cmdStr;
    String sensorStr = "0.00,0.00,0.0,0,0";
    char res[100];
    int size, cmdNumber, cmdSize;
    virtual bool OnStart()
    {
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        while (lightCom.available())
        {
            if (lightCom.read() == '{')
            {
                int size = 0;
                while (true)
                {
                    if (lightCom.available())
                    {
                        char ch = lightCom.read();
                        if (ch == '}')
                        {
                            res[size] = '\0';
                            String resData = res;
                            float s[4];
                            ExtractDataFloat(s, 1, resData);
                            //25.00 50.00 50.00 60.00 1583.54
                            light = s[0];
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
        String str = String(light);
        return str;
    }
};
LightCom *LightCom::s_instance = 0;