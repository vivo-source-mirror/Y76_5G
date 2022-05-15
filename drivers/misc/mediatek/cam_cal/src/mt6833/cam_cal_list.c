/*
 * Copyright (C) 2018 MediaTek Inc.
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
#include <linux/kernel.h>
#include "cam_cal_list.h"
#include "eeprom_i2c_common_driver.h"
#include "eeprom_i2c_custom_driver.h"
#include "kd_imgsensor.h"

#define MAX_EEPROM_SIZE_16K 0x4000

struct stCAM_CAL_LIST_STRUCT g_camCalList[] = {
    /*Below is commom sensor */
#ifdef CONFIG_MTK_CAM_PD2156
	//add by vivo rcs  2021.8.5 start
    {S5KJN1SQ03PD2156_SENSOR_ID, 0xA0, Common_read_region},
    {HI846PD2156_SENSOR_ID, 0xA2, Common_read_region},
    {OV02B10PD2156_SENSOR_ID, 0xA4, Common_read_region},
	//add by vivo rcs  2021.8.5 end
#else
    {OV13B10_SENSOR_ID, 0xA4, Common_read_region},
    {HI846_SENSOR_ID, 0xA2, Common_read_region},
    {HI846PD2069_SENSOR_ID, 0xA2, Common_read_region},
    {HI846PD2069A_SENSOR_ID, 0xA2, Common_read_region},
    {S5KGM1ST_SENSOR_ID, 0xA0, Common_read_region},
    {S5KGM1STPD2069CF_SENSOR_ID, 0xA0, Common_read_region},
    {S5K3P9SP04_SENSOR_ID, 0xA2, Common_read_region},
    {S5K3P9SP04PD2069_SENSOR_ID, 0xA2, Common_read_region},
    {OV8856_SENSOR_ID, 0xA8, Common_read_region},
    {OV8856PD2102_SENSOR_ID, 0xA8, Common_read_region},
    {OV8856PD2069_SENSOR_ID, 0xA0, Common_read_region},
    {OV48B2Q_SENSOR_ID, 0xA0, Common_read_region},
    {OV02B10_SENSOR_ID, 0xA4, Common_read_region},
    {OV02B10PD2069_SENSOR_ID, 0xA4, Common_read_region},
    {IMX586_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
    {IMX576_SENSOR_ID, 0xA2, Common_read_region},
    {IMX519_SENSOR_ID, 0xA0, Common_read_region},
    {IMX319_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
    {S5K3M5SX_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
    {IMX686_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
    {HI846_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
    {S5KGD1SP_SENSOR_ID, 0xA8, Common_read_region, MAX_EEPROM_SIZE_16K},
    {S5K2T7SP_SENSOR_ID, 0xA4, Common_read_region},
    {IMX386_SENSOR_ID, 0xA0, Common_read_region},
    {S5K2L7_SENSOR_ID, 0xA0, Common_read_region},
    {IMX398_SENSOR_ID, 0xA0, Common_read_region},
    {IMX350_SENSOR_ID, 0xA0, Common_read_region},
    {IMX386_MONO_SENSOR_ID, 0xA0, Common_read_region},
    {IMX499_SENSOR_ID, 0xA0, Common_read_region},
	{OV64B40PD2069_SENSOR_ID, 0xA0, Common_read_region},
	{OV64B40V1PD2069_SENSOR_ID, 0xA0, Common_read_region},
	{S5K3P9SPPD2069_SENSOR_ID, 0xA2, Common_read_region},
    {OV64B40_SENSOR_ID, 0xA0, Common_read_region},
	{OV64B40PD2102_SENSOR_ID, 0xA0, Common_read_region},
    {OV32ALQ_SENSOR_ID, 0xA2, Common_read_region},
    {S5KJN1SQ03_SENSOR_ID, 0xA0, Common_read_region},
    /*  ADD before this line */
#endif
    {0, 0, 0} /*end of list */
};

unsigned int cam_cal_get_sensor_list(
	struct stCAM_CAL_LIST_STRUCT **ppCamcalList)
{
	if (ppCamcalList == NULL)
		return 1;

	*ppCamcalList = &g_camCalList[0];
	return 0;
}


