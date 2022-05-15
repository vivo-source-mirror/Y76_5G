/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * OVT setting release hitory: 
 * 20200602:   Setting is based on OV64C AM07 released.
 * 20200610:   MirrorFlip enable.
 * 2020/06/17: PDAF PD location
 * 2020/06/22: setting update to AM08 
 * 2020/07/09?��oadd 4032x3024 res
 * 2020/09/16: update latest setting. 
 * 2021/6/1:  ov64b40 mipi cphy -> dphy update pre and full
 * 2021/6/3:  ov64b40 mipi cphy -> dphy update video slim hs custom1 custom2
 * 2021/6/10:  ov64b40 mipi cphy -> dphy all mode update OV Ameracan engineer
 * ***************************************************************************
 *
 * Filename:
 * ---------
 *	 ov50c40pd2164mipiraw_Sensor.c
 *
 * Project:
 * --------
 *	 ALPS
 *
 * Description:
 * ------------
 *	 Source code of Sensor driver
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/


#define PFX "ov50c40pd2164_camera_sensor"
//#define pr_fmt(format, args...) PFX "[%s] " format, __func__, ##args


#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>

#include "kd_camera_typedef.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "../imgsensor_i2c.h"
#include "ov50c40pd2164mipiraw_Sensor.h"




 #define pr_debinfo(format, args...) pr_info(PFX "[%s] " format, __func__, ##args)


static DEFINE_SPINLOCK(imgsensor_drv_lock);
//static bool bIsLongExposure = KAL_FALSE;

static struct imgsensor_info_struct imgsensor_info = {
	.sensor_id = OV50C40PD2164_SENSOR_ID,
	.checksum_value = 0x67b95889,

	.pre = {
		.pclk =99930000,
		.linelength = 1050,
		.framelength = 3174,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 4080,
		.grabwindow_height = 3060,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 300,//30.06
	},
	.cap = {
		.pclk =99930000,
		.linelength = 1050,
		.framelength = 3174,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 4080,
		.grabwindow_height = 3060,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 300,//30.06
	},
	.normal_video = {
		.pclk =99930000,
		.linelength = 1050,
		.framelength = 3174,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 4080,
		.grabwindow_height = 2296,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 300,//30.06
	},
	.hs_video = { /*hs_video 120fps*/
		.pclk =99930000,
		.linelength = 456,
		.framelength = 1110,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 1280,
		.grabwindow_height = 720,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 1200,//30.06
	},

	.slim_video = { /* hs_video 120fps*/
		.pclk =99930000,
		.linelength = 456,
		.framelength = 1110,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 1280,
		.grabwindow_height = 720,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 1200,//30.06
	},

	.custom1 = {
		.pclk =99930000,
		.linelength = 1050,
		.framelength = 3174,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 4080,
		.grabwindow_height = 3060,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 300,//30.06
	},
	.custom2 = { /* 3840*2160@60fps */
		.pclk =99930000,
		.linelength = 500,
		.framelength = 3333,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 3840,
		.grabwindow_height = 2160,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 873600000,/*511200000*/
		.max_framerate = 600,//60.01
		},
  .custom3 = { /*sw remosiac */
		.pclk =99930000,
		.linelength = 2550,
		.framelength = 3918,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 8160,
		.grabwindow_height = 6120,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 962000000,
		.max_framerate = 100,//10.08
	},  
	.custom4 = { /* 3840*2160@30fps */
		.pclk =99930000,
		.linelength = 500,
		.framelength = 3333,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 3840,
		.grabwindow_height = 2160,
		.mipi_data_lp2hs_settle_dc = 85,
		.mipi_pixel_rate = 873600000,/*511200000*/
		.max_framerate = 600,//60.01
		},
	.custom5 = {
		.pclk =99930000,
		.linelength = 1050,
		.framelength = 3174,
		.startx =0,
		.starty = 0,
		.grabwindow_width = 4080,
		.grabwindow_height = 3060,
		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.mipi_pixel_rate = 563200000,
		.max_framerate = 300,//30.06
	},

   
		.margin = 22,
		.min_shutter = 8,
		.min_gain = 64,
		.max_gain = 992,
		.min_gain_iso = 100,
		.gain_step = 1,
		.gain_type = 1,
		.max_frame_length = 0x3fffff, // Long exposure, limited to 32s.
		.ae_shut_delay_frame = 0,
		.ae_sensor_gain_delay_frame = 0,
		.ae_ispGain_delay_frame = 2,
		.frame_time_delay_frame = 2,	/* The delay frame of setting frame length  */
		.ihdr_support = 0,	  /*1, support; 0,not support*/
		.ihdr_le_firstline = 0,  /*1,le first ; 0, se first*/
		.sensor_mode_num = 9,	  /*support sensor mode num*/

		.cap_delay_frame = 2,/*3 guanjd modify for cts*/
		.pre_delay_frame = 2,/*3 guanjd modify for cts*/
		.video_delay_frame = 3,
		.hs_video_delay_frame = 2,
		.slim_video_delay_frame = 2,
		.custom1_delay_frame = 2,
		.custom2_delay_frame = 2,
		.custom3_delay_frame = 2,
        .custom4_delay_frame = 2,
		.custom5_delay_frame = 2,
		.isp_driving_current = ISP_DRIVING_2MA,
		.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
		.mipi_sensor_type = MIPI_OPHY_NCSI2, /*0,MIPI_OPHY_NCSI2;  1,MIPI_OPHY_CSI2*/
		.mipi_settle_delay_mode = 0, /*0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANNUAL*/
		//.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,SENSOR_OUTPUT_FORMAT_RAW_4CELL_HW_BAYER_B,
		.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_Gr,

        .mclk = 26, /* 6MHz mclk for 26fps */
	    .mipi_lane_num = SENSOR_MIPI_4_LANE,


		.i2c_addr_table = {0x6c, 0xff},
		.i2c_speed = 1000,
};


static struct imgsensor_struct imgsensor = {
	.mirror = IMAGE_HV_MIRROR,				//mirrorflip information
	.sensor_mode = IMGSENSOR_MODE_INIT, /*IMGSENSOR_MODE enum value,record current sensor mode,such as: INIT, Preview, Capture, Video,High Speed Video, Slim Video*/
	.shutter = 0xc00,					/*current shutter*/
	.gain = 0x100,						/*current gain*/
	.dummy_pixel = 0,					/*current dummypixel*/
	.dummy_line = 0,					/*current dummyline*/
	.current_fps = 300,  /*full size current fps : 24fps for PIP, 30fps for Normal or ZSD*/
	.autoflicker_en = KAL_FALSE,  /*auto flicker enable: KAL_FALSE for disable auto flicker, KAL_TRUE for enable auto flicker*/
	.test_pattern = KAL_FALSE,		/*test pattern mode or not. KAL_FALSE for in test pattern mode, KAL_TRUE for normal output*/
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,/*current scenario id*/
	.ihdr_mode = 0, /*sensor need support LE, SE with HDR feature*/
	.i2c_write_id = 0x6c,
	.current_ae_effective_frame = 2,
};

static struct  SENSOR_RAWINFO_STRUCT imgsensor_raw_info = {
	 4080,//raw_weight 
 	 3060,//raw_height
	 2,//raw_dataBit
	 BAYER_GBRG,//raw_colorFilterValue
	 64,//raw_blackLevel
	 78.0,//raw_viewAngle
	 10,//raw_bitWidth
	 64//raw_maxSensorGain
};

/* Sensor output window information */
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[10] = {  
	{8160, 6120,	0,  0, 8160, 6120, 4080, 3060, 0, 0, 4080, 3060, 0, 0, 4080, 3060}, /*Preview*/
	{8160, 6120,    0,  0, 8160, 6120, 4080, 3060, 0, 0, 4080, 3060, 0, 0, 4080, 3060}, /* capture */
	{8160, 6120,    0,  764, 8160, 4592, 4080, 2296, 0, 0, 4080, 2296, 0, 0, 4080, 2296}, /* normal video */
	{8160, 6120,  1520,  1620, 5120, 2880, 1280,  720, 0, 0, 1280,  720, 0, 0, 1280,  720}, /* hs_video */
	{8160, 6120,  1520,  1620, 5120, 2880, 1280,  720, 0, 0, 1280,  720, 0, 0, 1280,  720}, /* slim video */
	{8160, 6120,	0,  0, 8160, 6120, 4080, 3060, 0, 0, 4080, 3060, 0, 0, 4080, 3060}, /* custom1 */
	{8160, 6120,  240,  900, 7680, 4320, 3840, 2160, 0, 0, 3840, 2160, 0, 0, 3840, 2160}, /* custom2 */	
	{8160, 6120,    0,    0, 8160, 6120, 8160, 6120, 0, 0, 8160, 6120, 0, 0, 8160, 6120}, /* sw remosiac*/
	{8160, 6120,  240,  900, 7680, 4320, 3840, 2160, 0, 0, 3840, 2160, 0, 0, 3840, 2160}, /*custom4*/
	{8160, 6120,	0,  0, 8160, 6120, 4080, 3060, 0, 0, 4080, 3060, 0, 0, 4080, 3060}, /*custom5*/
};


/*no mirror flip*/

static struct SET_PD_BLOCK_INFO_T imgsensor_pd_info = 
{
    .i4OffsetX = 16,
    .i4OffsetY = 16,
    .i4PitchX = 16,
    .i4PitchY = 16,
    .i4PairNum = 8,
    .i4SubBlkW = 8,
    .i4SubBlkH = 4,
    .i4PosL = {{23,18},{31,18},{19,22},{27,22},{23,26},{31,26},{19,30},{27,30}},
    .i4PosR = {{22,18},{30,18},{18,22},{26,22},{22,26},{30,26},{18,30},{26,30}},
    .i4BlockNumX = 254,
    .i4BlockNumY = 190,
    .i4LeFirst = 0,
    .i4Crop = {
        {8, 6}, {8, 6}, {8, 388}, {0, 0}, {0, 0},
        {8, 6}, {0, 0}, {0, 0}, {0, 0}, {8, 6}
    },
	.iMirrorFlip = 3,
};



/*VC1 None , VC2 for PDAF(DT=0X30), unit : 10bit*/
static struct SENSOR_VC_INFO_STRUCT SENSOR_VC_INFO[2] = {
	/* Preview mode setting */ //4080*3060
	{
		0x03, 0x0a, 0x00, 0x08, 0x40, 0x00,
		0x00, 0x2b, 0xff0, 0x0bf4, 0x00, 0x00, 0x0280, 0x0001,
		0x01, 0x2b, 0x03f8, 0x02f8, 0x03, 0x00, 0x0000, 0x0000
	},

	/* Video mode setting */ //4080x2296
	{
		0x03, 0x0a, 0x00, 0x08, 0x40, 0x00,
		0x00, 0x2b, 0xff0, 0x8f8, 0x00, 0x00, 0x0280, 0x0001,
		0x01, 0x2b, 0x03f8, 0x023e, 0x03, 0x00, 0x0000, 0x0000
	},


};

/*hope add otp check start*/
static int vivo_otp_read_when_power_on;
extern int ov50c40pd2164_otp_read(void);
extern otp_error_code_t OV50C40PD2164_OTP_ERROR_CODE;
MUINT32  af_calib_inf_main_ov50c40pd2164[6];
MUINT32  sn_inf_main_ov50c40pd2164[13];  /*0 flag   1-12 data*/
//MUINT32  crosstalk_main_ov50c40pd2164[299]; 
MUINT32  material_inf_main_ov50c40pd2164[4];  
/*hope add otp check end*/
//#define READ_SHUTTER_FRANELENGTH_GAIN/*zjj add for check shutter and gain -2021/10/21*/


static kal_uint16 read_cmos_sensor_16_8(kal_uint16 addr)
{
	kal_uint16 get_byte= 0;
	char pusendcmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	/*kdSetI2CSpeed(imgsensor_info.i2c_speed);  Add this func to set i2c speed by each sensor*/
	iReadRegI2C(pusendcmd , 2, (u8*)&get_byte,1,imgsensor.i2c_write_id);
	return get_byte;
}

static void write_cmos_sensor_16_8(kal_uint16 addr, kal_uint8 para)
{
	char pusendcmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para & 0xFF)};
	 /* kdSetI2CSpeed(imgsensor_info.i2c_speed);Add this func to set i2c speed by each sensor*/
	iWriteRegI2C(pusendcmd , 3, imgsensor.i2c_write_id);
}

#define MULTI_WRITE 1

#if MULTI_WRITE
#define I2C_BUFFER_LEN 765/* trans# max is 255, each 3 bytes */
#else
#define I2C_BUFFER_LEN 3
#endif

static kal_uint16 table_write_cmos_sensor(kal_uint16 *para, kal_uint32 len)
{
	char puSendCmd[I2C_BUFFER_LEN];
	kal_uint32 tosend, IDX;
	kal_uint16 addr = 0, addr_last = 0, data;

	tosend = 0;
	IDX = 0;

	while (len > IDX) {
		addr = para[IDX];
		{
			puSendCmd[tosend++] = (char)(addr >> 8);
			puSendCmd[tosend++] = (char)(addr & 0xFF);
			data = para[IDX + 1];
			puSendCmd[tosend++] = (char)(data & 0xFF);
			IDX += 2;
			addr_last = addr;

		}

#if MULTI_WRITE
	if ((I2C_BUFFER_LEN - tosend) < 3 || IDX == len || addr != addr_last) {
		iBurstWriteReg_multi(puSendCmd, tosend,
			imgsensor.i2c_write_id, 3, imgsensor_info.i2c_speed);
			tosend = 0;
	}
#else
		iWriteRegI2C(puSendCmd, 3, imgsensor.i2c_write_id);
		tosend = 0;
#endif

	}
	return 0;
}



static void set_dummy(void)
{

	pr_debinfo("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);
	  if (imgsensor.shutter > imgsensor.min_frame_length - imgsensor_info.margin)
        imgsensor.frame_length = imgsensor.shutter + imgsensor_info.margin;	 
    write_cmos_sensor_16_8(0x380c, imgsensor.line_length >> 8);
    write_cmos_sensor_16_8(0x380d, imgsensor.line_length & 0xFF);
	write_cmos_sensor_16_8(0x3840, imgsensor.frame_length >> 16);
    write_cmos_sensor_16_8(0x380e, (imgsensor.frame_length >> 8)& 0xFF);
    write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFE);	 

}	/*	set_dummy  */


static void set_max_framerate(UINT16 framerate,kal_bool min_framelength_en)
{

	kal_uint32 frame_length = imgsensor.frame_length;

	pr_debinfo("framerate = %d, min framelength should enable %d \n", framerate,min_framelength_en);

	frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
	spin_lock(&imgsensor_drv_lock);
	if (frame_length >= imgsensor.min_frame_length)
		imgsensor.frame_length = frame_length;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;

	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
	{
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;
	spin_unlock(&imgsensor_drv_lock);
	set_dummy();
}	/*	set_max_framerate  */

static kal_uint32 streaming_control(kal_bool enable)
{

	pr_debinfo("streaming_enable(0= Sw Standby,1= streaming): %d\n", enable);
	if (enable) {
		write_cmos_sensor_16_8(0x0100, 0X01);
		mdelay(15);
	} else {
		write_cmos_sensor_16_8(0x0100, 0x00);
		mdelay(10);
		}

	return ERROR_NONE;
}


static void write_shutter(kal_uint32 shutter)
{
    //check
#ifdef READ_SHUTTER_FRANELENGTH_GAIN
	kal_uint16  a_0x3500 =0;
	kal_uint16  b_0x3501 =0;
	kal_uint16  c_0x3502 =0;
	kal_uint16  d_0x3822 =0;
	kal_uint16  e_0x380e =0;
	kal_uint16  f_0x380f =0;
#endif

	kal_uint16 realtime_fps = 0;
	//int framecnt = 0;
	pr_debinfo("write_shutter\n");
			
	spin_lock(&imgsensor_drv_lock);
	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);
	
	if (shutter < imgsensor_info.min_shutter) 
		shutter = imgsensor_info.min_shutter;
    //frame_length and shutter should be an even number.
    shutter = (shutter >> 1) << 1;

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if(realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296,0);
		else if(realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146,0);
		else {
		/* Extend frame length*/
		/*
		  	imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;
			write_cmos_sensor_16_8(0x3840, imgsensor.frame_length >> 16);
			write_cmos_sensor_16_8(0x380e, (imgsensor.frame_length >> 8)& 0xFF);
			write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFF);
		*/
	    }
	} else {
		/* Extend frame length*/
		/*
		  	imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;
			write_cmos_sensor_16_8(0x3840, imgsensor.frame_length >> 16);
			write_cmos_sensor_16_8(0x380e, (imgsensor.frame_length >> 8)& 0xFF);
			write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFF);
		*/
	}
	/* Update frame length & Shutter*/
	
	imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;
	/* ###groupon### */
	write_cmos_sensor_16_8(0x3208, 0x00); 
	/* ###groupon### */


	/* ###shutter start### */
	write_cmos_sensor_16_8(0x3500,(shutter >> 16)); 
	write_cmos_sensor_16_8(0x3501,(shutter >> 8)&0xFF); 
	write_cmos_sensor_16_8(0x3502,(shutter & 0xFE));
	/* ###shutter end### */

	/* ###framelength start### */
	write_cmos_sensor_16_8(0x3840, imgsensor.frame_length >> 16);
	write_cmos_sensor_16_8(0x380e, (imgsensor.frame_length >> 8)& 0xFF);
	write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFE);
	/* ###framelength end### */
	


	/* ###groupoff### */
	write_cmos_sensor_16_8(0x3208, 0x10);
	write_cmos_sensor_16_8(0x3208, 0xa0);
	/* ###groupoff### */
	
	pr_debinfo("shutter = %d, framelength = %d\n", shutter,imgsensor.frame_length);
#ifdef READ_SHUTTER_FRANELENGTH_GAIN
	a_0x3500 =	read_cmos_sensor_16_8(0x3500);
	b_0x3501 =	read_cmos_sensor_16_8(0x3501);
	c_0x3502 =	read_cmos_sensor_16_8(0x3502);
	d_0x3822 =	read_cmos_sensor_16_8(0x3822);
	e_0x380e =	read_cmos_sensor_16_8(0x380e);
	f_0x380f =	read_cmos_sensor_16_8(0x380f);
    	pr_info(" zjjread  a_0x3500=%d  b_0x3501=%d c_0x3502 =%d shutter=%d  0x%x\n",a_0x3500,b_0x3501,c_0x3502,((a_0x3500<<16)|(b_0x3501<<8)|c_0x3502),((a_0x3500<<16)|(b_0x3501<<8)|c_0x3502));
	pr_info(" zjjread  d_0x3822=%d  ", d_0x3822);
	pr_info(" zjjread  frame_length =%d e_0x380e=%d  e_0x380f=%d", (e_0x380e<<8|f_0x380f),e_0x380e,f_0x380f);
#endif
}



/*************************************************************************
* FUNCTION
*	set_shutter
*
* DESCRIPTION
*	This function set e-shutter of sensor to change exposure time.
*
* PARAMETERS
*	iShutter : exposured lines
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void set_shutter(kal_uint32 shutter)
{
	unsigned long flags;
	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	write_shutter(shutter);
}	/*	set_shutter */

/*	write_shutter  */
static void set_shutter_frame_length(kal_uint16 shutter, kal_uint16 frame_length)
{
	unsigned long flags;
	kal_uint16 realtime_fps = 0;
	kal_int32 dummy_line = 0;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	spin_lock(&imgsensor_drv_lock);
	/* Change frame time */
	if (frame_length > 1)
		dummy_line = frame_length - imgsensor.frame_length;
	imgsensor.frame_length = imgsensor.frame_length + dummy_line;

	/*  */
	if (shutter > imgsensor.frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;

	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;

	spin_unlock(&imgsensor_drv_lock);
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	shutter = (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin))
		? (imgsensor_info.max_frame_length - imgsensor_info.margin) : shutter;

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if (realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296, 0);
		else if (realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146, 0);
		else {
			/* Extend frame length */
			/*write_cmos_sensor_16_8(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFF);*/
		}
	} else {
			/* Extend frame length */
			/*write_cmos_sensor_16_8(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFF);*/
	}

	/* Update Shutter */
	//frame_length and shutter should be an even number.
    shutter = (shutter >> 1) << 1;
	imgsensor.frame_length = (imgsensor.frame_length  >> 1) << 1;
	write_cmos_sensor_16_8(0x3208, 0x00);
	write_cmos_sensor_16_8(0x3840, imgsensor.frame_length >> 16);
	write_cmos_sensor_16_8(0x380e, (imgsensor.frame_length >> 8)& 0xFF);
	write_cmos_sensor_16_8(0x380f, imgsensor.frame_length & 0xFE);
	write_cmos_sensor_16_8(0x3500,(shutter >> 16)); 
	write_cmos_sensor_16_8(0x3501,(shutter >> 8)&0xFF); 
	write_cmos_sensor_16_8(0x3502,(shutter & 0xFE));
	write_cmos_sensor_16_8(0x3208, 0x10);
	write_cmos_sensor_16_8(0x3208, 0xa0);	

	pr_debinfo("shutter = %d, framelength = %d/%d, dummy_line= %d\n", shutter, imgsensor.frame_length,
		frame_length, dummy_line);

}				/*      write_shutter  */


static kal_uint16 gain2reg(const kal_uint16 gain)
{
	kal_uint16 reg_gain = 0x0000;
	
	//platform 1xgain = 64, sensor driver 1*gain = 0x100
	reg_gain = gain*256/BASEGAIN;

	if(reg_gain < 0x100)// sensor 1xGain
	{
		reg_gain = 0X100;
	}
	if(reg_gain > 0xf80)// sensor 15.5xGain
	{
		reg_gain = 0Xf80;
	}

	 return (kal_uint16)reg_gain;	
}

/*************************************************************************
* FUNCTION
*	set_gain
*
* DESCRIPTION
*	This function is to set global gain to sensor.
*
* PARAMETERS
*	iGain : sensor global gain(base: 0x40)
*
* RETURNS
*	the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint16 set_gain(kal_uint16 gain)
{
	kal_uint16 reg_gain;
#ifdef READ_SHUTTER_FRANELENGTH_GAIN
	kal_uint16 reg_gain_3508;
	kal_uint16 reg_gain_3509;
#endif
	/*gain= 1024;for test*/
	/*return; for test*/

	if (gain < BASEGAIN || gain > (15* BASEGAIN + BASEGAIN/2)) {
	    printk("Error gain setting");

	    if (gain < BASEGAIN)
	        gain = BASEGAIN;
	    else if (gain > (15* BASEGAIN + BASEGAIN/2))
	        gain = (15* BASEGAIN + BASEGAIN/2);
	}

	reg_gain = gain2reg(gain);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.gain = reg_gain;
	spin_unlock(&imgsensor_drv_lock);
	printk("zjjread gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);

	write_cmos_sensor_16_8(0x3508,(reg_gain>>8));
	write_cmos_sensor_16_8(0x3509,(reg_gain&0xff));

#ifdef READ_SHUTTER_FRANELENGTH_GAIN
	reg_gain_3508 =read_cmos_sensor_16_8(0x3508);
	reg_gain_3509 =read_cmos_sensor_16_8(0x3509);
    pr_info(" zjjread  reg_gain_3508 = %d , reg_gain_3509 = %d reg_gain = 0x%x\n",reg_gain_3508, reg_gain_3509,(reg_gain_3508<<8|reg_gain_3509));
#endif
	return gain;
}	/*	set_gain  */

static void set_mirror_flip(kal_uint8 image_mirror)
{
	switch (image_mirror) {

	    case IMAGE_NORMAL:
	       
	        break;

	    case IMAGE_H_MIRROR:
	     
	        break;

	    case IMAGE_V_MIRROR:       
	        break;

	    case IMAGE_HV_MIRROR:

	        break;
	    default:
	    printk("Error image_mirror setting\n");
	}
}

/*************************************************************************
* FUNCTION
*	night_mode
*
* DESCRIPTION
*	This function night mode of sensor.
*
* PARAMETERS
*	bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if 0
static void night_mode(kal_bool enable)
{
/*No Need to implement this function*/
}	/*	night_mode	*/
#endif



static kal_uint16 addr_data_pair_init[] = {
0x0103,0x01,
0x0102,0x01,
0x0301,0xc8,
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0306,0x04,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0326,0xcb,
0x0327,0x05,
0x0328,0x07,
0x032a,0x0a,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x0350,0x00,
0x0360,0x09,
0x3002,0x00,
0x3009,0x04,
0x3012,0x41,
0x3019,0xc2,
0x301c,0x81,
0x3025,0x03,
0x3026,0x10,
0x3027,0x00,
0x3107,0x48,
0x3400,0x0c,
0x3409,0x02,
0x340c,0x10,
0x340d,0x00,
0x3420,0x00,
0x3421,0x08,
0x3423,0x15,
0x3424,0x40,
0x3425,0x10,
0x3426,0x40,
0x3500,0x00,
0x3501,0x0c,
0x3502,0x16,
0x3504,0x28,
0x3507,0x00,
0x3508,0x01,
0x3509,0x00,
0x350a,0x01,
0x350b,0x00,
0x350c,0x00,
0x350e,0x00,
0x3540,0x00,
0x3541,0x00,
0x3542,0x00,
0x3548,0x01,
0x3549,0x00,
0x354a,0x01,
0x354b,0x00,
0x354c,0x00,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3618,0x80,
0x361b,0x80,
0x3622,0x88,
0x3623,0x33,
0x3624,0x95,
0x3627,0xcc,
0x3628,0xaa,
0x362b,0x08,
0x362d,0x08,
0x363b,0x80,
0x363d,0x0c,
0x3680,0xc0,
0x3684,0x03,
0x368d,0x00,
0x368e,0x01,
0x3690,0x10,
0x3697,0x80,
0x3699,0x1f,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x369e,0x00,
0x36a1,0x01,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x371c,0x04,
0x371d,0x1e,
0x371e,0x13,
0x371f,0x0a,
0x3720,0x08,
0x3721,0x15,
0x3725,0x32,
0x3727,0x22,
0x3729,0x01,
0x3731,0x05,
0x3736,0x02,
0x3737,0x05,
0x3738,0x02,
0x3739,0x05,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x3760,0x08,
0x3761,0x10,
0x3762,0x08,
0x3763,0x08,
0x3764,0x08,
0x3765,0x10,
0x3766,0x18,
0x3767,0x28,
0x3768,0x00,
0x3769,0x08,
0x376c,0x00,
0x376f,0x02,
0x3770,0x3b,
0x379e,0x3b,
0x379f,0x3b,
0x37b0,0x37,
0x37b1,0x37,
0x37b2,0x37,
0x37b3,0x3b,
0x37b4,0x3a,
0x37b5,0x3a,
0x37ce,0x02,
0x37cf,0x05,
0x37d6,0x00,
0x37d9,0x00,
0x37dc,0x43,
0x37ed,0x02,
0x37ee,0x05,
0x37f0,0x02,
0x37f1,0x05,
0x37f2,0x02,
0x37f3,0x05,
0x37f4,0x00,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x0b,
0x380b,0xf4,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x00,
0x3813,0x0a,
0x3814,0x11,
0x3815,0x11,
0x381a,0x00,
0x381b,0x00,
0x381e,0x00,
0x381f,0x00,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x3824,0x00,
0x3825,0x00,
0x3826,0x00,
0x3827,0x00,
0x3828,0x07,
0x382a,0xc1,
0x382c,0x00,
0x382d,0x00,
0x3835,0x00,
0x3836,0x00,
0x383c,0x00,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3847,0x00,
0x3848,0x00,
0x3849,0x00,
0x384b,0x8e,
0x384f,0x00,
0x3856,0x10,
0x3857,0x0a,
0x3858,0x20,
0x3859,0x14,
0x3865,0x00,
0x3869,0x00,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x3906,0x00,
0x3909,0x00,
0x390c,0x00,
0x390f,0x00,
0x3912,0x00,
0x3915,0x04,
0x3918,0x04,
0x391b,0x00,
0x391d,0x02,
0x391e,0x00,
0x3924,0x02,
0x3927,0x02,
0x392a,0x02,
0x392d,0x02,
0x3930,0x02,
0x3933,0x02,
0x3936,0x02,
0x3939,0x02,
0x393c,0x02,
0x393f,0x02,
0x3942,0x02,
0x3945,0x02,
0x3980,0x37,
0x3981,0x3a,
0x3982,0x0c,
0x3983,0x3b,
0x3990,0x01,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39b1,0x05,
0x39b3,0x78,
0x39b6,0x40,
0x39b7,0x10,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x39c9,0x01,
0x39cf,0x00,
0x39d2,0x00,
0x3a01,0x1e,
0x3a12,0x00,
0x3a13,0x00,
0x3a14,0x00,
0x3a18,0x04,
0x3a36,0x20,
0x3d85,0x0b,
0x3d86,0x00,
0x3d87,0x12,
0x3d8c,0x73,
0x3d8d,0xd8,
0x3daa,0x00,
0x3dab,0x00,
0x3dac,0x00,
0x3dad,0x00,
0x3dae,0x00,
0x3daf,0x00,
0x3f01,0x13,
0x3f9e,0x03,
0x3f9f,0x04,
0x4009,0x01,
0x4010,0x38,
0x4011,0x01,
0x4012,0x0d,
0x4015,0x00,
0x4016,0x0f,
0x4017,0x00,
0x4018,0x03,
0x401a,0x40,
0x401b,0x04,
0x401e,0x00,
0x401f,0xd0,
0x4020,0x04,
0x4021,0x00,
0x4022,0x04,
0x4023,0x00,
0x4024,0x04,
0x4025,0x00,
0x4026,0x04,
0x4027,0x00,
0x4030,0x00,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4056,0x25,
0x4100,0x00,
0x4103,0x00,
0x4104,0x00,
0x4300,0x00,
0x4301,0x00,
0x4302,0x00,
0x4303,0x00,
0x4304,0x00,
0x4305,0x00,
0x4306,0x00,
0x4307,0x00,
0x4308,0x00,
0x430b,0xff,
0x430c,0xff,
0x430d,0x00,
0x430e,0x00,
0x4500,0x04,
0x4503,0x0f,
0x4507,0x00,
0x4508,0x00,
0x4510,0x07,
0x4512,0x00,
0x4513,0x00,
0x4514,0x00,
0x4515,0x00,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x460b,0x01,
0x4640,0x00,
0x4641,0x7f,
0x4642,0x42,
0x4643,0x0c,
0x4648,0x0a,
0x4649,0x05,
0x4700,0x0a,
0x4701,0x0c,
0x4702,0x0a,
0x4703,0x0a,
0x4704,0x12,
0x4705,0x0a,
0x4706,0x0a,
0x4707,0x0a,
0x4708,0x0a,
0x4709,0x12,
0x470a,0x0a,
0x470b,0x0a,
0x470c,0x0a,
0x470d,0x0a,
0x470e,0x12,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x4742,0x00,
0x4743,0x0f,
0x4744,0x55,
0x4745,0xff,
0x4747,0x00,
0x474e,0x80,
0x4750,0x01,
0x4753,0x80,
0x4755,0x01,
0x4757,0x0b,
0x4759,0x0b,
0x475b,0x06,
0x475d,0x03,
0x475f,0x02,
0x4761,0x07,
0x4763,0x07,
0x4765,0x04,
0x4767,0x02,
0x4769,0x01,
0x4800,0x04,
0x4802,0x00,
0x480b,0x10,
0x480c,0x80,
0x480e,0x04,
0x480f,0x32,
0x4815,0x19,
0x481b,0x3c,
0x481f,0x32,
0x4837,0x0b,
0x484b,0x27,
0x4850,0x43,
0x4851,0xaa,
0x4853,0x4a,
0x4854,0x06,
0x4860,0x00,
0x4861,0xec,
0x4862,0x04,
0x4883,0x00,
0x4888,0x10,
0x4d00,0x05,
0x4d01,0x0e,
0x4d02,0xb7,
0x4d03,0x39,
0x4d04,0xcd,
0x4d05,0x4e,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x504c,0x12,
0x504d,0x12,
0x504e,0x12,
0x504f,0x12,
0x5050,0x12,
0x5051,0x14,
0x5055,0x12,
0x5081,0x00,
0x5180,0x70,
0x5181,0x10,
0x5182,0x00,
0x5183,0x7f,
0x5184,0x40,
0x5185,0x2b,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x00,
0x5195,0x04,
0x5880,0xc1,
0x588a,0x04,
0x5c00,0x63,
0x5c01,0x05,
0x5c02,0x00,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c0e,0x01,
0x5c0f,0x00,
0x5c10,0x01,
0x5c11,0x00,
0x5c12,0x01,
0x5c13,0x00,
0x5c14,0x01,
0x5c15,0x00,
0x5c16,0x00,
0x5c17,0x50,
0x5c18,0x00,
0x5c19,0x50,
0x5c1a,0x00,
0x5c1b,0x50,
0x5c1c,0x00,
0x5c1d,0x50,
0x5c1e,0x00,
0x5c1f,0x50,
0x5c20,0x00,
0x5c21,0x50,
0x5c22,0x00,
0x5c23,0x50,
0x5c24,0x00,
0x5c25,0x50,
0x5c26,0x00,
0x5c27,0x50,
0x5c28,0x00,
0x5c29,0x50,
0x5c2a,0x00,
0x5c2b,0x50,
0x5c2c,0x00,
0x5c2d,0x50,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5c5f,0x00,
0x5c60,0x00,
0x5c61,0x30,
0x5c62,0x00,
0x5c63,0x30,
0x5c64,0x1f,
0x5c65,0xc0,
0x5c66,0x17,
0x5c67,0xc0,
0x5c68,0x02,
0x5c69,0x02,
0x5c6a,0x02,
0x5c6b,0x02,
0x5c90,0x01,
0x5d00,0xc2,
0x5d01,0x68,
0x5d02,0xff,
0x5d03,0x1c,
0x5d05,0x02,
0x5d06,0x04,
0x5d07,0x11,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,
0x5d12,0x00,
0x5d13,0x00,
0x5d14,0xff,
0x5d15,0x10,
0x5d16,0x10,
0x5d17,0x10,
0x5d18,0x10,
0x5d19,0xff,
0x5d1a,0x10,
0x5d1b,0x10,
0x5d1c,0x10,
0x5d1d,0x10,
0x5d1e,0x01,
0x5d1f,0x02,
0x5d20,0x04,
0x5d21,0xaa,
0x5d34,0x00,
0x5d35,0x30,
0x5d36,0x00,
0x5d37,0x30,
0x5d38,0x1f,
0x5d39,0xc0,
0x5d3a,0x17,
0x5d3b,0xc0,
0x5d3d,0x08,
0x5d40,0x00,
0x5d41,0x00,
0x5d45,0x05,
0x5e42,0x04,
0x5e43,0x03,
0x5e44,0x02,
0x5e45,0x00,
0x5e46,0x04,
0x5e47,0x03,
0x5e48,0x02,
0x5e49,0x00,

};

static kal_uint16 addr_data_pair_preview[] = {
//4080x3060_4C2_30fps
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x0c,
0x3502,0x16,
0x3508,0x01,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3684,0x03,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x37b0,0x37,
0x37b3,0x3b,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x0b,
0x380b,0xf4,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x00,
0x3813,0x0a,
0x3814,0x11,
0x3815,0x11,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x382a,0xc1,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3856,0x10,
0x3857,0x0a,
0x3858,0x20,
0x3859,0x14,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x4010,0x38,
0x4016,0x0f,
0x4018,0x03,
0x401e,0x00,
0x401f,0xd0,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4510,0x07,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x4641,0x7f,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x480e,0x04,
0x4837,0x0b,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x5051,0x14,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x0c,
0x5195,0x0b,
0x5c00,0x63,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,

};
static kal_uint16 addr_data_pair_capture[] = {
	
//4080x3060_4C2_30fps
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x0c,
0x3502,0x16,
0x3508,0x01,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3684,0x03,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x37b0,0x37,
0x37b3,0x3b,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x0b,
0x380b,0xf4,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x00,
0x3813,0x0a,
0x3814,0x11,
0x3815,0x11,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x382a,0xc1,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3856,0x10,
0x3857,0x0a,
0x3858,0x20,
0x3859,0x14,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x4010,0x38,
0x4016,0x0f,
0x4018,0x03,
0x401e,0x00,
0x401f,0xd0,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4510,0x07,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x4641,0x7f,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x480e,0x04,
0x4837,0x0b,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x5051,0x14,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x0c,
0x5195,0x0b,
0x5c00,0x63,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,
};
static kal_uint16 addr_data_pair_normal_video[] = { 
//4080x2296_4C2_30fps_PD_1016x574
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x0c,
0x3502,0x16,
0x3508,0x01,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3684,0x03,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x37b0,0x37,
0x37b3,0x3b,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x08,
0x380b,0xf8,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x01,
0x3813,0x88,
0x3814,0x11,
0x3815,0x11,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x382a,0xc1,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3856,0x10,
0x3857,0x00,
0x3858,0x20,
0x3859,0x00,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x4010,0x38,
0x4016,0x0f,
0x4018,0x03,
0x401e,0x00,
0x401f,0xd0,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4510,0x07,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x4641,0x7f,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x480e,0x04,
0x4837,0x0b,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x5051,0x14,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x0c,
0x5195,0x0b,
0x5c00,0x63,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,

};
static kal_uint16 addr_data_pair_hs_video[] = {
//1280x720_120fps_NoPD
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x02,
0x3502,0xf1,
0x3508,0x0f,
0x3607,0x00,
0x3608,0xc5,
0x360c,0x0b,
0x360e,0x1e,
0x3684,0x00,
0x369b,0x80,
0x369c,0x13,
0x369d,0x00,
0x3700,0x28,
0x3701,0x05,
0x3702,0x48,
0x3703,0x20,
0x3704,0x07,
0x3706,0x26,
0x3707,0x08,
0x3708,0x32,
0x3709,0x82,
0x370b,0x56,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x63,
0x3715,0x00,
0x3717,0x01,
0x373b,0x2a,
0x373d,0x2e,
0x3741,0x3c,
0x3754,0xee,
0x375f,0x00,
0x37b0,0x32,
0x37b3,0x36,
0x3800,0x05,
0x3801,0xe0,
0x3802,0x06,
0x3803,0x60,
0x3804,0x1a,
0x3805,0x3f,
0x3806,0x11,
0x3807,0xbf,
0x3808,0x05,
0x3809,0x00,
0x380a,0x02,
0x380b,0xd0,
0x380c,0x02,
0x380d,0xee,
0x380e,0x04,
0x380f,0x56,
0x3810,0x00,
0x3811,0x0c,
0x3812,0x00,
0x3813,0x04,
0x3814,0x31,
0x3815,0x31,
0x3820,0x46,
0x3821,0x19,
0x3822,0x08,
0x382a,0xc3,
0x383d,0x0c,
0x3845,0x00,
0x3846,0xfa,
0x3856,0x00,
0x3857,0x00,
0x3858,0x00,
0x3859,0x00,
0x3902,0x02,
0x3903,0x00,
0x3905,0x28,
0x399b,0x00,
0x399c,0x00,
0x39aa,0x55,
0x39ba,0x26,
0x39bc,0x26,
0x39be,0x26,
0x39c0,0x26,
0x39c2,0x56,
0x39c4,0x56,
0x39c6,0x56,
0x39c8,0x56,
0x4010,0x28,
0x4016,0x07,
0x4018,0x01,
0x401e,0x01,
0x401f,0x00,
0x4031,0x10,
0x4032,0x00,
0x4033,0x10,
0x4034,0x08,
0x4035,0x10,
0x4036,0x08,
0x4037,0x10,
0x4040,0x08,
0x4041,0x10,
0x4042,0x08,
0x4043,0x10,
0x4044,0x00,
0x4045,0x10,
0x4046,0x00,
0x4047,0x10,
0x4510,0x00,
0x4516,0x00,
0x4517,0x00,
0x4518,0x00,
0x4519,0x00,
0x451a,0x00,
0x451b,0x00,
0x451c,0x00,
0x451d,0x00,
0x451e,0x00,
0x451f,0x00,
0x4520,0x00,
0x4521,0x00,
0x4641,0x1f,
0x4720,0x44,
0x4721,0x42,
0x4723,0x00,
0x4724,0x03,
0x4725,0x01,
0x4726,0x01,
0x4748,0x03,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x12,
0x4734,0x12,
0x4735,0x10,
0x4736,0x09,
0x4737,0x00,
0x4738,0x12,
0x4739,0x12,
0x473a,0x10,
0x473b,0x09,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x12,
0x473f,0x12,
0x4740,0x10,
0x4741,0x09,
0x480e,0x00,
0x4837,0x0b,
0x5000,0x89,
0x5001,0x02,
0x5002,0x81,
0x5003,0xfa,
0x5005,0x04,
0x5006,0x00,
0x5016,0x00,
0x5017,0x1b,
0x5035,0x0c,
0x5037,0x0a,
0x5038,0x07,
0x5039,0xc0,
0x503a,0x05,
0x503b,0xd0,
0x5051,0x18,
0x5187,0x99,
0x518c,0x01,
0x518d,0x03,
0x518e,0x01,
0x518f,0x03,
0x5190,0x02,
0x5191,0xf0,
0x5192,0x0d,
0x5193,0x1f,
0x5194,0x08,
0x5195,0xdf,
0x5c00,0x63,
0x5c03,0x7e,
0x5c04,0x00,
0x5c05,0xa8,
0x5c44,0x01,
0x5c45,0x02,
0x5c46,0x04,
0x5c47,0x03,
0x5c48,0x05,
0x5c49,0x06,
0x5c4a,0x07,
0x5c4b,0x08,
0x5c4c,0x0a,
0x5c4d,0x09,
0x5c4e,0x0b,
0x5c4f,0x0c,
0x5c50,0x0d,
0x5c51,0x0e,
0x5c52,0x0f,
0x5c53,0x10,
0x5c54,0x04,
0x5c55,0x04,
0x5c56,0x00,
0x5c57,0x00,
0x5c58,0x02,
0x5c59,0x02,
0x5c5a,0x00,
0x5c5b,0x00,
0x5c5c,0x02,
0x5c5d,0x02,
0x5c5e,0x00,
0x5d08,0x04,
0x5d09,0x04,
0x5d0a,0x00,
0x5d0b,0x00,
0x5d0c,0x02,
0x5d0d,0x02,
0x5d0e,0x00,
0x5d0f,0x00,
0x5d10,0x02,
0x5d11,0x02,



};
static kal_uint16 addr_data_pair_slim_video[] = {
//1280x720_120fps_NoPD
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x02,
0x3502,0xf1,
0x3508,0x0f,
0x3607,0x00,
0x3608,0xc5,
0x360c,0x0b,
0x360e,0x1e,
0x3684,0x00,
0x369b,0x80,
0x369c,0x13,
0x369d,0x00,
0x3700,0x28,
0x3701,0x05,
0x3702,0x48,
0x3703,0x20,
0x3704,0x07,
0x3706,0x26,
0x3707,0x08,
0x3708,0x32,
0x3709,0x82,
0x370b,0x56,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x63,
0x3715,0x00,
0x3717,0x01,
0x373b,0x2a,
0x373d,0x2e,
0x3741,0x3c,
0x3754,0xee,
0x375f,0x00,
0x37b0,0x32,
0x37b3,0x36,
0x3800,0x05,
0x3801,0xe0,
0x3802,0x06,
0x3803,0x60,
0x3804,0x1a,
0x3805,0x3f,
0x3806,0x11,
0x3807,0xbf,
0x3808,0x05,
0x3809,0x00,
0x380a,0x02,
0x380b,0xd0,
0x380c,0x02,
0x380d,0xee,
0x380e,0x04,
0x380f,0x56,
0x3810,0x00,
0x3811,0x0c,
0x3812,0x00,
0x3813,0x04,
0x3814,0x31,
0x3815,0x31,
0x3820,0x46,
0x3821,0x19,
0x3822,0x08,
0x382a,0xc3,
0x383d,0x0c,
0x3845,0x00,
0x3846,0xfa,
0x3856,0x00,
0x3857,0x00,
0x3858,0x00,
0x3859,0x00,
0x3902,0x02,
0x3903,0x00,
0x3905,0x28,
0x399b,0x00,
0x399c,0x00,
0x39aa,0x55,
0x39ba,0x26,
0x39bc,0x26,
0x39be,0x26,
0x39c0,0x26,
0x39c2,0x56,
0x39c4,0x56,
0x39c6,0x56,
0x39c8,0x56,
0x4010,0x28,
0x4016,0x07,
0x4018,0x01,
0x401e,0x01,
0x401f,0x00,
0x4031,0x10,
0x4032,0x00,
0x4033,0x10,
0x4034,0x08,
0x4035,0x10,
0x4036,0x08,
0x4037,0x10,
0x4040,0x08,
0x4041,0x10,
0x4042,0x08,
0x4043,0x10,
0x4044,0x00,
0x4045,0x10,
0x4046,0x00,
0x4047,0x10,
0x4510,0x00,
0x4516,0x00,
0x4517,0x00,
0x4518,0x00,
0x4519,0x00,
0x451a,0x00,
0x451b,0x00,
0x451c,0x00,
0x451d,0x00,
0x451e,0x00,
0x451f,0x00,
0x4520,0x00,
0x4521,0x00,
0x4641,0x1f,
0x4720,0x44,
0x4721,0x42,
0x4723,0x00,
0x4724,0x03,
0x4725,0x01,
0x4726,0x01,
0x4748,0x03,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x12,
0x4734,0x12,
0x4735,0x10,
0x4736,0x09,
0x4737,0x00,
0x4738,0x12,
0x4739,0x12,
0x473a,0x10,
0x473b,0x09,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x12,
0x473f,0x12,
0x4740,0x10,
0x4741,0x09,
0x480e,0x00,
0x4837,0x0b,
0x5000,0x89,
0x5001,0x02,
0x5002,0x81,
0x5003,0xfa,
0x5005,0x04,
0x5006,0x00,
0x5016,0x00,
0x5017,0x1b,
0x5035,0x0c,
0x5037,0x0a,
0x5038,0x07,
0x5039,0xc0,
0x503a,0x05,
0x503b,0xd0,
0x5051,0x18,
0x5187,0x99,
0x518c,0x01,
0x518d,0x03,
0x518e,0x01,
0x518f,0x03,
0x5190,0x02,
0x5191,0xf0,
0x5192,0x0d,
0x5193,0x1f,
0x5194,0x08,
0x5195,0xdf,
0x5c00,0x63,
0x5c03,0x7e,
0x5c04,0x00,
0x5c05,0xa8,
0x5c44,0x01,
0x5c45,0x02,
0x5c46,0x04,
0x5c47,0x03,
0x5c48,0x05,
0x5c49,0x06,
0x5c4a,0x07,
0x5c4b,0x08,
0x5c4c,0x0a,
0x5c4d,0x09,
0x5c4e,0x0b,
0x5c4f,0x0c,
0x5c50,0x0d,
0x5c51,0x0e,
0x5c52,0x0f,
0x5c53,0x10,
0x5c54,0x04,
0x5c55,0x04,
0x5c56,0x00,
0x5c57,0x00,
0x5c58,0x02,
0x5c59,0x02,
0x5c5a,0x00,
0x5c5b,0x00,
0x5c5c,0x02,
0x5c5d,0x02,
0x5c5e,0x00,
0x5d08,0x04,
0x5d09,0x04,
0x5d0a,0x00,
0x5d0b,0x00,
0x5d0c,0x02,
0x5d0d,0x02,
0x5d0e,0x00,
0x5d0f,0x00,
0x5d10,0x02,
0x5d11,0x02,

};
static kal_uint16 addr_data_pair_custom1[] = {
//4080x3060_4C2_30fps
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x0c,
0x3502,0x16,
0x3508,0x01,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3684,0x03,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x37b0,0x37,
0x37b3,0x3b,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x0b,
0x380b,0xf4,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x00,
0x3813,0x0a,
0x3814,0x11,
0x3815,0x11,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x382a,0xc1,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3856,0x10,
0x3857,0x0a,
0x3858,0x20,
0x3859,0x14,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x4010,0x38,
0x4016,0x0f,
0x4018,0x03,
0x401e,0x00,
0x401f,0xd0,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4510,0x07,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x4641,0x7f,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x480e,0x04,
0x4837,0x0b,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x5051,0x14,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x0c,
0x5195,0x0b,
0x5c00,0x63,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,

};
static kal_uint16 addr_data_pair_custom2[] = {
//3840x2160_60fps_NoPD
0x0303 ,0x05,
0x0304 ,0x02,
0x0305 ,0xa0,
0x0323 ,0x05,
0x0324 ,0x02,
0x0325 ,0xe2,
0x0343 ,0x03,
0x0344 ,0x00,
0x0345 ,0xfe,
0x3501 ,0x08,
0x3502 ,0x5e,
0x3508 ,0x0f,
0x3607 ,0x00,
0x3608 ,0xb2,
0x360c ,0x0b,
0x360e ,0x1e,
0x3684 ,0x00,
0x369b ,0x80,
0x369c ,0x13,
0x369d ,0x00,
0x3700 ,0x28,
0x3701 ,0x05,
0x3702 ,0x4b,
0x3703 ,0x27,
0x3704 ,0x07,
0x3706 ,0x26,
0x3707 ,0x08,
0x3708 ,0x32,
0x3709 ,0x69,
0x370b ,0x56,
0x370c ,0x0f,
0x3711 ,0x00,
0x3712 ,0x50,
0x3714 ,0x67,
0x3715 ,0x00,
0x3717 ,0x01,
0x373b ,0x2a,
0x373d ,0x2e,
0x3741 ,0x41,
0x3754 ,0xee,
0x375f ,0x00,
0x37b0 ,0x37,
0x37b3 ,0x3b,
0x3800 ,0x01,
0x3801 ,0x00,
0x3802 ,0x03,
0x3803 ,0x98,
0x3804 ,0x1f,
0x3805 ,0x1f,
0x3806 ,0x14,
0x3807 ,0x87,
0x3808 ,0x0f,
0x3809 ,0x00,
0x380a ,0x08,
0x380b ,0x70,
0x380c ,0x01,
0x380d ,0xf4,
0x380e ,0x0d,
0x380f ,0x05,
0x3810 ,0x00,
0x3811 ,0x08,
0x3812 ,0x00,
0x3813 ,0x04,
0x3814 ,0x11,
0x3815 ,0x11,
0x3820 ,0x06,
0x3821 ,0x10,
0x3822 ,0x00,
0x382a ,0xc1,
0x383d ,0x08,
0x3845 ,0x01,
0x3846 ,0xf4,
0x3856 ,0x00,
0x3857 ,0x00,
0x3858 ,0x00,
0x3859 ,0x00,
0x3902 ,0x02,
0x3903 ,0x00,
0x3905 ,0x28,
0x399b ,0x00,
0x399c ,0x00,
0x39aa ,0x55,
0x39ba ,0x26,
0x39bc ,0x26,
0x39be ,0x26,
0x39c0 ,0x26,
0x39c2 ,0x56,
0x39c4 ,0x56,
0x39c6 ,0x56,
0x39c8 ,0x56,
0x4010 ,0x38,
0x4016 ,0x0f,
0x4018 ,0x03,
0x401e ,0x01,
0x401f ,0x00,
0x4031 ,0x02,
0x4032 ,0x00,
0x4033 ,0x02,
0x4034 ,0x00,
0x4035 ,0x02,
0x4036 ,0x00,
0x4037 ,0x02,
0x4040 ,0x00,
0x4041 ,0x00,
0x4042 ,0x00,
0x4043 ,0x00,
0x4044 ,0x00,
0x4045 ,0x00,
0x4046 ,0x00,
0x4047 ,0x00,
0x4510 ,0x00,
0x4516 ,0x00,
0x4517 ,0x00,
0x4518 ,0x00,
0x4519 ,0x00,
0x451a ,0x00,
0x451b ,0x00,
0x451c ,0x00,
0x451d ,0x00,
0x451e ,0x00,
0x451f ,0x00,
0x4520 ,0x00,
0x4521 ,0x00,
0x4641 ,0x1f,
0x4720 ,0x44,
0x4721 ,0x42,
0x4723 ,0x00,
0x4724 ,0x03,
0x4725 ,0x01,
0x4726 ,0x01,
0x4748 ,0x03,
0x4731 ,0x0d,
0x4732 ,0x0d,
0x4733 ,0x0f,
0x4734 ,0x0f,
0x4735 ,0x0c,
0x4736 ,0x04,
0x4737 ,0x00,
0x4738 ,0x0f,
0x4739 ,0x0f,
0x473a ,0x0c,
0x473b ,0x04,
0x473c ,0x00,
0x473d ,0x0d,
0x473e ,0x0f,
0x473f ,0x0f,
0x4740 ,0x0c,
0x4741 ,0x04,
0x480e ,0x00,
0x4837 ,0x07,
0x5000 ,0x89,
0x5001 ,0x03,
0x5002 ,0x8d,
0x5003 ,0xfa,
0x5005 ,0x00,
0x5006 ,0x00,
0x5016 ,0x00,
0x5017 ,0x00,
0x5035 ,0x18,
0x5037 ,0x14,
0x5038 ,0x0f,
0x5039 ,0xe0,
0x503a ,0x0b,
0x503b ,0xe0,
0x5051 ,0x14,
0x5187 ,0x88,
0x518c ,0x01,
0x518d ,0x01,
0x518e ,0x01,
0x518f ,0x01,
0x5190 ,0x00,
0x5191 ,0x80,
0x5192 ,0x0f,
0x5193 ,0x8f,
0x5194 ,0x0a,
0x5195 ,0x43,
0x5c00 ,0x61,
0x5c03 ,0x3f,
0x5c04 ,0x00,
0x5c05 ,0x54,
0x5c44 ,0x01,
0x5c45 ,0x02,
0x5c46 ,0x04,
0x5c47 ,0x03,
0x5c48 ,0x05,
0x5c49 ,0x06,
0x5c4a ,0x07,
0x5c4b ,0x08,
0x5c4c ,0x0a,
0x5c4d ,0x09,
0x5c4e ,0x0b,
0x5c4f ,0x0c,
0x5c50 ,0x0d,
0x5c51 ,0x0e,
0x5c52 ,0x0f,
0x5c53 ,0x10,
0x5c54 ,0x08,
0x5c55 ,0x08,
0x5c56 ,0x02,
0x5c57 ,0x02,
0x5c58 ,0x06,
0x5c59 ,0x06,
0x5c5a ,0x02,
0x5c5b ,0x02,
0x5c5c ,0x06,
0x5c5d ,0x06,
0x5c5e ,0x00,
0x5d08 ,0x08,
0x5d09 ,0x08,
0x5d0a ,0x02,
0x5d0b ,0x02,
0x5d0c ,0x06,
0x5d0d ,0x06,
0x5d0e ,0x02,
0x5d0f ,0x02,
0x5d10 ,0x06,
0x5d11 ,0x06,

};
static kal_uint16 addr_data_pair_custom3[] = {
//8160x6120_4C2_10fps_PD_1016x760
0x0303, 0x05,
0x0304, 0x02,
0x0305, 0xe4,
0x0323, 0x05,
0x0324, 0x02,
0x0325, 0xe2,
0x0343, 0x03,
0x0344, 0x00,
0x0345, 0xfe,
0x3501, 0x0c,
0x3502, 0x73,
0x3508, 0x01,
0x3607, 0x01,
0x3608, 0xe4,
0x360c, 0x03,
0x360e, 0x1d,
0x3684, 0x03,
0x369b, 0x10,
0x369c, 0x2B,
0x369d, 0x40,
0x3700, 0x28,
0x3701, 0x05,
0x3702, 0x4b,
0x3703, 0x27,
0x3704, 0x07,
0x3706, 0x2f,
0x3707, 0x08,
0x3708, 0x32,
0x3709, 0x60,
0x370b, 0x5d,
0x370c, 0x0f,
0x3711, 0x00,
0x3712, 0x50,
0x3714, 0x67,
0x3715, 0x00,
0x3717, 0x03,
0x373b, 0x3c,
0x373d, 0x40,
0x3741, 0x41,
0x3754, 0xee,
0x375f, 0x80,
0x37b0, 0x37,
0x37b3, 0x3b,
0x3800, 0x00,
0x3801, 0x00,
0x3802, 0x00,
0x3803, 0x08,
0x3804, 0x20,
0x3805, 0x1f,
0x3806, 0x18,
0x3807, 0x17,
0x3808, 0x1f,
0x3809, 0xe0,
0x380a, 0x17,
0x380b, 0xe8,
0x380c, 0x09,
0x380d, 0xf6,
0x380e, 0x0f,
0x380f, 0x4e,
0x3810, 0x00,
0x3811, 0x20,
0x3812, 0x00,
0x3813, 0x14,
0x3814, 0x11,
0x3815, 0x11,
0x3820, 0x06,
0x3821, 0x02,
0x3822, 0x12,
0x382a, 0xc1,
0x383d, 0x10,
0x3845, 0x09,
0x3846, 0xf6,
0x3856, 0x10,
0x3857, 0x0a,
0x3858, 0x20,
0x3859, 0x14,
0x3902, 0x01,
0x3903, 0x08,
0x3905, 0x05,
0x399b, 0x02,
0x399c, 0x03,
0x39aa, 0x05,
0x39ba, 0x2f,
0x39bc, 0x2f,
0x39be, 0x2f,
0x39c0, 0x2f,
0x39c2, 0x5d,
0x39c4, 0x5d,
0x39c6, 0x5d,
0x39c8, 0x5d,
0x4010, 0x28,
0x4016, 0x0f,
0x4018, 0x03,
0x401e, 0x00,
0x401f, 0xd0,
0x4031, 0x10,
0x4032, 0x00,
0x4033, 0x10,
0x4034, 0x08,
0x4035, 0x10,
0x4036, 0x08,
0x4037, 0x10,
0x4040, 0x08,
0x4041, 0x10,
0x4042, 0x08,
0x4043, 0x10,
0x4044, 0x00,
0x4045, 0x10,
0x4046, 0x00,
0x4047, 0x10,
0x4510, 0x07,
0x4516, 0x55,
0x4517, 0x55,
0x4518, 0x55,
0x4519, 0x55,
0x451a, 0x11,
0x451b, 0xbb,
0x451c, 0x11,
0x451d, 0xbb,
0x451e, 0x11,
0x451f, 0xbb,
0x4520, 0x11,
0x4521, 0xbb,
0x4641, 0x7f,
0x4720, 0x00,
0x4721, 0x00,
0x4723, 0x00,
0x4724, 0x00,
0x4725, 0x01,
0x4726, 0x01,
0x4748, 0x00,
0x4731, 0x0d,
0x4732, 0x0d,
0x4733, 0x0f,
0x4734, 0x0f,
0x4735, 0x0c,
0x4736, 0x04,
0x4737, 0x00,
0x4738, 0x0f,
0x4739, 0x0f,
0x473a, 0x0c,
0x473b, 0x04,
0x473c, 0x00,
0x473d, 0x0d,
0x473e, 0x0f,
0x473f, 0x0f,
0x4740, 0x0c,
0x4741, 0x04,
0x480e, 0x04,
0x4837, 0x06,
0x5000, 0x8b,
0x5001, 0x63,
0x5002, 0x9d,
0x5003, 0xca,
0x5005, 0x00,
0x5006, 0x00,
0x5016, 0x00,
0x5017, 0x00,
0x5035, 0x18,
0x5037, 0x14,
0x5038, 0x0f,
0x5039, 0xe0,
0x503a, 0x0b,
0x503b, 0xe0,
0x5051, 0x14,
0x5187, 0x88,
0x518c, 0x01,
0x518d, 0x01,
0x518e, 0x01,
0x518f, 0x01,
0x5190, 0x00,
0x5191, 0x00,
0x5192, 0x10,
0x5193, 0x0f,
0x5194, 0x0c,
0x5195, 0x0b,
0x5c00, 0x63,
0x5c03, 0x3f,
0x5c04, 0x00,
0x5c05, 0x54,
0x5c44, 0x02,
0x5c45, 0x0a,
0x5c46, 0x00,
0x5c47, 0x0f,
0x5c48, 0x07,
0x5c49, 0x01,
0x5c4a, 0x00,
0x5c4b, 0x09,
0x5c4c, 0x0e,
0x5c4d, 0x06,
0x5c4e, 0x00,
0x5c4f, 0x00,
0x5c50, 0x00,
0x5c51, 0x00,
0x5c52, 0x00,
0x5c53, 0x00,
0x5c54, 0x10,
0x5c55, 0x08,
0x5c56, 0x04,
0x5c57, 0x04,
0x5c58, 0x0c,
0x5c59, 0x0c,
0x5c5a, 0x02,
0x5c5b, 0x02,
0x5c5c, 0x06,
0x5c5d, 0x06,
0x5c5e, 0x01,
0x5d08, 0x08,
0x5d09, 0x08,
0x5d0a, 0x02,
0x5d0b, 0x02,
0x5d0c, 0x06,
0x5d0d, 0x06,
0x5d0e, 0x02,
0x5d0f, 0x02,
0x5d10, 0x06,
0x5d11, 0x06,

};
static kal_uint16 addr_data_pair_custom4[] = {
//3840x2160_60fps_NoPD
0x0303 ,0x05,
0x0304 ,0x02,
0x0305 ,0xa0,
0x0323 ,0x05,
0x0324 ,0x02,
0x0325 ,0xe2,
0x0343 ,0x03,
0x0344 ,0x00,
0x0345 ,0xfe,
0x3501 ,0x08,
0x3502 ,0x5e,
0x3508 ,0x0f,
0x3607 ,0x00,
0x3608 ,0xb2,
0x360c ,0x0b,
0x360e ,0x1e,
0x3684 ,0x00,
0x369b ,0x80,
0x369c ,0x13,
0x369d ,0x00,
0x3700 ,0x28,
0x3701 ,0x05,
0x3702 ,0x4b,
0x3703 ,0x27,
0x3704 ,0x07,
0x3706 ,0x26,
0x3707 ,0x08,
0x3708 ,0x32,
0x3709 ,0x69,
0x370b ,0x56,
0x370c ,0x0f,
0x3711 ,0x00,
0x3712 ,0x50,
0x3714 ,0x67,
0x3715 ,0x00,
0x3717 ,0x01,
0x373b ,0x2a,
0x373d ,0x2e,
0x3741 ,0x41,
0x3754 ,0xee,
0x375f ,0x00,
0x37b0 ,0x37,
0x37b3 ,0x3b,
0x3800 ,0x01,
0x3801 ,0x00,
0x3802 ,0x03,
0x3803 ,0x98,
0x3804 ,0x1f,
0x3805 ,0x1f,
0x3806 ,0x14,
0x3807 ,0x87,
0x3808 ,0x0f,
0x3809 ,0x00,
0x380a ,0x08,
0x380b ,0x70,
0x380c ,0x01,
0x380d ,0xf4,
0x380e ,0x0d,
0x380f ,0x05,
0x3810 ,0x00,
0x3811 ,0x08,
0x3812 ,0x00,
0x3813 ,0x04,
0x3814 ,0x11,
0x3815 ,0x11,
0x3820 ,0x06,
0x3821 ,0x10,
0x3822 ,0x00,
0x382a ,0xc1,
0x383d ,0x08,
0x3845 ,0x01,
0x3846 ,0xf4,
0x3856 ,0x00,
0x3857 ,0x00,
0x3858 ,0x00,
0x3859 ,0x00,
0x3902 ,0x02,
0x3903 ,0x00,
0x3905 ,0x28,
0x399b ,0x00,
0x399c ,0x00,
0x39aa ,0x55,
0x39ba ,0x26,
0x39bc ,0x26,
0x39be ,0x26,
0x39c0 ,0x26,
0x39c2 ,0x56,
0x39c4 ,0x56,
0x39c6 ,0x56,
0x39c8 ,0x56,
0x4010 ,0x38,
0x4016 ,0x0f,
0x4018 ,0x03,
0x401e ,0x01,
0x401f ,0x00,
0x4031 ,0x02,
0x4032 ,0x00,
0x4033 ,0x02,
0x4034 ,0x00,
0x4035 ,0x02,
0x4036 ,0x00,
0x4037 ,0x02,
0x4040 ,0x00,
0x4041 ,0x00,
0x4042 ,0x00,
0x4043 ,0x00,
0x4044 ,0x00,
0x4045 ,0x00,
0x4046 ,0x00,
0x4047 ,0x00,
0x4510 ,0x00,
0x4516 ,0x00,
0x4517 ,0x00,
0x4518 ,0x00,
0x4519 ,0x00,
0x451a ,0x00,
0x451b ,0x00,
0x451c ,0x00,
0x451d ,0x00,
0x451e ,0x00,
0x451f ,0x00,
0x4520 ,0x00,
0x4521 ,0x00,
0x4641 ,0x1f,
0x4720 ,0x44,
0x4721 ,0x42,
0x4723 ,0x00,
0x4724 ,0x03,
0x4725 ,0x01,
0x4726 ,0x01,
0x4748 ,0x03,
0x4731 ,0x0d,
0x4732 ,0x0d,
0x4733 ,0x0f,
0x4734 ,0x0f,
0x4735 ,0x0c,
0x4736 ,0x04,
0x4737 ,0x00,
0x4738 ,0x0f,
0x4739 ,0x0f,
0x473a ,0x0c,
0x473b ,0x04,
0x473c ,0x00,
0x473d ,0x0d,
0x473e ,0x0f,
0x473f ,0x0f,
0x4740 ,0x0c,
0x4741 ,0x04,
0x480e ,0x00,
0x4837 ,0x07,
0x5000 ,0x89,
0x5001 ,0x03,
0x5002 ,0x8d,
0x5003 ,0xfa,
0x5005 ,0x00,
0x5006 ,0x00,
0x5016 ,0x00,
0x5017 ,0x00,
0x5035 ,0x18,
0x5037 ,0x14,
0x5038 ,0x0f,
0x5039 ,0xe0,
0x503a ,0x0b,
0x503b ,0xe0,
0x5051 ,0x14,
0x5187 ,0x88,
0x518c ,0x01,
0x518d ,0x01,
0x518e ,0x01,
0x518f ,0x01,
0x5190 ,0x00,
0x5191 ,0x80,
0x5192 ,0x0f,
0x5193 ,0x8f,
0x5194 ,0x0a,
0x5195 ,0x43,
0x5c00 ,0x61,
0x5c03 ,0x3f,
0x5c04 ,0x00,
0x5c05 ,0x54,
0x5c44 ,0x01,
0x5c45 ,0x02,
0x5c46 ,0x04,
0x5c47 ,0x03,
0x5c48 ,0x05,
0x5c49 ,0x06,
0x5c4a ,0x07,
0x5c4b ,0x08,
0x5c4c ,0x0a,
0x5c4d ,0x09,
0x5c4e ,0x0b,
0x5c4f ,0x0c,
0x5c50 ,0x0d,
0x5c51 ,0x0e,
0x5c52 ,0x0f,
0x5c53 ,0x10,
0x5c54 ,0x08,
0x5c55 ,0x08,
0x5c56 ,0x02,
0x5c57 ,0x02,
0x5c58 ,0x06,
0x5c59 ,0x06,
0x5c5a ,0x02,
0x5c5b ,0x02,
0x5c5c ,0x06,
0x5c5d ,0x06,
0x5c5e ,0x00,
0x5d08 ,0x08,
0x5d09 ,0x08,
0x5d0a ,0x02,
0x5d0b ,0x02,
0x5d0c ,0x06,
0x5d0d ,0x06,
0x5d0e ,0x02,
0x5d0f ,0x02,
0x5d10 ,0x06,
0x5d11 ,0x06,

};
static kal_uint16 addr_data_pair_custom5[] = {
//4080x3060_4C2_30fps
0x0303,0x05,
0x0304,0x01,
0x0305,0xb1,
0x0323,0x05,
0x0324,0x02,
0x0325,0xe2,
0x0343,0x03,
0x0344,0x00,
0x0345,0xfe,
0x3501,0x0c,
0x3502,0x16,
0x3508,0x01,
0x3607,0x01,
0x3608,0xda,
0x360c,0x03,
0x360e,0x1d,
0x3684,0x03,
0x369b,0x10,
0x369c,0x2B,
0x369d,0x40,
0x3700,0x28,
0x3701,0x05,
0x3702,0x4b,
0x3703,0x27,
0x3704,0x07,
0x3706,0x2f,
0x3707,0x08,
0x3708,0x32,
0x3709,0x69,
0x370b,0x5d,
0x370c,0x0f,
0x3711,0x00,
0x3712,0x50,
0x3714,0x67,
0x3715,0x00,
0x3717,0x03,
0x373b,0x3c,
0x373d,0x40,
0x3741,0x41,
0x3754,0xee,
0x375f,0x80,
0x37b0,0x37,
0x37b3,0x3b,
0x3800,0x00,
0x3801,0x00,
0x3802,0x00,
0x3803,0x08,
0x3804,0x20,
0x3805,0x1f,
0x3806,0x18,
0x3807,0x17,
0x3808,0x0f,
0x3809,0xf0,
0x380a,0x0b,
0x380b,0xf4,
0x380c,0x04,
0x380d,0x1a,
0x380e,0x0c,
0x380f,0x66,
0x3810,0x00,
0x3811,0x10,
0x3812,0x00,
0x3813,0x0a,
0x3814,0x11,
0x3815,0x11,
0x3820,0x06,
0x3821,0x02,
0x3822,0x10,
0x382a,0xc1,
0x383d,0x10,
0x3845,0x04,
0x3846,0x1a,
0x3856,0x10,
0x3857,0x0a,
0x3858,0x20,
0x3859,0x14,
0x3902,0x01,
0x3903,0x08,
0x3905,0x05,
0x399b,0x02,
0x399c,0x03,
0x39aa,0x05,
0x39ba,0x2f,
0x39bc,0x2f,
0x39be,0x2f,
0x39c0,0x2f,
0x39c2,0x5d,
0x39c4,0x5d,
0x39c6,0x5d,
0x39c8,0x5d,
0x4010,0x38,
0x4016,0x0f,
0x4018,0x03,
0x401e,0x00,
0x401f,0xd0,
0x4031,0x00,
0x4032,0x00,
0x4033,0x04,
0x4034,0x00,
0x4035,0x04,
0x4036,0x00,
0x4037,0x04,
0x4040,0x00,
0x4041,0x00,
0x4042,0x00,
0x4043,0x00,
0x4044,0x00,
0x4045,0x00,
0x4046,0x00,
0x4047,0x00,
0x4510,0x07,
0x4516,0x55,
0x4517,0x55,
0x4518,0x55,
0x4519,0x55,
0x451a,0x11,
0x451b,0xbb,
0x451c,0x11,
0x451d,0xbb,
0x451e,0x11,
0x451f,0xbb,
0x4520,0x11,
0x4521,0xbb,
0x4641,0x7f,
0x4720,0x00,
0x4721,0x00,
0x4723,0x00,
0x4724,0x00,
0x4725,0x01,
0x4726,0x01,
0x4748,0x00,
0x4731,0x0d,
0x4732,0x0d,
0x4733,0x0f,
0x4734,0x0f,
0x4735,0x0c,
0x4736,0x04,
0x4737,0x00,
0x4738,0x0f,
0x4739,0x0f,
0x473a,0x0c,
0x473b,0x04,
0x473c,0x00,
0x473d,0x0d,
0x473e,0x0f,
0x473f,0x0f,
0x4740,0x0c,
0x4741,0x04,
0x480e,0x04,
0x4837,0x0b,
0x5000,0x8b,
0x5001,0x23,
0x5002,0x9d,
0x5003,0xca,
0x5005,0x00,
0x5006,0x00,
0x5016,0x00,
0x5017,0x00,
0x5035,0x18,
0x5037,0x14,
0x5038,0x0f,
0x5039,0xe0,
0x503a,0x0b,
0x503b,0xe0,
0x5051,0x14,
0x5187,0x88,
0x518c,0x01,
0x518d,0x01,
0x518e,0x01,
0x518f,0x01,
0x5190,0x00,
0x5191,0x00,
0x5192,0x10,
0x5193,0x0f,
0x5194,0x0c,
0x5195,0x0b,
0x5c00,0x63,
0x5c03,0x3f,
0x5c04,0x00,
0x5c05,0x54,
0x5c44,0x02,
0x5c45,0x0a,
0x5c46,0x00,
0x5c47,0x0f,
0x5c48,0x07,
0x5c49,0x01,
0x5c4a,0x00,
0x5c4b,0x09,
0x5c4c,0x0e,
0x5c4d,0x06,
0x5c4e,0x00,
0x5c4f,0x00,
0x5c50,0x00,
0x5c51,0x00,
0x5c52,0x00,
0x5c53,0x00,
0x5c54,0x10,
0x5c55,0x08,
0x5c56,0x04,
0x5c57,0x04,
0x5c58,0x0c,
0x5c59,0x0c,
0x5c5a,0x02,
0x5c5b,0x02,
0x5c5c,0x06,
0x5c5d,0x06,
0x5c5e,0x01,
0x5d08,0x08,
0x5d09,0x08,
0x5d0a,0x02,
0x5d0b,0x02,
0x5d0c,0x06,
0x5d0d,0x06,
0x5d0e,0x02,
0x5d0f,0x02,
0x5d10,0x06,
0x5d11,0x06,

};

static void sensor_init(void)
{
	//Global setting 
	
//   int i;
	pr_debinfo("start \n");	
	table_write_cmos_sensor(addr_data_pair_init,
		   sizeof(addr_data_pair_init) / sizeof(kal_uint16));

//	for(i = 0; i<288; i++){
//   write_cmos_sensor_16_8(0x5a40 + i, crosstalk_main_ov50c40pd2164[i+1]); 
//   }

	pr_debinfo("end \n");				  
}	/*	sensor_init  */


static void preview_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_preview,
		   sizeof(addr_data_pair_preview) / sizeof(kal_uint16));
	pr_debinfo("end \n");
}	/*	preview_setting  */

// Pll Setting - VCO = 280Mhz
static void capture_setting(kal_uint16 currefps)
{
	

    pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_capture,
		   sizeof(addr_data_pair_capture) / sizeof(kal_uint16));
	

	pr_debinfo("end \n");
}


static void normal_video_setting(kal_uint16 currefps)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_normal_video,
		   sizeof(addr_data_pair_normal_video) / sizeof(kal_uint16));
	pr_debinfo("end \n");
}

static void hs_video_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_hs_video,
		   sizeof(addr_data_pair_hs_video) / sizeof(kal_uint16));
	pr_debinfo("end \n");
}

static void slim_video_setting(void)
{
    pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_slim_video,
		   sizeof(addr_data_pair_slim_video) / sizeof(kal_uint16));
	pr_debinfo("end \n");

}

static void custom1_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_custom1,
		   sizeof(addr_data_pair_custom1) / sizeof(kal_uint16));
	pr_debinfo("end \n");
}

static void custom2_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_custom2,
		   sizeof(addr_data_pair_custom2) / sizeof(kal_uint16));
	pr_debinfo("end \n");


}

static void custom3_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_custom3,
		   sizeof(addr_data_pair_custom3) / sizeof(kal_uint16));
	pr_debinfo("end \n");


}

static void custom4_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_custom4,
		   sizeof(addr_data_pair_custom4) / sizeof(kal_uint16));
	pr_debinfo("end \n");

}
static void custom5_setting(void)
{
	pr_debinfo("start \n");
	table_write_cmos_sensor(addr_data_pair_custom5,
		   sizeof(addr_data_pair_custom5) / sizeof(kal_uint16));
	pr_debinfo("end \n");

}
static kal_uint32 return_sensor_id(void)
{
	
    return ((read_cmos_sensor_16_8(0x300b) << 8) | ((read_cmos_sensor_16_8(0x300c))));
}

/*************************************************************************
 * FUNCTION
 *	get_imgsensor_id
 *
 * DESCRIPTION
 *	This function get the sensor ID
 *
 * PARAMETERS
 *	*sensorID : return the sensor ID
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
 extern int check_i2c_timeout(u16 addr, u16 i2cid);
static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
#if 0
	int I2C_BUS = -1 ;
	I2C_BUS = i2c_adapter_id(pgi2c_cfg_legacy->pinst->pi2c_client->adapter);
	pr_debinfo(" I2C_BUS = %d\n",I2C_BUS);
	if(I2C_BUS != 4){	
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
#endif
	#if 1  //IIC is grounded, the camera can be opend ,check iic timeout
		kal_uint8 timeout = 0;
		timeout = check_i2c_timeout(0x300b, imgsensor.i2c_write_id);
	    if (timeout) {
	    	 pr_err(PFX "[%s] timeout =null \n",__FUNCTION__,timeout);
	        *sensor_id = 0xFFFFFFFF;
	        return ERROR_SENSOR_CONNECT_FAIL;
	    }
	#endif
	/*sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address*/
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			*sensor_id = return_sensor_id();
			pr_debinfo("read out sensor id 0x%x \n",*sensor_id);
			if ( *sensor_id == imgsensor_info.sensor_id) {
				pr_debinfo("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
				/*vivo lxd add for CameraEM otp errorcode*/
				pr_debug("start read eeprom ---vivo_otp_read_when_power_on = %d\n", vivo_otp_read_when_power_on);
				vivo_otp_read_when_power_on = ov50c40pd2164_otp_read();
				pr_debug("read eeprom ---vivo_otp_read_when_power_on = %d,OV50C40PD2164_OTP_ERROR_CODE=%d\n", vivo_otp_read_when_power_on, OV50C40PD2164_OTP_ERROR_CODE);
				/*vivo lxd add end*/
				return ERROR_NONE;
			}
			pr_debinfo("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id);
			retry--;
		} while(retry > 0);
		i++;
		retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		/* if Sensor ID is not correct, Must set *sensor_id to 0xFFFFFFFF*/
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}


/*************************************************************************
 * FUNCTION
 *	open
 *
 * DESCRIPTION
 *	This function initialize the registers of CMOS sensor
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 open(void)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint16 sensor_id = 0;

	pr_debinfo("%s", __func__);

	/*sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address*/
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
	    		sensor_id = return_sensor_id();
			if (sensor_id == imgsensor_info.sensor_id) {
				printk("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
				break;
			}
			pr_debinfo("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id);
			retry--;
		} while(retry > 0);
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (imgsensor_info.sensor_id != sensor_id)
		return ERROR_SENSOR_CONNECT_FAIL;

	/* initail sequence write in  */
	sensor_init();

	spin_lock(&imgsensor_drv_lock);

	imgsensor.autoflicker_en= KAL_FALSE;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.shutter = 0xc00;
	imgsensor.gain = 0x100;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_mode = 0;
	imgsensor.test_pattern = KAL_FALSE;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	spin_unlock(&imgsensor_drv_lock);
	
	
	   
	return ERROR_NONE;
}	/*	open  */



/*************************************************************************
* FUNCTION
*	close
*
* DESCRIPTION
*
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 close(void)
{
	pr_debinfo("E\n");

	return ERROR_NONE;
}				/*      close  */


/*************************************************************************
 * FUNCTION
 * preview
 *
 * DESCRIPTION
 *	This function start the sensor preview.
 *
 * PARAMETERS
 *	*image_window : address pointer of pixel numbers in one period of HSYNC
 *  *sensor_config_data : address pointer of line numbers in one period of VSYNC
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("%s E\n", __func__);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	preview_setting();
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}				/*      preview   */

/*************************************************************************
 * FUNCTION
 *	capture
 *
 * DESCRIPTION
 *	This function setup the CMOS sensor in capture MY_OUTPUT mode
 *
 * PARAMETERS
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("%s E\n", __func__);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
	if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
	pr_debinfo("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",imgsensor.current_fps,imgsensor_info.cap.max_framerate/10);
	imgsensor.pclk = imgsensor_info.cap.pclk;
	imgsensor.line_length = imgsensor_info.cap.linelength;
	imgsensor.frame_length = imgsensor_info.cap.framelength;
	imgsensor.min_frame_length = imgsensor_info.cap.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	
	capture_setting(imgsensor.current_fps);

	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}				/* capture() */

static kal_uint32 normal_video(
	MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
	MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("%s E\n", __func__);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length = imgsensor_info.normal_video.linelength;
	imgsensor.frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	normal_video_setting(imgsensor.current_fps);
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}				/*      normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			   MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("%s E\n", __func__);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	/* imgsensor.video_mode = KAL_TRUE; */
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	/* imgsensor.current_fps = 300; */
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}				/*      hs_video   */

static kal_uint32 slim_video(
	MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
	MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("%s E\n", __func__);

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	/* imgsensor.video_mode = KAL_TRUE; */
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	/* imgsensor.current_fps = 300; */
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}				/*      slim_video       */

static kal_uint32 Custom1(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM1;
	imgsensor.pclk = imgsensor_info.custom1.pclk;
	imgsensor.line_length = imgsensor_info.custom1.linelength;
	imgsensor.frame_length = imgsensor_info.custom1.framelength;
	imgsensor.min_frame_length = imgsensor_info.custom1.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	custom1_setting();/*using preview setting*/
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}	/*	custom1   */

static kal_uint32 Custom2(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	pr_debinfo("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM2;
	imgsensor.pclk = imgsensor_info.custom2.pclk;
	imgsensor.line_length = imgsensor_info.custom2.linelength;
	imgsensor.frame_length = imgsensor_info.custom2.framelength;
	imgsensor.min_frame_length = imgsensor_info.custom2.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	custom2_setting();/*using preview setting*/
	set_mirror_flip(imgsensor.mirror);

	return ERROR_NONE;
}	/*	custom2   */

static kal_uint32 Custom3(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,      
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)                       
{                                                                                
	pr_debinfo("E\n");                                                                
                                                                                 
	spin_lock(&imgsensor_drv_lock);                                                
	imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM3;                                
	imgsensor.pclk = imgsensor_info.custom3.pclk;                                  
	imgsensor.line_length = imgsensor_info.custom3.linelength;                     
	imgsensor.frame_length = imgsensor_info.custom3.framelength;                   
	imgsensor.min_frame_length = imgsensor_info.custom3.framelength;               
	imgsensor.autoflicker_en = KAL_FALSE;                                          
	spin_unlock(&imgsensor_drv_lock);                                              
                                                                                 
	custom3_setting();                                    
	set_mirror_flip(imgsensor.mirror);                                             
                                                                                 
	return ERROR_NONE;                                                             
}	/*	custom3   */                                                               

 
static kal_uint32 Custom4(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,	  
					   MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)						
 {																				  
	 pr_debinfo("E\n"); 															   
																				  
	 spin_lock(&imgsensor_drv_lock);												
	 imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM4;								
	 imgsensor.pclk = imgsensor_info.custom4.pclk;									
	 imgsensor.line_length = imgsensor_info.custom4.linelength; 					
	 imgsensor.frame_length = imgsensor_info.custom4.framelength;					
	 imgsensor.min_frame_length = imgsensor_info.custom4.framelength;				
	 imgsensor.autoflicker_en = KAL_FALSE;											
	 spin_unlock(&imgsensor_drv_lock);												
																				  
	 custom4_setting();									
	 set_mirror_flip(imgsensor.mirror); 											
																				  
	 return ERROR_NONE; 															
 }	 /*  custom4   */

static kal_uint32 Custom5(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,	  
					   MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)						
 {																				  
	 pr_debinfo("E\n"); 															   
																				  
	 spin_lock(&imgsensor_drv_lock);												
	 imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM5;								
	 imgsensor.pclk = imgsensor_info.custom4.pclk;									
	 imgsensor.line_length = imgsensor_info.custom5.linelength; 					
	 imgsensor.frame_length = imgsensor_info.custom5.framelength;					
	 imgsensor.min_frame_length = imgsensor_info.custom5.framelength;				
	 imgsensor.autoflicker_en = KAL_FALSE;											
	 spin_unlock(&imgsensor_drv_lock);												
																				  
	 custom5_setting();									
	 set_mirror_flip(imgsensor.mirror); 											
																				  
	 return ERROR_NONE; 															
 }	 /*  custom4   */															
static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
	pr_debinfo("%s E\n", __func__);
	
	sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;

	sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

	sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;

	sensor_resolution->SensorHighSpeedVideoWidth = imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight = imgsensor_info.hs_video.grabwindow_height;

	sensor_resolution->SensorSlimVideoWidth = imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight = imgsensor_info.slim_video.grabwindow_height;

	sensor_resolution->SensorCustom1Width = imgsensor_info.custom1.grabwindow_width;
	sensor_resolution->SensorCustom1Height = imgsensor_info.custom1.grabwindow_height;

	sensor_resolution->SensorCustom2Width = imgsensor_info.custom2.grabwindow_width;
	sensor_resolution->SensorCustom2Height = imgsensor_info.custom2.grabwindow_height;
	
	sensor_resolution->SensorCustom3Width = imgsensor_info.custom3.grabwindow_width;
	sensor_resolution->SensorCustom3Height = imgsensor_info.custom3.grabwindow_height;
	
	sensor_resolution->SensorCustom4Width = imgsensor_info.custom4.grabwindow_width;
	sensor_resolution->SensorCustom4Height =imgsensor_info.custom4.grabwindow_height;

	sensor_resolution->SensorCustom5Width = imgsensor_info.custom5.grabwindow_width;
	sensor_resolution->SensorCustom5Height =imgsensor_info.custom5.grabwindow_height;

	return ERROR_NONE;
}				/*      get_resolution  */

static kal_uint32 get_info(enum MSDK_SCENARIO_ID_ENUM scenario_id,
			   MSDK_SENSOR_INFO_STRUCT *sensor_info,
			   MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	/*printk("get_info -> scenario_id = %d\n", scenario_id);*/

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;

	/* not use */
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;

	/* inverse with datasheet */
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;

	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4;	/* not use */
	sensor_info->SensorResetActiveHigh = FALSE;	/* not use */
	sensor_info->SensorResetDelayCount = 5;	/* not use */

	sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
	sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;

	sensor_info->SensorOutputDataFormat =
		imgsensor_info.sensor_output_dataformat;

	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
	sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;

	sensor_info->HighSpeedVideoDelayFrame =
		imgsensor_info.hs_video_delay_frame;

	sensor_info->SlimVideoDelayFrame =
		imgsensor_info.slim_video_delay_frame;
	sensor_info->Custom1DelayFrame = imgsensor_info.custom1_delay_frame;	
	sensor_info->Custom2DelayFrame = imgsensor_info.custom2_delay_frame;
	sensor_info->Custom3DelayFrame = imgsensor_info.custom3_delay_frame;
	sensor_info->Custom4DelayFrame = imgsensor_info.custom4_delay_frame;
	sensor_info->Custom5DelayFrame = imgsensor_info.custom5_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0;	/* not use */
	sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

	/* The frame of setting shutter default 0 for TG int */
	sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame;

	/* The frame of setting sensor gain*/
	sensor_info->AESensorGainDelayFrame =
				imgsensor_info.ae_sensor_gain_delay_frame;

	sensor_info->AEISPGainDelayFrame =
				imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->FrameTimeDelayFrame =
		imgsensor_info.frame_time_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;

	/* change pdaf support mode to pdaf VC mode */
	sensor_info->PDAF_Support = PDAF_SUPPORT_CAMSV;
	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3;	/* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2;	/* not use */
	sensor_info->SensorPixelClockCount = 3;	/* not use */
	sensor_info->SensorDataLatchCount = 2;	/* not use */

	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;	/* 0 is default 1x */
	sensor_info->SensorHightSampling = 0;	/* 0 is default 1x */
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

		break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.cap.mipi_data_lp2hs_settle_dc;

		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

		sensor_info->SensorGrabStartX =
			imgsensor_info.normal_video.startx;
		sensor_info->SensorGrabStartY =
			imgsensor_info.normal_video.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

		break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;

		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
		sensor_info->SensorGrabStartX =
			imgsensor_info.slim_video.startx;
		sensor_info->SensorGrabStartY =
			imgsensor_info.slim_video.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
			imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;

		break;
	    case MSDK_SCENARIO_ID_CUSTOM1:
	        sensor_info->SensorGrabStartX = imgsensor_info.custom1.startx; 
	        sensor_info->SensorGrabStartY = imgsensor_info.custom1.starty;

	        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom1.mipi_data_lp2hs_settle_dc; 

	        break;
	    case MSDK_SCENARIO_ID_CUSTOM2:
	        sensor_info->SensorGrabStartX = imgsensor_info.custom2.startx; 
	        sensor_info->SensorGrabStartY = imgsensor_info.custom2.starty;

	        sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom2.mipi_data_lp2hs_settle_dc; 

	        break;   
	        
	     case MSDK_SCENARIO_ID_CUSTOM3:                                                                                    
	         sensor_info->SensorGrabStartX = imgsensor_info.custom3.startx;                                                 
	         sensor_info->SensorGrabStartY = imgsensor_info.custom3.starty;                                                 
	                                                                                                                        
	         sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom3.mipi_data_lp2hs_settle_dc;      
	                                                                                                                        
	         break;                                                                                                         
	 	 case MSDK_SCENARIO_ID_CUSTOM4: 																				   
		      sensor_info->SensorGrabStartX = imgsensor_info.custom4.startx; 												
		      sensor_info->SensorGrabStartY = imgsensor_info.custom4.starty; 												
																														
		      sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom4.mipi_data_lp2hs_settle_dc;		
																														
		     break;
		 case MSDK_SCENARIO_ID_CUSTOM5: 																				   
		      sensor_info->SensorGrabStartX = imgsensor_info.custom5.startx; 												
		      sensor_info->SensorGrabStartY = imgsensor_info.custom5.starty; 												
																														
		      sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom5.mipi_data_lp2hs_settle_dc;		
																														
		     break;  																										
	        
	default:
		sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
		sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

		sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
		break;
	}

	return ERROR_NONE;
}				/*      get_info  */


static kal_uint32 control(enum MSDK_SCENARIO_ID_ENUM scenario_id,
			  MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
			  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);
#ifdef CONFIG_MTK_CAM_PD2102F_EX
	pr_err("ov64b call scenario_id = %d\n", scenario_id);
#endif
	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		preview(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		capture(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		normal_video(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		hs_video(image_window, sensor_config_data);
		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
		slim_video(image_window, sensor_config_data);
		break;
	    case MSDK_SCENARIO_ID_CUSTOM1:
	        Custom1(image_window, sensor_config_data);
	        break;
	    case MSDK_SCENARIO_ID_CUSTOM2:
	        Custom2(image_window, sensor_config_data);
	        break;  
	    case MSDK_SCENARIO_ID_CUSTOM3:                     
	        Custom3(image_window, sensor_config_data);      
	        break;  
	   case MSDK_SCENARIO_ID_CUSTOM4:					   
			Custom4(image_window, sensor_config_data);		
			break;
	   case MSDK_SCENARIO_ID_CUSTOM5:					   
			Custom5(image_window, sensor_config_data);		
			break;		
	default:
		pr_debinfo("Error ScenarioId setting");
		preview(image_window, sensor_config_data);
		return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}				/* control() */

static kal_uint32 set_video_mode(UINT16 framerate)
{
	/* //printk("framerate = %d\n ", framerate); */
	/* SetVideoMode Function should fix framerate */
	if (framerate == 0)
		/* Dynamic frame rate */
		return ERROR_NONE;
	spin_lock(&imgsensor_drv_lock);
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = framerate;
	spin_unlock(&imgsensor_drv_lock);
	set_max_framerate(imgsensor.current_fps, 1);

	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(
	kal_bool enable, UINT16 framerate)
{
	pr_debinfo("enable = %d, framerate = %d\n", enable, framerate);
	spin_lock(&imgsensor_drv_lock);
	if (enable)		/* enable auto flicker */
		imgsensor.autoflicker_en = KAL_TRUE;
	else			/* Cancel Auto flick */
		imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario(
	enum MSDK_SCENARIO_ID_ENUM scenario_id,	MUINT32 framerate)
{
	kal_uint32 frame_length;

	pr_debinfo("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		frame_length = imgsensor_info.pre.pclk
			/ framerate * 10 / imgsensor_info.pre.linelength;
		spin_lock(&imgsensor_drv_lock);

		imgsensor.dummy_line =
			(frame_length > imgsensor_info.pre.framelength)
			? (frame_length - imgsensor_info.pre.framelength) : 0;

		imgsensor.frame_length =
			imgsensor_info.pre.framelength + imgsensor.dummy_line;

		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		else {
			/*No need to set*/
			printk("frame_length %d < shutter %d",
				imgsensor.frame_length, imgsensor.shutter);
		}
		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		if (framerate == 0)
			return ERROR_NONE;
		frame_length = imgsensor_info.normal_video.pclk
		    / framerate * 10 / imgsensor_info.normal_video.linelength;

		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
	    (frame_length > imgsensor_info.normal_video.framelength)
	  ? (frame_length - imgsensor_info.normal_video.  framelength) : 0;

		imgsensor.frame_length =
		 imgsensor_info.normal_video.framelength + imgsensor.dummy_line;

		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		else {
			/*No need to set*/
			pr_debinfo("frame_length %d < shutter %d",
				imgsensor.frame_length, imgsensor.shutter);
		}
		break;

	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
		printk("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",framerate,imgsensor_info.cap.max_framerate/10);
		frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
		imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
	    set_dummy();
	    break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		frame_length = imgsensor_info.hs_video.pclk
			/ framerate * 10 / imgsensor_info.hs_video.linelength;
		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
		  (frame_length > imgsensor_info.hs_video.framelength)
		? (frame_length - imgsensor_info.hs_video.  framelength) : 0;

		imgsensor.frame_length =
		    imgsensor_info.hs_video.framelength + imgsensor.dummy_line;

		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		else {
			/*No need to set*/
			pr_debinfo("frame_length %d < shutter %d",
				imgsensor.frame_length, imgsensor.shutter);
		}
		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
		frame_length = imgsensor_info.slim_video.pclk
			/ framerate * 10 / imgsensor_info.slim_video.linelength;

		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
		  (frame_length > imgsensor_info.slim_video.framelength)
		? (frame_length - imgsensor_info.slim_video.  framelength) : 0;

		imgsensor.frame_length =
		  imgsensor_info.slim_video.framelength + imgsensor.dummy_line;

		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		else {
			/*No need to set*/
			pr_debinfo("frame_length %d < shutter %d",
				imgsensor.frame_length, imgsensor.shutter);
		}
		break;
	case MSDK_SCENARIO_ID_CUSTOM1:
		frame_length = imgsensor_info.custom1.pclk / framerate * 10 / imgsensor_info.custom1.linelength;
		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line = (frame_length > imgsensor_info.custom1.framelength) ? (frame_length - imgsensor_info.custom1.framelength) : 0;
		imgsensor.frame_length = imgsensor_info.custom1.framelength + imgsensor.dummy_line;
		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
		set_dummy();
		break;
	case MSDK_SCENARIO_ID_CUSTOM2:
		frame_length = imgsensor_info.custom2.pclk / framerate * 10 / imgsensor_info.custom2.linelength;
		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line = (frame_length > imgsensor_info.custom2.framelength) ? (frame_length - imgsensor_info.custom2.framelength) : 0;
		imgsensor.frame_length = imgsensor_info.custom2.framelength + imgsensor.dummy_line;
		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		break;  
	case MSDK_SCENARIO_ID_CUSTOM3:                                                                                                              
		frame_length = imgsensor_info.custom3.pclk / framerate * 10 / imgsensor_info.custom3.linelength;                                          	
		spin_lock(&imgsensor_drv_lock);                                                                                                           	
		imgsensor.dummy_line = (frame_length > imgsensor_info.custom3.framelength) ? (frame_length - imgsensor_info.custom3.framelength) : 0;     	
		imgsensor.frame_length = imgsensor_info.custom3.framelength + imgsensor.dummy_line;                                                       	
		imgsensor.min_frame_length = imgsensor.frame_length;                                                                                      	
		spin_unlock(&imgsensor_drv_lock);                                                                                                         	
		if (imgsensor.frame_length > imgsensor.shutter)                                                                                           	
			set_dummy();                                                                                                                            	
		break;   
	
	case MSDK_SCENARIO_ID_CUSTOM4:                                                                                                              
		frame_length = imgsensor_info.custom4.pclk / framerate * 10 / imgsensor_info.custom4.linelength;                                          	
		spin_lock(&imgsensor_drv_lock);                                                                                                           	
		imgsensor.dummy_line = (frame_length > imgsensor_info.custom4.framelength) ? (frame_length - imgsensor_info.custom4.framelength) : 0;     	
		imgsensor.frame_length = imgsensor_info.custom4.framelength + imgsensor.dummy_line;                                                       	
		imgsensor.min_frame_length = imgsensor.frame_length;                                                                                      	
		spin_unlock(&imgsensor_drv_lock);                                                                                                         	
		if (imgsensor.frame_length > imgsensor.shutter)                                                                                           	
			set_dummy();                                                                                                                            	
		break;
	case MSDK_SCENARIO_ID_CUSTOM5:                                                                                                              
		frame_length = imgsensor_info.custom5.pclk / framerate * 10 / imgsensor_info.custom5.linelength;                                          	
		spin_lock(&imgsensor_drv_lock);                                                                                                           	
		imgsensor.dummy_line = (frame_length > imgsensor_info.custom5.framelength) ? (frame_length - imgsensor_info.custom5.framelength) : 0;     	
		imgsensor.frame_length = imgsensor_info.custom5.framelength + imgsensor.dummy_line;                                                       	
		imgsensor.min_frame_length = imgsensor.frame_length;                                                                                      	
		spin_unlock(&imgsensor_drv_lock);                                                                                                         	
		if (imgsensor.frame_length > imgsensor.shutter)                                                                                           	
			set_dummy();                                                                                                                            	
		break;                                                                                                                                     	
	default:		/* coding with  preview scenario by default */
		frame_length = imgsensor_info.pre.pclk
			/ framerate * 10 / imgsensor_info.pre.linelength;

		spin_lock(&imgsensor_drv_lock);
		imgsensor.dummy_line =
			(frame_length > imgsensor_info.pre.framelength)
			? (frame_length - imgsensor_info.pre.framelength) : 0;

		imgsensor.frame_length =
			imgsensor_info.pre.framelength + imgsensor.dummy_line;

		imgsensor.min_frame_length = imgsensor.frame_length;
		spin_unlock(&imgsensor_drv_lock);
		if (imgsensor.frame_length > imgsensor.shutter)
			set_dummy();
		else {
			/*No need to set*/
			printk("frame_length %d < shutter %d",
				imgsensor.frame_length, imgsensor.shutter);
		}
		pr_debinfo("error scenario_id = %d, we use preview scenario\n",
		scenario_id);
		break;
	}
	return ERROR_NONE;
}


static kal_uint32 get_default_framerate_by_scenario(
	enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate)
{
	/*printk("scenario_id = %d\n", scenario_id);*/

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		*framerate = imgsensor_info.pre.max_framerate;
		break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		*framerate = imgsensor_info.normal_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		*framerate = imgsensor_info.cap.max_framerate;
		break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		*framerate = imgsensor_info.hs_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
		*framerate = imgsensor_info.slim_video.max_framerate;
		break;
	case MSDK_SCENARIO_ID_CUSTOM1:
	    *framerate = imgsensor_info.custom1.max_framerate;
	    break;
	case MSDK_SCENARIO_ID_CUSTOM2:
	    *framerate = imgsensor_info.custom2.max_framerate;
	    break;   
	case MSDK_SCENARIO_ID_CUSTOM3:                              
	    *framerate = imgsensor_info.custom3.max_framerate;       
	    break;  
	case MSDK_SCENARIO_ID_CUSTOM4:                              
	    *framerate = imgsensor_info.custom4.max_framerate;       
	    break;
	case MSDK_SCENARIO_ID_CUSTOM5:                              
	    *framerate = imgsensor_info.custom5.max_framerate;       
	    break;                                               
	default:
		break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
	pr_debinfo("enable: %d\n", enable);

	if (enable) {
/* 0 : Normal, 1 : Solid Color, 2 : Color Bar, 3 : Shade Color Bar, 4 : PN9 */
		write_cmos_sensor_16_8(0x50c1, 0x01);
	} else {
		write_cmos_sensor_16_8(0x50c1, 0x00);
	}
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}
static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
				  UINT8 *feature_para, UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16 = (UINT16 *) feature_para;
	UINT16 *feature_data_16 = (UINT16 *) feature_para;
	UINT32 *feature_return_para_32 = (UINT32 *) feature_para;
	UINT32 *feature_data_32 = (UINT32 *) feature_para;
	//INT32 *feature_return_para_i32 = (INT32 *) feature_para;
	unsigned long long *feature_data = (unsigned long long *)feature_para;

	struct SET_PD_BLOCK_INFO_T *PDAFinfo;
	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
   	struct SENSOR_VC_INFO_STRUCT *pvcinfo;
	struct SENSOR_RAWINFO_STRUCT *rawinfo;
	MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data =
		(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

	/*printk("feature_id = %d\n", feature_id);*/
	switch (feature_id) {
	case SENSOR_FEATURE_GET_GAIN_RANGE_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_gain;
		*(feature_data + 2) = imgsensor_info.max_gain;
		break;
	case SENSOR_FEATURE_GET_BASE_GAIN_ISO_AND_STEP:
		*(feature_data + 0) = imgsensor_info.min_gain_iso;
		*(feature_data + 1) = imgsensor_info.gain_step;
		*(feature_data + 2) = imgsensor_info.gain_type;
		break;
	case SENSOR_FEATURE_GET_MIN_SHUTTER_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_shutter;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ_BY_SCENARIO:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.cap.pclk;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.normal_video.pclk;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.hs_video.pclk;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.slim_video.pclk;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.custom1.pclk;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.custom2.pclk;
			break;
		case MSDK_SCENARIO_ID_CUSTOM3:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.custom3.pclk;
			break;
		case MSDK_SCENARIO_ID_CUSTOM4:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.custom4.pclk;
			break;
		case MSDK_SCENARIO_ID_CUSTOM5:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.custom5.pclk;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.pre.pclk;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_PERIOD_BY_SCENARIO:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.cap.framelength << 16)
				+ imgsensor_info.cap.linelength;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.normal_video.framelength << 16)
				+ imgsensor_info.normal_video.linelength;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.hs_video.framelength << 16)
				+ imgsensor_info.hs_video.linelength;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.slim_video.framelength << 16)
				+ imgsensor_info.slim_video.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom1.framelength << 16)
				+ imgsensor_info.custom1.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom2.framelength << 16)
				+ imgsensor_info.custom2.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM3:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom3.framelength << 16)
				+ imgsensor_info.custom3.linelength;
			break;
		
		case MSDK_SCENARIO_ID_CUSTOM4:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom4.framelength << 16)
				+ imgsensor_info.custom4.linelength;
			break;
		case MSDK_SCENARIO_ID_CUSTOM5:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.custom5.framelength << 16)
				+ imgsensor_info.custom5.linelength;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.pre.framelength << 16)
				+ imgsensor_info.pre.linelength;
			break;
		}
		break;

	case SENSOR_FEATURE_GET_PERIOD:
		*feature_return_para_16++ = imgsensor.line_length;
		*feature_return_para_16 = imgsensor.frame_length;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
#if 0
		printk(
			"feature_Control imgsensor.pclk = %d,imgsensor.current_fps = %d\n",
			imgsensor.pclk, imgsensor.current_fps);
#endif
		*feature_return_para_32 = imgsensor.pclk;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_ESHUTTER:
		set_shutter(*feature_data);
		break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
	/* night_mode((BOOL) *feature_data); no need to implement this mode */
		break;
	case SENSOR_FEATURE_SET_GAIN:
		set_gain((UINT16) *feature_data);
		break;
	case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		break;

	case SENSOR_FEATURE_SET_REGISTER:
		write_cmos_sensor_16_8(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
		break;

	case SENSOR_FEATURE_GET_REGISTER:
		sensor_reg_data->RegData = read_cmos_sensor_16_8(sensor_reg_data->RegAddr);
		break;

	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
		/* get the lens driver ID from EEPROM or
		 * just return LENS_DRIVER_ID_DO_NOT_CARE
		 */
		/* if EEPROM does not exist in camera module. */
		*feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:
		set_video_mode(*feature_data);
		break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
		get_imgsensor_id(feature_return_para_32);
		break;
	case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
		set_auto_flicker_mode((BOOL) (*feature_data_16),
					*(feature_data_16 + 1));
		break;
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
		set_max_framerate_by_scenario(
	    (enum MSDK_SCENARIO_ID_ENUM) *feature_data, *(feature_data + 1));
		break;

	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
		get_default_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM) *(feature_data),
			  (MUINT32 *) (uintptr_t) (*(feature_data + 1)));
		break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
		set_test_pattern_mode((BOOL) (*feature_data));
		break;

	/* for factory mode auto testing */
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
		*feature_return_para_32 = imgsensor_info.checksum_value;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_FRAMERATE:
		printk("current fps :%d\n", *feature_data_32);
		spin_lock(&imgsensor_drv_lock);
		imgsensor.current_fps = (UINT16)*feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
		break;
	case SENSOR_FEATURE_GET_BINNING_TYPE:	
		switch (*(feature_data + 1)) {	/*2sum = 2; 4sum = 4; 4avg = 1 not 4cell sensor is 4avg*/
			
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		case MSDK_SCENARIO_ID_CUSTOM1:
		case MSDK_SCENARIO_ID_CUSTOM2:
			*feature_return_para_32 = 2;	/*BINNING_AVERAGED 2sum2avg*/
			break;
		
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
		case MSDK_SCENARIO_ID_CUSTOM3:
		case MSDK_SCENARIO_ID_CUSTOM4:
			*feature_return_para_32 = 2; /*4sum*/
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		default:
			*feature_return_para_32 = 1; /*BINNING_NONE,*/ 
			break;
		}
		printk("SENSOR_FEATURE_GET_BINNING_TYPE AE_binning_type:%d,\n",
			*feature_return_para_32);
		*feature_para_len = 4;
		break;

		case SENSOR_FEATURE_GET_RAW_INFO:
		pr_debug("SENSOR_FEATURE_GET_RAW_INFO scenarioId:%d\n",
			(UINT32) *feature_data);
		rawinfo = (struct SENSOR_RAWINFO_STRUCT *) (uintptr_t) (*(feature_data + 1));
		memcpy((void *)rawinfo,
				(void *)&imgsensor_raw_info,
				sizeof(struct SENSOR_RAWINFO_STRUCT));
		break;


		/* PK_DBG("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n",
		 *	(UINT32) *feature_data);
		 */
	case SENSOR_FEATURE_GET_CROP_INFO:
		/* printk("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n",
		 *	(UINT32) *feature_data);
		 */

		wininfo =
	(struct SENSOR_WINSIZE_INFO_STRUCT *) (uintptr_t) (*(feature_data + 1));

		switch (*feature_data_32) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			memcpy((void *)wininfo, (void *)&imgsensor_winsize_info[1], sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			memcpy((void *)wininfo, (void *)&imgsensor_winsize_info[2], sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			memcpy((void *)wininfo, (void *)&imgsensor_winsize_info[3], sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			memcpy((void *)wininfo, (void *)&imgsensor_winsize_info[4], sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		    case MSDK_SCENARIO_ID_CUSTOM1:
		      memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[5],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		      break;
		    case MSDK_SCENARIO_ID_CUSTOM2:
		      memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[6],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
		      break; 
		    case MSDK_SCENARIO_ID_CUSTOM3:                                                                           
		      memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[7],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));       
		     break; 
			case MSDK_SCENARIO_ID_CUSTOM4:																			 
			  memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[8],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT)); 	  
			 break;
			case MSDK_SCENARIO_ID_CUSTOM5:																			 
			  memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[9],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT)); 	  
			 break;  																							  
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			memcpy((void *)wininfo, (void *)&imgsensor_winsize_info[0], sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		}
					break;

	case SENSOR_FEATURE_GET_PDAF_INFO:
		printk("SENSOR_FEATURE_GET_PDAF_INFO scenarioId:%lld\n", *feature_data);
		PDAFinfo= (struct SET_PD_BLOCK_INFO_T *)(uintptr_t)(*(feature_data+1));

		switch (*feature_data) {
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			case MSDK_SCENARIO_ID_CUSTOM1:
			case MSDK_SCENARIO_ID_CUSTOM5:
				imgsensor_pd_info.i4BlockNumX = 254;//4080x3060
				imgsensor_pd_info.i4BlockNumY = 190;
			    memcpy((void *)PDAFinfo,(void *)&imgsensor_pd_info,sizeof(struct SET_PD_BLOCK_INFO_T));
				break;
            case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				imgsensor_pd_info.i4BlockNumX = 254;//4080x2296
				imgsensor_pd_info.i4BlockNumY = 143;
			    memcpy((void *)PDAFinfo,(void *)&imgsensor_pd_info,sizeof(struct SET_PD_BLOCK_INFO_T));
				break;



			
			default:
				break;
		}
		break;


	case SENSOR_FEATURE_GET_SENSOR_PDAF_CAPACITY:
		printk("SENSOR_FEATURE_GET_SENSOR_PDAF_CAPACITY scenarioId:%lld\n", *feature_data);
		/*PDAF capacity enable or not, 2p8 only full size support PDAF*/
		switch (*feature_data) {
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
				break;
			case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1; /* video & capture use same setting*/
				break;
			case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_SLIM_VIDEO:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
				break;
		    case MSDK_SCENARIO_ID_CUSTOM1:
			    *(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
			    break;
			case MSDK_SCENARIO_ID_CUSTOM2:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_CUSTOM3:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_CUSTOM4:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
			case MSDK_SCENARIO_ID_CUSTOM5:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 1;
				break;

			default:
				*(MUINT32 *)(uintptr_t)(*(feature_data+1)) = 0;
				break;
		}
		break;

/*
    case SENSOR_FEATURE_SET_PDAF:
        printk("PDAF mode :%d\n", *feature_data_16);
        imgsensor.pdaf_mode= *feature_data_16;
        break;
    */
    case SENSOR_FEATURE_GET_VC_INFO:
        printk("SENSOR_FEATURE_GET_VC_INFO %d\n", (UINT16)*feature_data);
        pvcinfo = (struct SENSOR_VC_INFO_STRUCT *)(uintptr_t)(*(feature_data+1));
		switch (*feature_data_32) {
		
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_CUSTOM1:
		case MSDK_SCENARIO_ID_CUSTOM5:
				memcpy((void *)pvcinfo, (void *) &SENSOR_VC_INFO[0], sizeof(struct SENSOR_VC_INFO_STRUCT));
		    break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		    memcpy((void *)pvcinfo, (void *)&SENSOR_VC_INFO[1], sizeof(struct SENSOR_VC_INFO_STRUCT));
		    break;

		default:	   
		    break;
		}
		break;  

		

	case SENSOR_FEATURE_GET_CUSTOM_INFO:
	    printk("SENSOR_FEATURE_GET_CUSTOM_INFO information type:%lld  OV50C40PD2164_OTP_ERROR_CODE:%d \n", *feature_data,OV50C40PD2164_OTP_ERROR_CODE);
		switch (*feature_data) {
			case 0:    //info type: otp state
			printk("*feature_para_len = %d, sizeof(MUINT32)*13 + 2 =%ld, \n", *feature_para_len, sizeof(MUINT32)*13 + 2);
			if (*feature_para_len >= sizeof(MUINT32)*13 + 2) {
			    *(MUINT32 *)(uintptr_t)(*(feature_data+1)) = OV50C40PD2164_OTP_ERROR_CODE;//otp_state
				memcpy( feature_data+2, sn_inf_main_ov50c40pd2164, sizeof(MUINT32)*13); 
				memcpy( feature_data+10, material_inf_main_ov50c40pd2164, sizeof(MUINT32)*4);
				memcpy( feature_data+12, af_calib_inf_main_ov50c40pd2164, sizeof(MUINT32)*6); 
				#if 0
						for (i = 0 ; i<12 ; i++ ){
						printk("sn_inf_main_ov50c40pd2164[%d]= 0x%x\n", i, sn_inf_main_ov50c40pd2164[i]);
						}
				#endif
			}
				break;
			}
			break;


	case SENSOR_FEATURE_SET_SHUTTER_FRAME_TIME:
		set_shutter_frame_length((UINT16) *feature_data, (UINT16) *(feature_data + 1));
		break;
	case SENSOR_FEATURE_GET_FRAME_CTRL_INFO_BY_SCENARIO:
		/*
		 * 1, if driver support new sw frame sync
		 * set_shutter_frame_length() support third para auto_extend_en
		 */
		*(feature_data + 1) = 1;
		/* margin info by scenario */
		*(feature_data + 2) = imgsensor_info.margin;
		break;
	case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
		printk("SENSOR_FEATURE_SET_STREAMING_SUSPEND\n");
		streaming_control(KAL_FALSE);
		break;
	case SENSOR_FEATURE_SET_STREAMING_RESUME:
		printk("SENSOR_FEATURE_SET_STREAMING_RESUME, shutter:%llu\n", *feature_data);
		if (*feature_data != 0)
			set_shutter(*feature_data);
		streaming_control(KAL_TRUE);
		break;
	case SENSOR_FEATURE_GET_AE_FRAME_MODE_FOR_LE:
	printk("SENSOR_FEATURE_GET_AE_FRAME_MODE_FOR_LE\n");
		memcpy(feature_return_para_32,
		&imgsensor.ae_frm_mode, sizeof(struct IMGSENSOR_AE_FRM_MODE));
		break;
	case SENSOR_FEATURE_GET_AE_EFFECTIVE_FRAME_FOR_LE:
		printk("SENSOR_FEATURE_GET_AE_EFFECTIVE_FRAME_FOR_LE\n");
		*feature_return_para_32 =  imgsensor.current_ae_effective_frame;
		break;


	case SENSOR_FEATURE_GET_PIXEL_RATE:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.cap.pclk /
			(imgsensor_info.cap.linelength - 80))*
			imgsensor_info.cap.grabwindow_width;
			break;
			
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.normal_video.pclk /
			(imgsensor_info.normal_video.linelength - 80))*
			imgsensor_info.normal_video.grabwindow_width;
			break;
			
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.hs_video.pclk /
			(imgsensor_info.hs_video.linelength - 80))*
			imgsensor_info.hs_video.grabwindow_width;
			break;
			
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.slim_video.pclk /
			(imgsensor_info.slim_video.linelength - 80))*
			imgsensor_info.slim_video.grabwindow_width;
			break;
		
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.custom1.pclk /
			(imgsensor_info.custom1.linelength - 80))*
			imgsensor_info.custom1.grabwindow_width;
			break;

		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.custom2.pclk /
			(imgsensor_info.custom2.linelength - 80))*
			imgsensor_info.custom2.grabwindow_width;
			break;
			
		case MSDK_SCENARIO_ID_CUSTOM3:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.custom3.pclk /
			(imgsensor_info.custom3.linelength - 80))*
			imgsensor_info.custom3.grabwindow_width;
			break;
	
		case MSDK_SCENARIO_ID_CUSTOM4:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.custom4.pclk /
			(imgsensor_info.custom4.linelength - 80))*
			imgsensor_info.custom4.grabwindow_width;
			break;
		case MSDK_SCENARIO_ID_CUSTOM5:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.custom5.pclk /
			(imgsensor_info.custom5.linelength - 80))*
			imgsensor_info.custom5.grabwindow_width;
			break;
		
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.pre.pclk /
			(imgsensor_info.pre.linelength - 80))*
			imgsensor_info.pre.grabwindow_width;
			break;
		}
		break;

	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.cap.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.normal_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.hs_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.slim_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM1:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.custom1.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM2:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.custom2.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM3:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.custom3.mipi_pixel_rate;
			break;
		
		case MSDK_SCENARIO_ID_CUSTOM4:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.custom4.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CUSTOM5:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.custom5.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.pre.mipi_pixel_rate;
			break;
		}
		break;
	default:
		break;
	}

	return ERROR_NONE;
}				/*      feature_control()  */

static struct SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};

UINT32 OV50C40PD2164_MIPI_RAW_SensorInit(
	struct SENSOR_FUNCTION_STRUCT **pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc != NULL)
		*pfFunc = &sensor_func;
	return ERROR_NONE;
}
