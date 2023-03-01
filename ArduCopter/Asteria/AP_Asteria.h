/**
 *  * @file Asteria.h
 * This file will have Asteria class details, other globals, Macros used
 * 
 */
#pragma once

#include "../Copter.h"


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

        }
        void disarm_sequence_number();
        void flight_sequence_number();
        void send_seq_numbers(mavlink_channel_t chan);
        void send_prearm_flag(mavlink_channel_t chan);

    private:

        int16_t disarm_counter; ///< Disarm counter
        int16_t flight_counter; ///< flight counter

        float inavi_alt;        ///< to hold the current altitude value.
};

