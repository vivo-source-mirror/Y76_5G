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
#ifdef CONFIG_MTK_CAM_PD2159F_EX
	//add by vivo chenjunliang  2021.8.24 start
	{S5KJN1SQ03PD2159_SENSOR_ID, 0xA0, Common_read_region},
	{S5KGH1SM24PD2159_SENSOR_ID, 0xA2, Common_read_region},
	{OV8856PD2159_SENSOR_ID, 0xA8, Common_read_region},
	{GC02M1PD2159_SENSOR_ID, 0xA4, Common_read_region},
	//add by vivo chenjunliang  2021.8.24 end
#elif defined(CONFIG_MTK_CAM_PD2164)
	{S5KJN1SQ03PD2164_SENSOR_ID, 0xA0, Common_read_region},
	{S5KJN1SQ03V1PD2164_SENSOR_ID, 0xA0, Common_read_region},
	{OV50C40PD2164_SENSOR_ID, 0xA0, Common_read_region},
	{S5K4H7YXPD2164_SENSOR_ID, 0xA2, Common_read_region},
	{S5K4H7YXV1PD2164_SENSOR_ID, 0xA2, Common_read_region},
	{GC02M1PD2164_SENSOR_ID, 0xA4, Common_read_region},
#else
	{S5KJN1SQ03PD2156F_SENSOR_ID, 0xA0, Common_read_region},
	{S5K3P9SPPD2156F_SENSOR_ID, 0xA2, Common_read_region},
	{GC02M1PD2156F_SENSOR_ID, 0xA4, Common_read_region},
	{GC02M1BPD2156F_SENSOR_ID, 0xA4, Common_read_region},
//	{OV13B10_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K}, //remove by changcheng@vivo.com
	{IMX355_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV02B10_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K},
	/*Below is commom sensor */
	{OV13B10_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K}, //add by changcheng@vivo.com
	{OV13B10PD2069CF_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K},
	{HI846_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{HI846PD2069_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{HI846PD2069A_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5KGM1ST_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5KGM1STPD2069CF_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
   	{S5K3P9SP04_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV8856_SENSOR_ID, 0xA8, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV8856PD2069_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5KGW3SP13PD2069_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV48B2Q_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV64B40_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV64B40PD2069_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV64B40V1PD2069_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV64B40PD2102_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV02B10PD2069_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K},
	{OV02B10PD2069V1_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX586_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K,
		BL24SA64_write_region},
	{IMX576_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX519_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX319_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5K3M5SX_SENSOR_ID, 0xA2, Common_read_region, MAX_EEPROM_SIZE_16K,
		BL24SA64_write_region},
	{IMX686_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{HI846_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5KGD1SP_SENSOR_ID, 0xA8, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5K2T7SP_SENSOR_ID, 0xA4, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX386_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{S5K2L7_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX398_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX350_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX386_MONO_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX499_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
	{IMX481_SENSOR_ID, 0xA4, Common_read_region, DEFAULT_MAX_EEPROM_SIZE_8K,
		BL24SA64_write_region},
       {S5KJN1SQ03_SENSOR_ID, 0xA0, Common_read_region, MAX_EEPROM_SIZE_16K},
#endif
	/*  ADD before this line */
	{0, 0, 0}       /*end of list */
};

unsigned int cam_cal_get_sensor_list(
	struct stCAM_CAL_LIST_STRUCT **ppCamcalList)
{
	if (ppCamcalList == NULL)
		return 1;

	*ppCamcalList = &g_camCalList[0];
	return 0;
}


