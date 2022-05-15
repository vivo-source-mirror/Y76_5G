/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef __IMGSENSOR_SENSOR_H__
#define __IMGSENSOR_SENSOR_H__

#include "kd_camera_feature.h"
#include "kd_imgsensor_define.h"
#include "imgsensor_i2c.h"

#include "eeprom_driver.h"  //vivo lishuo add for otp remake 20210823 [B210820-3030]


enum IMGSENSOR_STATE {
	IMGSENSOR_STATE_CLOSE,
	IMGSENSOR_STATE_OPEN
};

struct IMGSENSOR_SENSOR_STATUS {
	u32 reserved:28;
	u32 arch:4;
};

// Modify for CCM Hopping Start
enum STREAMING_STATE {
	STREAMING_OFF,
	STREAMING_ON
};
// Modify for CCM Hopping End

struct IMGSENSOR_SENSOR_INST {
	enum IMGSENSOR_STATE state;
	enum IMGSENSOR_SENSOR_IDX sensor_idx;
	struct IMGSENSOR_I2C_CFG i2c_cfg;
	struct IMGSENSOR_SENSOR_STATUS status;
	struct IMGSENSOR_SENSOR_LIST *psensor_list;
    
    //vivo lishuo add for otp remake 20210823 [B210820-3030] begin    
    struct IMGSENSOR_SENSOR_TABLEINFO *psensor_tableInfo; 
    struct EEPROM_DRV *pEEpInfo;
    //vivo lishuo add for otp remake 20210823 [B210820-3030] end

	struct mutex sensor_mutex;
	struct timeval profile_time;
	// Modify for CCM Hopping Start
	enum STREAMING_STATE streaming_state;
	// Modify for CCM Hopping End
};

struct IMGSENSOR_SENSOR {
	struct IMGSENSOR_SENSOR_INST  inst;
	struct SENSOR_FUNCTION_STRUCT *pfunc;
};

#endif

