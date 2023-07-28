#ifndef __USER_TM1688_H___
#define __USER_TM1688_H___

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  u8  seg1;
  u8  seg2;
  u8  seg3;
  u8  seg4;
} disp_buff_stru;
//fix display



#define     SEGA 1
#define     SEGB 2
#define     SEGC 4
#define     SEGD 8
#define     SEGE 0X10
#define     SEGF 0X20
#define     SEGG 0X40
#define     SEGH 0X80
#define     ASC_0 48
#define     ASC_a 97
#define     ASC_A 65
#define     d_0     (u8)(SEGA|SEGB|SEGC|SEGD|SEGE|SEGF) //0
#define     d_1     (u8)(SEGB|SEGC) //1
#define     d_2     (u8)(SEGA|SEGB|SEGD|SEGE) //2
#define     d_3     (u8)(SEGA|SEGB|SEGC|SEGD|SEGG) //3
#define     d_4     (u8)(SEGB|SEGC|SEGG|SEGF) //4
#define     d_5     (u8)(SEGA|SEGB|SEGC|SEGD|SEGE|SEGF) //5
#define     d_6     (u8)(SEGA|SEGG|SEGC|SEGD|SEGE|SEGF) //6
#define     d_7     (u8)(SEGA|SEGB|SEGC) //7
#define     d_8     (u8)(SEGA|SEGB|SEGC|SEGD|SEGE|SEGF|SEGG) //8
#define     d_9     (u8)(SEGA|SEGB|SEGC|SEGD|SEGG|SEGF) //9




#define d_s   SEGA|SEGC|SEGD|SEGF|SEGG
#define d_p   SEGA|SEGB|SEGE|SEGF|SEGG
#define d_e   SEGA|SEGC|SEGD|SEGE|SEGF|SEGG
#define d_d   SEGB|SEGC|SEGD|SEGG
#define d_H   SEGB|SEGC|SEGE|SEGF|SEGH
#define d_h   SEGC|SEGE|SEGF|SEGH
#define d_r   SEGE|SEGG
#define d_o   SEGD|SEGC|SEGE|SEGG
#define d_t   SEGD|SEGE|SEGF|SEGG
#define dh_   SEGH
#define d_dot   SEGH
#define d_L   SEGD|SEGE|SEGF
#define d_n   SEGD|SEGB|SEGG
#define d_h1   SEGA
#define d_m1   SEGG
#define d_l1   SEGD
#define d_y   SEGB|SEGC|SEGF|SEGG
#define d_i   SEGB|SEGC
#define d_c   SEGC|SEGD|SEGG
#define d_u   SEGB|SEGC|SEGD|SEGE|SEGF
#define d_v   SEGC|SEGD|SEGE
#define d_R   SEGA|SEGB|SEGC|SEGF|SEGE|SEGG
#define d_M   SEGA|SEGB|SEGC|SEGF|SEGE|SEGG
#define d_f   SEGA|SEGD|SEGE|SEGF|SEGG
#define d_a   SEGA|SEGB|SEGC|SEGE||SEGF|SEGG
#define d_T   SEGA|SEGE||SEGF
#define d_g   SEGA|SEGB|SEGC|SEGD|SEGG|SEGF
#define d_b   SEGC|SEGD|SEGE|SEGG|SEGF
#define d_LAS   1
#define d_PWR   2
#define d_TEACH   4
#define d_ZERO   8
#define d_PRO   0x10
#define join_u32(s1,s2,s3,s4)  (u32)((u32)s1<<24+(u32)s2<<24+(u32)s3<<24+(u32)s4<<24)
typedef enum
{

    err_off =0,//保持OFF
    err_01,//闪存发生损坏，或已到使用寿命
    err_11,//检测输出的负荷短路形成的过大电流
    err_21,//半导体激光发生损坏，或者已到使用寿命
    err_31,//调零时，未能正常测量  由于显示设定设为偏移，因此不能使用调零功能。
    err_41,//执行教导时，未能正常测量
    err_90,//系统错误
    err_91,//系统错误
    err_92,//系统错误
    err_93//系统错误
 } error_selection_type;   

 
//const fix_disp_out_stru disp_tbl[]={
//    
//    { {d_t,d_c,d_H,d_0|d_dot},
//      {d_t,d_c,d_H,d_1|d_dot},
//      {d_t,d_c,d_H,d_3|d_dot},
//      {d_t,d_c,d_H,d_4|d_dot},
//      {d_t,d_c,d_H,d_5|d_dot}
//    },
//    {d_H,d_a,d_r,d_d},
//    {d_g,d_o,d_o,d_d},


//     {{SEGG,SEGG,SEGG,SEGG},
//     {d_e,d_r,d_0,d_1},//闪存发生损坏，或已到使用寿命
//     {d_e,d_r,d_1,d_1},//检测输出的负荷短路形成的过大电流
//     {d_e,d_r,d_2,d_1},//半导体激光发生损坏，或者已到使用寿命
//     {d_e,d_r,d_3,d_1},//调零时，未能正常测量  由于显示设定设为偏移，因此不能使用调零功能。
//     {d_e,d_r,d_4,d_1},//执行教导时，未能正常测量
//     {d_e,d_r,d_9,d_0},//系统错误
//     {d_e,d_r,d_9,d_1},//系统错误
//     {d_e,d_r,d_9,d_2},//系统错误
//     {d_e,d_r,d_9,d_3}//系统错误
//     }
//};








void disp_init();
u8 read_key(void);
void disp_out(app_stru *p);

/*******************************************/
#ifdef __cplusplus
}
#endif

#endif