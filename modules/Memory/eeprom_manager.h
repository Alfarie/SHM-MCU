#include <EEPROM.h>
    byte mode;
#include "./address.h"

struct data_manual_s{
    byte status;
};
struct data_timer_s{
    byte size;
    byte mode;
    timer_s timer_list[8];
};
struct data_setpoint_s{
    float setpoint;
    float detecting;
    float working;
};
struct data_setbound_s{
    float upper;
    float lower;
};
struct data_irr_s{
    float soil_upper;
    float soil_lower;
    float par_accum;
    float working;
    byte mode;
};
struct data_table_s{
    byte mode;
    byte gpio_status;
    byte sensor;
    data_timer_s timer;
    data_manual_s manual;
    data_setpoint_s setpoint;
    data_setbound_s setbound;
    data_irr_s irrigation;
};

struct data_water_process{
    byte isCir;
    int cirTime;
    int waitingTime;
    byte isFill;
};

struct data_calibration{
    float ecCal;
    float phCal;
};
data_table_s rom_channel[CHANNEL_NUMBER];
data_water_process waterProcess;
data_calibration calibrationData;

class EEPROM_Manager
{
  public:
    EEPROM_Manager(){};
    static int init_byte;
    static int water_process_byte;
    static int calibration_byte;
    static void InitEEPROM()
    {
        #if !defined(ARDUINO_ARCH_AVR)
            EEPROM.begin(EEPROM_SIZE);
        #endif
        byte init;
        EEPROM.get(init_byte, init);
        //testCom.println("init:" + String(init));
        if (init == 2)
        {
            //debugCom.println("Initialize eeprom GetAll data");
            //testCom.println("Initialize eeprom GetAll data");
            mpuCom.println("INFO-GETMEMORY");
            GetAll();
        }
        else
        {
            //debugCom.println("Initialize eeprom create new setting");
            //testCom.println("Initialize eeprom create new setting");
            mpuCom.println("INFO-CREATEMEMORY");
            data_table_s dt = InitData();
            data_water_process wt = InitWaterProcess();
            data_calibration cal = {1.1, 1.1};

            // {timer,3,1,300-480,540-720,780-960,1020-1200,1260-1439}
            dt.mode = 1; // led timer
            dt.timer.mode = 0;
            EEPROM.put(channel_list[0], dt);

            dt.mode = 3; // co2 setbound
            dt.sensor = 4;
            dt.setbound.lower = 1200;
            dt.setbound.upper = 1500;
            EEPROM.put(channel_list[1], dt);

            dt.mode = 2; // ec setpoint
            dt.sensor = 5;
            dt.setpoint.setpoint = 1.2;
            dt.setpoint.working = 15;
            dt.setpoint.detecting = 30;
            EEPROM.put(channel_list[2], dt);

            dt.mode = 2; // ph setpoint
            dt.sensor = 6;
            dt.setpoint.setpoint = 7.5;
            dt.setpoint.working = 15;
            dt.setpoint.detecting = 30;
            EEPROM.put(channel_list[3], dt);
            
            dt.mode = 5;
            EEPROM.put(channel_list[4], dt);
            EEPROM.put(channel_list[5], dt);
            
            EEPROM.put(water_process_byte, wt);
            EEPROM.put(calibration_byte, cal);

            EEPROM.put(init_byte, 2);
			Commit();
            GetAll();
        }
    }
    static void Update(int channel)
    {
        DisplayLog::PrintInfo(" Update to Channel " + String(channel) + " " + String(channel_list[channel - 1]));

        // Serial.println("size: " + String(rom_channel[channel - 1].timer_size));
        // for(int i = 0 ; i < rom_channel[channel - 1].timer_size; i++){
        //     Serial.print( String(rom_channel[channel - 1].timer_list[i].st) + "-" + String(rom_channel[channel - 1].timer_list[i].en) + " ");
        // }
        // Serial.println();
        EEPROM.put(channel_list[channel - 1], rom_channel[channel - 1]);
        Commit();
    }
    static void UpdateWaterProcess(){
        EEPROM.put(water_process_byte, waterProcess);
        //testCom.println("WaterProcess Updated");
        Commit();
    }

    static void UpdateCalibration(){
        EEPROM.put(calibration_byte, calibrationData);
        //testCom.println("WaterProcess Updated");
        Commit();
    }
    static data_water_process InitWaterProcess(){
        data_water_process water_process;
        water_process = {0};
        water_process.isCir = true;
        water_process.cirTime = 900;
        water_process.waitingTime = 900;
        water_process.isFill = true;

        return water_process;
    }
    static data_table_s InitData(){
        data_table_s initData;
        initData = {0};
        initData.mode = 0;
        initData.sensor = 1;
        initData.manual.status = 0;
        
        //{timer,3,1,300-480,540-720,780-960,1020-1200,1260-1439}
        timer_s timer_list[8];
        initData.timer.timer_list[0] = {300,480};
        initData.timer.timer_list[1] = {540,720};
        initData.timer.timer_list[2] = {780,960};
        initData.timer.timer_list[3] = {1020,1200};
        initData.timer.timer_list[4] = {1260,1439};
        initData.timer.size = 5;
        initData.timer.mode = 0;

        initData.setpoint.setpoint = 40.0;
        initData.setpoint.detecting = 30;
        initData.setpoint.working = 30;

        initData.setbound.upper = 60;
        initData.setbound.lower = 40;

        initData.irrigation.soil_upper = 60;
        initData.irrigation.soil_lower = 40;
        initData.irrigation.par_accum = 1.5;
        initData.irrigation.working = 30;
        initData.irrigation.mode = 0;
        return initData;
    }

    static void GetAll()
    {
        for(int i = 0 ; i < CHANNEL_NUMBER ; i++){
            EEPROM.get(channel_list[i], rom_channel[i]);
        }
        EEPROM.get(water_process_byte, waterProcess);
        EEPROM.get(calibration_byte, calibrationData);
    }

    static void Commit(){
        #if !defined(ARDUINO_ARCH_AVR)
		    EEPROM.commit();
        #endif
    }
  private:
    static int channel_list[CHANNEL_NUMBER];
};
int EEPROM_Manager::init_byte = 0;
int EEPROM_Manager::water_process_byte = 50;
int EEPROM_Manager::calibration_byte = 20;
int EEPROM_Manager::channel_list[CHANNEL_NUMBER] = {100, 200, 300, 400, 500, 600};

