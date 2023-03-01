/**
 * @file Prearm_flag.cpp
 * This file contains below Asteria added feature
 *  1. To send pre-arm flag as Mavlink packet at 1Hz 
 */

#include "../Copter.h"

/** 
 * @brief This function send pre-arm flag as Mavlink packet at 1Hz
 * @param [mavlink_channel_t chan]
 * @details This function send pre-arm flag as Mavlink packet at 1Hz
 * @return None
 */
void AP_Asteria::send_prearm_flag(mavlink_channel_t chan)
{
	static uint8_t timeout = 0;
	mavlink_pre_arm_flag_t var_prearm;

	var_prearm.prearm_flag = copter.ap.pre_arm_check;

    /**
     * Send prearm flag value(copter.ap.pre_arm_check) to mavlink message using mavlink_msg_pre_arm_flag_send() 
     * every 10 cycles once
     */

	if(timeout++ >= 10)
	{

		mavlink_msg_pre_arm_flag_send(chan,var_prearm.prearm_flag);

		timeout = 0;
	}

}

