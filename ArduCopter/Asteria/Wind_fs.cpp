/**
 * @file Wind_fs.cpp
 * This file implements Asteria High Wind Condition & Wind Failsafe
 */

#include "../Copter.h"

/** 
 * @brief This function updates the wind speed and direction
 * @param None
 * @details This function updates wind_speed and wind_direction
 * @return None
 */
void AP_Asteria::set_Wind_dir_Wind_Spd()
{
	///get the wind vector by invoking AP::ahrs().wind_estimate()
    const Vector3f wind = AP::ahrs().wind_estimate();

	///Update wind_speed and wind_direction from wind vector
    wind_speed = wind.length();
    wind_direction = degrees(atan2f(-wind.y, -wind.x));
}

/**
 * @brief This function performs High Wind Condition & Wind Failsafe
 * @param None
 * @details This function performs below,
 * 			- sets high_windspeed_flag to true, when AP is armed and wind_speed is more than 70% of 
 *            WIND_SPEED_MAX for more than 5 sec continuously
 *          - sets high_windspeed_flag to false, when wind_speed is less than 70% of WIND_SPEED_MAX
 * 			- sets failsafe.wind_fs and high_windspeed_flag with true, 
 *             when high_wind_failsafe() returns true
 * @return None
 */

void AP_Asteria::high_wind_flag_failsafe()
{
	static int count = 0;

	if(wind_speed>(copter.g.max_wind_speed*0.7f) && AP_Notify::flags.armed)
	{
		// runs at 10 Hz
		if (count > 50)
		{
			high_windspeed_flag = true;
			count = 0;
		}
		count++;

		if(high_wind_failsafe())
		{
			failsafe.wind_fs=true;
			high_windspeed_flag = true;
		}
		else
		{
			failsafe.wind_fs=false;
		}
	}
	else
	{
		high_windspeed_flag = false;
		count = 0;
	}
}

/**
 * @brief This function performs the wind fail safe
 * @param None
 * @details This function performs below,
 * 			- returns false if already in failsafe
 * 			- Change mode to RTL and returns true, when wind_speed is more than 
 *             WIND_SPEED_MAX for continuously 3sec
 *          - otherwise , return false
 * @return false/true
 */
bool AP_Asteria::high_wind_failsafe()
{
	/// retun false if already in failsafe
	if (copter.flightmode->mode_number() == Mode::Number::RTL)
	{
		return false;
	}

	static int count = 0;

	///If wind speed is more than WIND_SPEED_MAX continuously for
	/// more than 3 sec then hit failsafe and RTL, returns true
	if(wind_speed>copter.g.max_wind_speed)
	{
		//runs at 10Hz
		if (count > 30)
		{
			gcs().send_text(MAV_SEVERITY_CRITICAL,"WIND FAILSAFE | RTL ENGAGED");
			copter.set_mode(Mode::Number::RTL, ModeReason::WIND_FAILSAFE);
			count = 0;
			return true;
		}
		count++;
	}
	else
	{
		count = 0;
	}
	return false;
}

/**
 * @brief This function returns status of wind failsafe
 * @param None
 * @details This function return status of wind failsafe
 * @return failsafe.wind_fs
 */
bool AP_Asteria::have_wind_failsafe() const
{
    return failsafe.wind_fs;
}

/**
 * @brief This function logs the wind estimation data and high wind flag
 * @param None
 * @details This function writes the wind estimation(wind_speed,wind_direction 
 *          and high_windspeed_flag) onto logs 
 * @return None
 */
void AP_Asteria::log_wind_estimate_to_Dflash(void)
{
	struct log_wind_est pkt =
	{
		LOG_PACKET_HEADER_INIT(LOG_WIND_EST_MSG),
		wind_spd		   : wind_speed,
		wind_dir   	       : wind_direction,
		is_High_wind       : high_windspeed_flag,
	};
	//writing a packet of wind_estimate onto logs.
	copter.logger.WriteBlock(&pkt, sizeof(pkt));
}

/** 
 * @brief This function sends wind estimation as Mavlink packet
 * @param [mavlink_channel_t chan]
 * @details This function sends wind_speed,wind_direction and high_windspeed_flag
 *          as Mavlink packet by invoking mavlink_msg_wind_estimation_send()
 * @return None
 */
void AP_Asteria::send_wind_estimate_to_GCS(mavlink_channel_t chan)
{
	static int count=0;

	if(count>5)
	{
		mavlink_msg_wind_estimation_send(chan,wind_speed, wind_direction, high_windspeed_flag);
		count=0;
	}
	count++;

}