/**
 * @file Seq_num.cpp
 * This file contains below Asteria added features
 *  1. Disarm sequence number calculation
 *  2. Flight sequence number calculation
 */

#include "../Copter.h"

/** 
 * @brief This function performs Disarm sequence number calculation
 * @param None
 * @details This function maintains the  count of the number of times 
 * the UAV state goes from ARMED to DISARMED state
 * @return None
 * @note Disarm sequence number does not get erased on power up or on flashing new firmware to the autopilot.
 */

void AP_Asteria::disarm_sequence_number()
{
	static int16_t disarm_arm_flag=0;

    ///get previous Disarm counter using g.disarm_seq_num.get()
	disarm_counter=copter.g.disarm_seq_num.get(); 

	/// increment Disarm counter only when UAV state changed from ARMED to DISARMED
	if(AP_Notify::flags.armed)
	{
		disarm_arm_flag=1;
	}

	if(!(AP_Notify::flags.armed) && disarm_arm_flag==1)
	{
		disarm_counter++;
		disarm_arm_flag=0;
	}
	
    /**
	 * Save the Disarm counter,set_and_save_ifchanged() will force save the parameter again and again on sdcard
	 * and will log number of instances it will change.
	 */

	copter.g.disarm_seq_num.set_and_save_ifchanged(disarm_counter);
}

/**
 * @brief This function performs the Flight sequence number calculation
 * @param None
 * @details This function maintains the count of the number of
 * times UAV altitude is above 5 meters since last ARM state.
 * @return None
 * @note Flight sequence number does not get erased on power up or on flashing new firmware to the autopilot.
 */

void AP_Asteria::flight_sequence_number()
{
	static int16_t flight_arm_flag=0;
    
    ///get previous flight counter using g.flight_seq_num.get()
	flight_counter=copter.g.flight_seq_num.get();

	///get the UAV current altitude
	inavi_alt=(copter.inertial_nav.get_position_z_up_cm()/100.0f);

    /// increment flight counter when UAV state becomes DISARMED after a flight
    /// (flight is considered when UAV is ARMED and UAV altitude become above 5 meters)
	if(AP_Notify::flags.armed && inavi_alt >5.0f)  
	{
		flight_arm_flag=1;
	}

	if(!(AP_Notify::flags.armed) && flight_arm_flag==1)
	{
		flight_counter++;
		flight_arm_flag=0;
	}

    /**
	 * Save the flight counter, set_and_save_ifchanged() will force save the parameter again and again on sdcard
	 * and will log number of instances it will change.
	 */
	copter.g.flight_seq_num.set_and_save_ifchanged(flight_counter);
}

/** 
 * @brief This function sends Sequence numbers as Mavlink packet
 * @param [mavlink_channel_t chan]
 * @details This function sends Sequence numbers as Mavlink packet by invoking mavlink_msg_sequence_num_send()
 * @return None
 */
void AP_Asteria::send_seq_numbers(mavlink_channel_t chan)
{
	mavlink_msg_sequence_num_send(chan,copter.g.disarm_seq_num.get(),copter.g.flight_seq_num.get());
}