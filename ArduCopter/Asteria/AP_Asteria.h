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

    private:

        int16_t disarm_counter; ///< Disarm counter
        int16_t flight_counter; ///< flight counter

        float inavi_alt;        ///< to hold the current altitude value.

        float wind_speed;       ///< Wind speed
        float wind_direction;   ///< Wind direction
        bool  high_windspeed_flag; ///< high windspeed flag

        /// structure for Asteria related Failsafe
        struct {
            uint8_t wind_fs             : 1; ///< true if wind_failsafe has triggered
            } failsafe;
};

