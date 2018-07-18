#include <Task.h>
extern TaskManager taskManager;
class Communication : public Task
{
public:
  static Communication *s_instance;
  Communication() : Task(MsToTaskTime(1))
  {
    cmdStr = "";
    cmdNumber = 0;
    cmdSize = 0;
    size = 0;
  };
  static Communication *instance()
  {
    if (!s_instance)
      s_instance = new Communication;
    return s_instance;
  }

private:
  String cmdStr;
  char res[100];
  int size, cmdNumber, cmdSize;
  virtual bool OnStart()
  {
    return true;
  }
  virtual void OnUpdate(uint32_t delta_time)
  {
    //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}

    while (mpuCom.available())
    {
      if (mpuCom.read() == '{')
      {
        int size = 0;
        while (true)
        {
          if (mpuCom.available())
          {
            char ch = mpuCom.read();
            if (ch == '}')
            {
              res[size] = '\0';
              String strRes = res;
              // CommandProcess(String(res));
              cmdStr += "{" + strRes + "}";
              cmdNumber++;
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

    if (cmdNumber > 0)
    {

      for (int i = 0; i < cmdNumber; i++)
      {
        String subcom = cmdStr.substring(1, cmdStr.indexOf("}"));
        CommandProcess(subcom);
        cmdStr = cmdStr.substring(cmdStr.indexOf("}") + 1);
      }

      cmdNumber = 0;
      cmdStr = "";
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

  void ExtractDataInt(int *data, int size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < size)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a.toInt();

      si = index;
      res = res.substring(index + 1);
      j++;
    }
  }

  void ExtractDataString(String *data, int &size, String res)
  {
    int i = 0, si = 0, ei, j = 0;
    while (j < 20)
    {
      int index = res.indexOf(",");
      String a = res.substring(0, index);
      data[j] = a;
      // mpuCom.println(data[j]);
      si = index;
      res = res.substring(index + 1);
      j++;
      if (index == -1)
      {
        size = j;
        return;
      }
    }
  }

  timer_s *TimerStringToTimer(String *timer, timer_s *vtimer, int size)
  {
    for (int i = 0; i < size; i++)
    {
      int index = timer[i].indexOf("-");
      String startStr = timer[i].substring(0, index);
      String endStr = timer[i].substring(index + 1);

      int start = startStr.toInt();
      int end = endStr.toInt();
      timer_s t = {start, end};
      vtimer[i] = t;
    }
  }
  //{timer,1,1,20-60,90-150,200-260}{setpoint,2,50.5,30,30,1}{manual,3,0}{manual,4,1}
  bool CommandProcess(String res)
  {
    // mpuCom.println(res);
    // {datetime,8,1,17,6,31}
    if (res.startsWith("Gsensors"))
    {
      mpuCom.println(Sensor::instance()->GetSensorsString());
    }
    else if(res.startsWith("Gdatetime")){
      mpuCom.println(RTC::instance()->GetDateTimeString());
    }
    else if (res.startsWith("checkstatus"))
    {
      mpuCom.println(MCU_STATE);
    }
    else if(res.startsWith("freememory"))
    {
      String data = "{\"type\": \"free-memory\",\"data\": " + String(MemoryCheck::instance()->GetFreeMemory()) + "}";
      mpuCom.println(data);
    }
    else if(res.startsWith("Ggpio")){
      res.replace("Ggpio,", "");
      res.trim();
      mpuCom.println(ChannelHanler::instance()->GpioStatus());
    }
    else if (res.startsWith("Gcontrol"))/*  */
    {
      // ChannelHanler::instance()->JsonSettingAll();
      // mpuCom.println(ChannelHanler::instance()->JsonControl());
      res.replace("Gcontrol,", "");
      res.trim();
      String data[3];
      int size = 0;
      ExtractDataString(data, size,res);
      String cmd = data[0];
      int start_ch = data[1].toInt() - 1;
      int number = data[2].toInt();

      if(cmd.startsWith("channelstatus"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonChannelStatus(start_ch, number));
      }
      else if(cmd.startsWith("manual"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonManual(start_ch, number));
      }
      else if(cmd.startsWith("timer"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonTimer(start_ch, number));
      }
      else if(cmd.startsWith("setpoint"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonSetpoint(start_ch, number));
      }
      else if(cmd.startsWith("setbound"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonSetbound(start_ch, number));
      }
      else if(cmd.startsWith("irrigation"))
      {
        mpuCom.println(ChannelHanler::instance()->JsonIrrigation());
      }
    }

    else if (res.startsWith("datetime"))
    {
      res.replace("datetime,", "");
      res.trim();
      int dt[5] = {0, 0, 0, 0, 0};
      ExtractDataInt(dt, 5, res);
      // (byte s, byte m, byte h, byte dow, byte dom, byte mo, byte y)
      RTC::instance()->setDateDs1307(0, dt[4], dt[3], 0, dt[0], dt[1], dt[2]);
      mpuCom.println("UPD-DATETIME" );
    }

    else if(res.startsWith("mode"))
    {
      res.replace("mode,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-MODE-" + String(ch));
    }

    //{manual, channel, status}
    // {manual, 1,1}
    else if (res.startsWith("manual"))
    {
      res.replace("manual,", "");
      res.trim();
      int mode[2] = {0, 0};
      ExtractDataInt(mode, 2, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 0;
      rom_channel[ch - 1].manual.status = mode[1];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-MANUAL-" + String(ch));
    }
    // {setpoint,channel,setpoint_value, working, detecting, sensor}
    // {setpoint,1,50.5,30,30,1}
    else if (res.startsWith("setpoint"))
    {
      res.replace("setpoint,", "");
      res.trim();
      float mode[5];
      ExtractDataFloat(mode, 5, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 2;
      rom_channel[ch - 1].setpoint.setpoint = mode[1];
      rom_channel[ch - 1].setpoint.working = mode[2];
      rom_channel[ch - 1].setpoint.detecting = mode[3];
      rom_channel[ch - 1].sensor = (byte)mode[4];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-SETPOINT-" + String(ch));
    }
    //{timer,1,1,20-60,90-150,200-260}
    else if (res.startsWith("timer"))
    {
      res.replace("timer,", "");
      res.trim();
      int ch = res.substring(0, 1).toInt();
      int timer_mode = res.substring(2, 3).toInt();
      res = res.substring(4);
      String timer[15];
      int size = 0;
      ExtractDataString(timer, size, res);
      timer_s timerlist[8];
      TimerStringToTimer(timer, timerlist, size);
      
      memcpy(rom_channel[ch - 1].timer.timer_list, timerlist, 8 * sizeof(timer_s));
      
      rom_channel[ch - 1].mode = 1;
      rom_channel[ch - 1].timer.size = size;
      rom_channel[ch - 1].timer.mode = timer_mode;
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-TIMER-" + String(ch));
    }
    //{setbound, channel, upper,lower,sensor}
    // {setbound, 1, 40.0, 50.0, 1}
    else if (res.startsWith("setbound"))
    {
      res.replace("setbound,", "");
      res.trim();
      float mode[4];
      ExtractDataFloat(mode, 4, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 3;
      rom_channel[ch - 1].setbound.upper = mode[1];
      rom_channel[ch - 1].setbound.lower = mode[2];
      rom_channel[ch - 1].sensor = (byte)mode[3];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-SETBOUND-" + String(ch));
    }
    
    //{irrigation,ch, irr_mode,soil_up, soil_low, par_acc, working_time}
    //{irrigation,1, 0, 60,40,1.5, 30}
    else if (res.startsWith("irrigation"))
    {
      res.replace("irrigation,", "");
      res.trim();
      float mode[6];
      ExtractDataFloat(mode, 6, res);
      int ch = (int)mode[0];
      rom_channel[ch - 1].mode = 4;
      rom_channel[ch - 1].irrigation.mode = (int)mode[1];
      rom_channel[ch - 1].irrigation.soil_upper = mode[2];
      rom_channel[ch - 1].irrigation.soil_lower = mode[3];
      rom_channel[ch - 1].irrigation.par_accum = mode[4];
      rom_channel[ch -1].irrigation.working = mode[5];
      EEPROM_Manager::Update(ch);
      ChannelHanler::instance()->Update(ch);
      mpuCom.println("UPD-IRR-" + String(ch));
    }
  //{waterprocess,1,1,10,10}
    else if (res.startsWith("waterprocess"))
    {
      res.replace("waterprocess,", "");
      res.trim();
      int mode[4] = {0, 0, 0, 0};
      ExtractDataInt(mode, 4, res);
      waterProcess.isCir = mode[0];
      waterProcess.isFill = mode[1];
      waterProcess.cirTime = mode[2];
      waterProcess.waitingTime = mode[3];
      EEPROM_Manager::UpdateWaterProcess();
      mpuCom.println("UPD-WATER");
    }
    else if (res.startsWith("Gwater-status"))
    {
      mpuCom.println(WaterProcessControl::GetStatus());
    }
    else if (res.startsWith("Gco2-status"))
    {
      mpuCom.println(ChannelHanler::instance()->CO2Status());
    }
    else if (res.startsWith("Gec-status"))
    {
      mpuCom.println(ChannelHanler::instance()->ECStatus());
    }
    else if (res.startsWith("Gph-status"))
    {
      mpuCom.println(ChannelHanler::instance()->PHStatus());
    }
    else if (res.startsWith("Gwater-control"))
    {
      mpuCom.println(WaterProcessControl::GetControl());
    }
    else if (res.startsWith("getcal"))
    {
      // String str = "{\"type\": \"calibration\",\"data\":{\"ec\":" + String(calibrationData.ecCal) 
      //               + ",\"ph\":"+String(calibrationData.phCal) + "}}";
      String cal = "{se-cal," + String(calibrationData.ecCal)  + "," + String(calibrationData.phCal) + "}";
      mpuCom.println(cal);
    }
    else if (res.startsWith("setcal"))
    {
      res.replace("setcal,", "");
      res.trim();
      float mode[2];
      ExtractDataFloat(mode, 2, res);
      calibrationData.ecCal = mode[0];
      calibrationData.phCal = mode[1];
      EEPROM_Manager::UpdateCalibration();
      mpuCom.println("UPD-SETCAL");
    }
    else if (res.startsWith("clear-memory"))
    {
      EEPROM.write(EEPROM_Manager::init_byte, 0);
      mpuCom.println("UPD-CLEARMEM");
    }
    else if (res.startsWith("done"))
    {
      mpuCom.println("DONE");
    }
  }
};
Communication *Communication::s_instance = 0;