#include <Task.h>
extern TaskManager taskManager;
class ChannelHanler : public Task
{
  public:
    static ChannelHanler *s_instance;

    ChannelHanler() : Task(MsToTaskTime(100))
    {
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            pinMode(ChannelGpio[i], OUTPUT);
        }
        Init();
    };
    static ChannelHanler *instance()
    {
        if (!s_instance)
            s_instance = new ChannelHanler;
        return s_instance;
    }

    void Update(int ch)
    {
        DigitalWrite(ch-1, CH_OFF);
        taskManager.StopTask(channel[ch - 1]);
        int mode = rom_channel[ch - 1].mode;

        switch (mode)
        {
        case 0:
            channel[ch - 1] = ControlFactory::instance("manual", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 1:
            channel[ch - 1] = ControlFactory::instance("timer", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 2:
            channel[ch - 1] = ControlFactory::instance("setpoint", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 3:
            channel[ch - 1] = ControlFactory::instance("setbound", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        case 4:
            channel[ch - 1] = ControlFactory::instance("irrigation", ch);
            taskManager.StartTask(channel[ch - 1]);
            break;
        default:
            debugCom.println("ChannelHandler: Unknow channel mode " + String(mode));
        }
    }

    static String CO2Status()
    {
        /*
            {
                mode: {0,1}, // active or inactive
                crt:  1800, // current value
                status: {0,1} // feeding or no feed
            }
        */
        // String data = "{ \"mode\":" + String(rom_channel[1].mode) + ", \"crt\":" + String(Sensor::instance()->GetSensor(4)) +
        //               ",\"status\":" + String(ChannelStatus[1]) + ",\"sensor\":" + String(rom_channel[1].sensor) + "}";
        // return "{\"type\": \"co2-status\",\"data\":" + data + "}";

        String data = "{st-co2," + String(rom_channel[1].mode) + 
                    "," + String(Sensor::instance()->GetSensor(4)) + 
                    "," + String(ChannelStatus[1]) + 
                    "," + String(rom_channel[1].sensor)  + "}";
        return data;
    }
    static String ECStatus()
    {
        /*String data = "{ \"mode\":" + String(rom_channel[2].mode) + ", \"crt\":" + String(Sensor::instance()->GetSensor(5)) +
                      ",\"status\":" + String(ChannelStatus[2]) + ",\"sensor\":" + String(rom_channel[2].sensor) + "}";

        return "{\"type\": \"ec-status\",\"data\":" + data + "}";*/

        String data = "{st-ec," + String(rom_channel[2].mode) + 
                    "," + String(Sensor::instance()->GetSensor(5)) + 
                    "," + String(ChannelStatus[2]) + 
                    "," + String(rom_channel[2].sensor)  + "}";
        return data;
    }
    static String PHStatus()
    {
        /*
        String data = "{ \"mode\":" + String(rom_channel[3].mode) + ", \"crt\":" + String(Sensor::instance()->GetSensor(6)) +
                      ",\"status\":" + String(ChannelStatus[3]) + ",\"sensor\":" + String(rom_channel[3].sensor) + "}";
        return "{\"type\": \"ph-status\",\"data\":" + data + "}";
        */
        String data = "{st-ph," + String(rom_channel[3].mode) + 
                    "," + String(Sensor::instance()->GetSensor(6)) + 
                    "," + String(ChannelStatus[3]) + 
                    "," + String(rom_channel[3].sensor)  + "}";
        return data;
    }

    static String GpioStatus(){
        String data = "{st-gpio,";
        for (int i = 0 ; i < CHANNEL_NUMBER ; i++)
        {
            data += String(ChannelStatus[i]);
            if( i < CHANNEL_NUMBER-1) data += ",";
        }
        data += "}";
        return data;
    }

    static String JsonChannelStatus(int start_ch,int number)
    {
        /*
            [
                {
                    status: 0,1
                    mode: [0,5],
                    sensor: rom_channel[i].sensor
                }
            ]
        */
        /*String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            ch[i] = "{ \"status\": " + String(ChannelStatus[i]) + "," +
                    "\"mode\": " + String(rom_channel[i].mode) + "," +
                    "\"sensor\": " + String(rom_channel[i].sensor) + "}";
        }
        String chstr = "";
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            chstr += ch[i];
            if (i != CHANNEL_NUMBER - 1)
                chstr += ",";
        }
        return "{\"type\": \"channel-status\",\"data\": [" + chstr + "]}";*/

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch ; i < end_ch ; i++)
        {
            String chstr = "{ct-chst," + String(i + 1) + 
                        "," + String(rom_channel[i].mode) + 
                        "," + String(rom_channel[i].sensor) +
                        "," + String(rom_channel[i].manual.status) + "}";
            data += chstr;
        }
        return data;
    }

    String JsonManual(int start_ch, int number)
    {
        /*String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String data = "\"manual\":{ \"status\":" + String(rom_channel[i].manual.status) + "}";
            ch[i] = "{ \"ch\":" + String(i + 1) + "," + data + "}";
        }
        return "{\"type\": \"control-manual\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";*/

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch ; i < end_ch ; i++)
        {
            //{ct-manual, ch, status}
            String chstr = "{ct-manual," + String(i + 1) + 
                        "," + String(rom_channel[i].manual.status) + "}";
            data += chstr;
        }
        return data;
    }
    String JsonSetpoint(int start_ch, int number)
    {

        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch ; i < end_ch ; i++)
        {
            //{ct-manual, ch, status}
            String chstr = "{ct-setpoint," + String(i + 1) + 
                        "," + String(rom_channel[i].setpoint.working) +
                        "," + String(rom_channel[i].setpoint.setpoint) +
                        "," + String(rom_channel[i].setpoint.detecting) + "}";
            data += chstr;
        }
        return data;

        /*String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String data = "\"setpoint\": { \"working\":" + String(rom_channel[i].setpoint.working) + ",\"setpoint\":" + String(rom_channel[i].setpoint.setpoint) + ",\"detecting\":" + String(rom_channel[i].setpoint.detecting) + "}";
            ch[i] = "{ \"ch\":" + String(i + 1)  + "," + data + "}";
        }
        return "{\"type\": \"control-setpoint\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";
        */

    }

    String JsonSetbound(int start_ch, int number)
    {
        int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch ; i < end_ch ; i++)
        {
            //{ct-manual, ch, status}
            String chstr = "{ct-setbound," + String(i + 1) + 
                        "," + String(rom_channel[i].setbound.upper) +
                        "," + String(rom_channel[i].setbound.lower) + "}";
            data += chstr;
        }
        return data;
        /*String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String data = "\"setbound\": { \"upper\":" + String(rom_channel[i].setbound.upper) + ",\"lower\":" + String(rom_channel[i].setbound.lower) + "}";
            ch[i] = "{ \"ch\":" + String(i + 1) + "," + data + "}";
        }
        return "{\"type\": \"control-setbound\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";
        */
    }

    String JsonIrrigation()
    {
        String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String data = "\"irrigation\":{ \"soil_upper\": " + String(rom_channel[i].irrigation.soil_upper) + ",\"soil_lower\":" + String(rom_channel[i].irrigation.soil_lower) + ",\"par_accum\":" + String(rom_channel[i].irrigation.par_accum) + ",\"mode\": " + String(rom_channel[i].irrigation.mode) + ",\"working\": " + String(rom_channel[i].irrigation.working) + "}";
            ch[i] = "{ \"ch\":" + String(i + 1)  + "," + data + "}";
        }
        return "{\"type\": \"control-irrigation\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "]}";
    }

    String JsonTimer(int start_ch, int number)
    {
        /*
        String ch[CHANNEL_NUMBER];
        for (int i = 0; i < CHANNEL_NUMBER; i++)
        {
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].timer.size; j++)
            {
                String timer = "[" + String(rom_channel[i].timer.timer_list[j].st) + "," + String(rom_channel[i].timer.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].timer.size - 1))
                {
                    timer_list += ",";
                }
            }
            timer_list += "]";
            // data = "{\"mode\": 1,\"timer_mode\":" + String(rom_channel[i].timer.mode) +
            //         ",\"timer_list\":" + timer_list + "}";
            String data = "\"timer\": { \"mode\":" + String(rom_channel[i].timer.mode) + ",\"list\":" + timer_list + "}";
            ch[i] = "{ \"ch\":" + String(i + 1) + "," + data + "}";
        }
        return "{\"type\": \"control-timer\",\"data\": [" + ch[0] + "," + ch[1] + "," + ch[2] + "," + ch[3] + "," + ch[4] + "]}";
        */
       int end_ch = start_ch + number;
        String data = "";
        for (int i = start_ch ; i < end_ch ; i++)
        {
            String timer_list = "[";
            for (int j = 0; j < rom_channel[i].timer.size; j++)
            {
                String timer = "[" + String(rom_channel[i].timer.timer_list[j].st) + "-" + String(rom_channel[i].timer.timer_list[j].en) + "]";
                timer_list += timer;
                if (j != (rom_channel[i].timer.size - 1))
                {
                    timer_list += "-";
                }
            }
            timer_list += "]";
            data += "{ct-timer," + String(i+1) + "," + String(rom_channel[i].timer.mode) + 
                            "," + timer_list + "}";
        }
        return data;
    }

  private:
    void Init()
    {
        int i = 0;
        for (i = 0; i < CHANNEL_NUMBER; i++)
        {
            int mode = rom_channel[i].mode;
            switch (mode)
            {
            case 0:
                channel[i] = ControlFactory::instance("manual", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 1:
                channel[i] = ControlFactory::instance("timer", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 2:
                channel[i] = ControlFactory::instance("setpoint", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 3:
                channel[i] = ControlFactory::instance("setbound", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            case 4:
                channel[i] = ControlFactory::instance("irrigation", (i + 1));
                taskManager.StartTask(channel[i]);
                break;
            default:
                debugCom.println("ChannelHandler: Unknow channel mode " + String(mode));
            }
        }
    }
    virtual bool OnStart()
    {

        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
    }
};
ChannelHanler *ChannelHanler::s_instance = 0;
