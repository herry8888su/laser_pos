#ifndef __LASER_ADC_H___
#define __LASER_ADC_H___

#ifdef __cplusplus
extern "C" {
#endif
#define DAC_VOLT    (u32)(1900 * (3300 + 1000) / 3300)
#define DAC_VAL_H   (u32)(4095 * DAC_VOLT / 3300)
#define DAC_DVOLT   (u32)(400 * (3300 + 1000) / 3300)
#define DAC_DVAL    (u32)(4095 * DAC_DVOLT / 3300)
#define DAC_VAL_L   (u32)(DAC_VAL_H - DAC_DVAL)
#define DV_400MV    (u32)( 400*4095/3300)
typedef enum
{
    cmos_st_level 	= 0x00000001,  // st out enable
    laser_sw_en 		= 0x00000002,    // st out enable
    laser_pwm_level = 0x00000004 // st out enable
}laser_cmos_type;
#define s_sum 512 * 12.5 * 10
typedef struct
{
    s16 max_n;
    s16 mean;

    u32 stat;
    u16 dacval;
    s16 distance;
} laser_cmos_stru;

void laser_Init(void);
/*
laser_cmos_stru *pLaser=(laser_cmos_stru*)open_laser_calc();
*/
u8* open_laser_calc(void);
/*******************************************/
#ifdef __cplusplus
}
#endif

#endif