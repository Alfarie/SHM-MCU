#include <Task.h>
extern TaskManager taskManager;

class ControlFactory 
{
  public:
    static Control * instance(String type, byte channel)
    {
        // (0)Manual, (1)timer, (2)setpoint, (3)setbound, (4)irrigation, (5)misting
        if(type == "manual")
        {
            return new Manual(channel);
        }
        else if(type == "timer")
        {
            return new Timer(channel);
        }
        else if(type == "setpoint")
        {
            return new Setpoint(channel);
        }
        else if(type == "setbound")
        {
            return new SetBound(channel);
        }
        else if(type == "irrigation")
        {
            return new Irrigation(channel);
        }
        else{
            return new Manual(channel);
        }

    }

};