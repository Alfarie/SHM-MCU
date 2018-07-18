#include <Task.h>
extern TaskManager taskManager;
class Irrigation : public Control
{
  public:
    static float channel_paracc[4];
    Irrigation(int ch) : Control(1000, ch)
    {
        UpdateFromRom();
    };

    void SetValue(float su, float sl, float pac, int mode)
    {
        SaveToRom();
    }

    float GetCurrentPar(){
        return current_par_accum;
    }

  private:
    float soil_upper, soil_lower, par_accum, working;
    float current_par_accum;
    int mode, state;
    virtual bool OnStart()
    {
        DigitalWrite(channel - 1, CH_OFF);
        state = 0;
        current_par_accum = 0;
        return true;
    }
    virtual void OnUpdate(uint32_t delta_time)
    {
        

        float par = Sensor::instance()->GetSensor(2);
        float soil = Sensor::instance()->GetSensor(1);
        double max_par = (par_accum * (1000000.00));
        current_par_accum += par;
        Irrigation::channel_paracc[channel - 1] = current_par_accum;
        
        // String str = "Irrigation is running " +
        //            String(channel) + " " +
        //            String(soil_lower) + "/" +
        //            String(soil) + "/" +
        //            String(soil_upper) + "-par" +
        //            String(par) + " " +
        //            String(current_par_accum) + "/" +
        //            String(max_par) + " st" +
        //            String(state) + " m" +
        //            String(mode);
        //debugCom.println(str);

        //only soil
        if (mode == 0)
        {
            if (state == 0)
            {
                bool upperCheck = soil <= soil_upper;
                bool lowerCheck = soil <= soil_lower;

                if (upperCheck && lowerCheck)
                {
                    DigitalWrite(channel - 1, CH_ON);
                    state = 1;
                }
            }

            if (state == 1)
            {
                bool upperCheck = soil >= soil_upper;
                bool lowerCheck = soil >= soil_lower;
                if (upperCheck && lowerCheck)
                {
                    DigitalWrite(channel - 1, CH_OFF);
                    state = 0;
                }
            }
        }

        //only par
        else if (mode == 1)
        {
            if (state == 0)
            {
                
                if (current_par_accum >=  max_par)
                {
                    DigitalWrite(channel-1, CH_ON);
                    current_par_accum = 0;
                    state = 1;
                }
            }
            else if(state == 1){
                currentTime += (delta_time/1000);
                if(currentTime >= working){
                    DigitalWrite(channel - 1, CH_OFF);
                    currentTime = 0;
                    state = 0;
                }
            }
        }

        // soil & par
        else if (mode == 2)
        {

        }
    }

    void UpdateFromRom()
    {
        soil_upper = rom_channel[channel - 1].irrigation.soil_upper;
        soil_lower = rom_channel[channel - 1].irrigation.soil_lower;
        par_accum = rom_channel[channel - 1].irrigation.par_accum;
        working = rom_channel[channel - 1].irrigation.working;
        mode = rom_channel[channel - 1].irrigation.mode;
    }
    void SaveToRom()
    {
        rom_channel[channel - 1].irrigation.soil_upper = soil_upper;
        rom_channel[channel - 1].irrigation.soil_lower = soil_lower;
        rom_channel[channel - 1].irrigation.par_accum = par_accum;
        rom_channel[channel - 1].irrigation.mode = mode;
        rom_channel[channel - 1].irrigation.working = working;
        EEPROM_Manager::Update(channel);
    }
};

float Irrigation::channel_paracc[4] = {0,0,0,0};