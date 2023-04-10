/**
 * @file Led.cpp
 * @author Pratheep Kumar
 * This file implements the Navigation LED Flashing Pattern for A200
 */

#include "../Copter.h"

#if COPTER_TYPE_A200 == ENABLE

/** 
 * @brief This function checks for the required LED status
 * @param None
 * @details This function check for the LED pattern required and invokes asteria_led_update() to change the LED pattern.
 * @return None
 */
void AP_Asteria::asteria_led_status()
{
	static uint8_t prev_status = 0;

    static Asteria_led_states led_status = (Asteria_led_states)0;///< led status

    /// read parameter [led_toggle_enable] to stop the led pattern
	Asteria_LED_Toggle = copter.g.led_toggle_enable;

    /**
     * If the parameter [led_toggle_enable] is set to 0, 
     * - stop the LED pattern if pattern is set previously
     * - return
     */
	if(Asteria_LED_Toggle == 0)
    {
        if(prev_status == 1)
        {
            hal.gpio->write(RED_LED,0);
            hal.gpio->write(GREEN_LED,0);
            prev_status = 0;
        }
		return;
	}else if(!AP_Notify::flags.armed && Asteria_LED_Toggle!=1)//only for testing the pattern
    {
       static uint8_t prev_led_status = 0;
       
       hal.gpio->pinMode(RED_LED, HAL_GPIO_OUTPUT);
       hal.gpio->pinMode(GREEN_LED, HAL_GPIO_OUTPUT);

       prev_led_status = led_status;

       //Asteria_LED_Toggle (2 to 7 for testing,corresponds to ASTERIA_DISARMED_NO_GPS to ASTERIA_LED_OFF)
       led_status = (Asteria_led_states)(Asteria_LED_Toggle-1);

        if(led_status!= prev_led_status)
        {
            pattern_counter = 0;
        }
        asteria_led_update(led_status);
        return ;
    }else
    {
        Asteria_LED_Toggle = 1; //Enable the pattern if [led_toggle_enable] is other than 0
    }

    ///set AUX 54 and 55 as output  
	hal.gpio->pinMode(RED_LED, HAL_GPIO_OUTPUT);
    hal.gpio->pinMode(GREEN_LED, HAL_GPIO_OUTPUT);

    ///perform the LED patter only when AP is initialised
    if(copter.ap.initialised)
    {
        ///when the mavlink command value MAV_CMD_LED_POWER is 0, set led_status to ASTERIA_LED_OFF
        if(led_power_request != 1)
        {
            led_status = ASTERIA_LED_OFF;
        }///set led_status to ASTERIA_FAILSAFE, when there  any_failsafe_triggered() returns true
        else if(copter.any_failsafe_triggered())
        {
            if(led_status!=ASTERIA_FAILSAFE)
			{
                led_status = ASTERIA_FAILSAFE;
			}
        } ///set led_status to ASTERIA_ARMED_GPS, when Armed with GPS Fix          
        else if((AP_Notify::flags.armed && copter.gps.status() >= AP_GPS::GPS_OK_FIX_2D))
        {
            if(led_status!=ASTERIA_ARMED_GPS)
            {
                led_status = ASTERIA_ARMED_GPS;
                pattern_counter = 0;
            }
        } ///set led_status to ASTERIA_DISARMED_GPS, when Disarmed with GPS Fix             
        else if((!(AP_Notify::flags.armed) && copter.gps.status() >= AP_GPS::GPS_OK_FIX_2D))
        {
            if(led_status!=ASTERIA_DISARMED_GPS)
            {
                led_status = ASTERIA_DISARMED_GPS;
                pattern_counter = 0;
            }
        }///set led_status to ASTERIA_DISARMED_NO_GPS, when Disarmed with No GPS Fix 
        else if((!(AP_Notify::flags.armed) && copter.gps.status() < AP_GPS::GPS_OK_FIX_2D))
        {
            if(led_status!=ASTERIA_DISARMED_NO_GPS)
            {
                led_status = ASTERIA_DISARMED_NO_GPS;
            }
        }else///set led_status to ASTERIA_PIXHAWK_CODE_ERROR,for any other error
        {
            led_status = ASTERIA_PIXHAWK_CODE_ERROR;
        }
        ///invoke the function asteria_led_update() with led_status to process the LED pattern
        asteria_led_update(led_status);
    }

    prev_status = 1;

    return;
}

/** 
 * @brief This function process the LED pattern
 * @param [led_status] this indicates the required LED pattern(Red led AUX54,Green led AUX55)
 * @details This function process the LED pattern(Every 10Hz)
 * @return None
 */
void AP_Asteria::asteria_led_update(Asteria_led_states led_status)
{
    switch(led_status)
    {
    /// when led status is ASTERIA_DISARMED_NO_GPS, Set Solid Red and Solid Green
    case ASTERIA_DISARMED_NO_GPS:
		hal.gpio->write(RED_LED,1);
		hal.gpio->write(GREEN_LED,1);
        break;

    /// when led status is ASTERIA_DISARMED_GPS, 
    /// Set Solid Red and Double Flashing Green leds(Two consecutive blinks, followed by pause. Pattern repeats every 1 sec)
    case ASTERIA_DISARMED_GPS:
		hal.gpio->write(RED_LED,1);
        pattern_counter++;
        if(pattern_counter==1 || pattern_counter==3)
        {
            hal.gpio->write(GREEN_LED,1);
        }
        else
        {
            hal.gpio->write(GREEN_LED,0);
        }
        if(pattern_counter>=12)
        {
			pattern_counter=0;
        }
        break;

    /// when led status is ASTERIA_ARMED_GPS, 
    /// Double Flashing Red and Green leds(Two consecutive blinks, followed by pause. Pattern repeats every 1 sec)
    case ASTERIA_ARMED_GPS:
		pattern_counter++;
		if(pattern_counter==1 || pattern_counter==3)
		{
			hal.gpio->write(RED_LED,1);
			hal.gpio->write(GREEN_LED,1);
		}
		else
		{
			hal.gpio->write(RED_LED,0);
			hal.gpio->write(GREEN_LED,0);
		}
		if(pattern_counter>=12)
		{
			pattern_counter=0;
		}
        break;

    /// when led status is ASTERIA_ARMED_GPS, 
    /// Set Fast Flashing Red and Green LEDs(flashing at 10Hz)
    case ASTERIA_FAILSAFE:
        hal.gpio->toggle(RED_LED);
        hal.gpio->toggle(GREEN_LED);
        break;

    /// when led status is ASTERIA_PIXHAWK_CODE_ERROR, 
    /// Set Solid Red and Off Green
    case ASTERIA_PIXHAWK_CODE_ERROR:
		hal.gpio->write(RED_LED,1);
        hal.gpio->write(GREEN_LED,0);
        break;

    /// when led status is ASTERIA_LED_OFF, Power of both LEDs
    case ASTERIA_LED_OFF:   
		hal.gpio->write(GREEN_LED,0);
        hal.gpio->write(RED_LED,0);
        break;

    default:
        break;
    }
}
#endif