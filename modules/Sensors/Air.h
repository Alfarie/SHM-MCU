#include <Task.h>
extern TaskManager taskManager;
class Air : public Task
{
  public:
    static Air *s_instance;
    Air() : Task(MsToTaskTime(50))
    {
        temperature = 0;
        humidity = 0;
        co2 = 0;
    }
    static Air *instance()
    {
        if (!s_instance)
            s_instance = new Air;
        return s_instance;
    }

    float GetTemperature(){
        return temperature;
    }

    float GetHumidity(){
        return humidity;
    }

    float GetCO2(){
        return co2;
    }

  private:
    float temperature, humidity, co2;
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
        while (airCom.available())
        {
            if (airCom.read() == '{')
            {
                int size = 0;
                while (true)
                {
                    if (airCom.available())
                    {
                        char ch = airCom.read();
                        if (ch == '}')
                        {
                            res[size] = '\0';
                            String resData = res;
                            float s[4];
                            ExtractDataFloat(s, 4, resData);
                            //25.00 50.00 50.00 60.00 1583.54
                            co2 = s[2];
                            temperature = s[0];
                            humidity = s[1];
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
        String str = String(temperature) + " " +
                     String(humidity) + " " +
                     String(co2);
        return str;
    }
};
Air *Air::s_instance = 0;