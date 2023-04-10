/**
 *  * @file Asteria.h
 * This file will have Asteria class details, other globals, Macros used
 * 
 */
#pragma once

#include "../Copter.h"

/*Add all Asteria related defines here*/
#ifndef MAX_WIND_SPEED_DEFAULT
 # define MAX_WIND_SPEED_DEFAULT  13.00f ///<high wind threshold default value
#endif

#if COPTER_TYPE_A200 == ENABLE

#define RED_LED 54  //PIXHAWK_AUX_5 
#define GREEN_LED 55 //PIXHAWK_AUX_6

/// Different LED state enums
enum Asteria_led_states
{
	ASTERIA_INITIALIZING_AUTOPILOT = 0,
	ASTERIA_DISARMED_NO_GPS = 1,
	ASTERIA_DISARMED_GPS = 2,
	ASTERIA_ARMED_GPS = 3,
	ASTERIA_FAILSAFE = 4,
	ASTERIA_PIXHAWK_CODE_ERROR = 5,
	ASTERIA_LED_OFF = 6
};
#endif

/** @brief AP_Asteria class
*
*   This class is used for Asteria related custom features related implementation
*/
class AP_Asteria
{
    public:
        /// Asteria constructor
        AP_Asteria()
        {
            disarm_counter = 0; 
            flight_counter = 0;
            inavi_alt = 0;

            wind_speed = 0;
            wind_direction = 0;
            high_windspeed_flag = 0;
            failsafe.wind_fs = 0;

#if COPTER_TYPE_A200 == ENABLE
            pattern_counter=0;
            Asteria_LED_Toggle = 0;
            led_power_request = 1; ///< mavlink command value default ON
#endif

        }
        void disarm_sequence_number();
        void flight_sequence_number();
        void send_seq_numbers(mavlink_channel_t chan);
        void send_prearm_flag(mavlink_channel_t chan);

        void set_Wind_dir_Wind_Spd();
        bool high_wind_failsafe();
        void high_wind_flag_failsafe();
        bool have_wind_failsafe() const;
        void log_wind_estimate_to_Dflash(void);
        void send_wind_estimate_to_GCS(mavlink_channel_t chan);

#if COPTER_TYPE_A200 == ENABLE
        void asteria_led_status();
        void asteria_led_update(Asteria_led_states led_status);
        uint8_t led_power_request; ///<mavlink command value MAV_CMD_LED_POWER(1=ON, 0=OFF)
#endif

    private:

        int16_t disarm_counter;      ///< Disarm counter
        int16_t flight_counter;      ///< flight counter

        float inavi_alt;             ///< to hold the current altitude value.

        float wind_speed;            ///< Wind speed
        float wind_direction;        ///< Wind direction
        bool  high_windspeed_flag;   ///< high windspeed flag

        /// structure for Asteria related Failsafe
        struct {
            uint8_t wind_fs             : 1; ///< true if wind_failsafe has triggered
            } failsafe;

#if COPTER_TYPE_A200 == ENABLE
        uint8_t pattern_counter;       ///< led pattern counter
        uint8_t Asteria_LED_Toggle;    ///< To read parameter [led_toggle_enable]     
#endif

};



