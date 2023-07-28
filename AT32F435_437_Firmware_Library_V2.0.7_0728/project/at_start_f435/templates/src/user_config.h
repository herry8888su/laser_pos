/*
测量中心距离: 50mm
测量范围: ±4mm
重复精度: 20μm
光源:最大输出：0.2mW、发光光束波长：655nm;红色半导体激光　1类(JIS/IEC/GB）
光束直径:约ø150μm
电源电压:12V～24V DC±10%　脉动P-P10%
消耗电流:40mA以下(电源电压24V DC时)、65mA以下(电源电压12V DC时)
*/
/*
控制输出:
1.NPN输出型:NPN开路集电极晶体管
            • 最大流入电流：50mA
            • 外加电压：30V DC以下
            (控制输出-0V之间)
            • 剩余电压：1.5V以下
            (流入电流50mA下)
            • 漏电流 ：0.1mA以下
2.PNP输出型: PNP开路集电极晶体管
            • 最大源电流：50mA
            • 外加电压：30V DC以下
            (控制输出-＋V之间)
            • 剩余电压：1.5V以下
            (流出电流50mA下)
            • 漏电流 ：0.1mA以下
输出动作: 入光时ON/非入光时ON　可切换
短路保护: 配备(自动恢复型)
*/
/*----------------------------------------------------------------
模拟输出:
电压:
• 输出范围：0V～5V(警报时：＋5.2V)
• 输出阻抗：100Ω
电流:
• 输出范围：4mA～20mA(警报时：0mA)
• 负载阻抗：300Ω 最大

*/
/*
反应时间:1.5ms/5ms/10ms　可切换

*/
/*
外部输入:
＜NPN输出型＞
NPN无接点输入
• 输入条件
无效：＋8V～＋V DC或者开放
有效：0V～＋1.2V DC
• 输入阻抗：约10kΩ
＜PNP输出型＞
PNP无接点输入
• 输入条件
无效：0V～＋0.6V DC或者开放
有效：＋4V～＋V DC
• 输入阻抗：约10kΩ


*/
#ifndef __USER_CONFIG_H___
#define __USER_CONFIG_H___

#ifdef __cplusplus
extern "C" {
#endif
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

/***********user define**********************/
#define u8   uint8_t
#define u16   uint16_t
#define u32   uint32_t
#define s8   int8_t
#define s16   int16_t
#define s32   int32_t
#define MODE_NONE 0
typedef enum
{
  KEY_NONE                          = 0X00,
  KEY_TEACH                         = 0x01, /*!< usart no parity */
  KEY_UP                            = 0x02, /*!< usart even parity */
  KEY_DOWN                          = 0x04  /*!< usart odd parity */
} key_selection_type;
typedef enum
{
  DELT_10MS                               = 100, /*time base is 100us;10ms */
  DELT_5MS                                = 50, /*5ms */
  DELT_1MS5                               = 15  /*1.5ms*/
} deltt_mode_selection_type;
typedef enum
{

  TEACH_MODE_NORM  = 0, //通常检测模式
  TEACH_MODE_1DOT,  //  1点教导(窗口比较模式)               
  TEACH_MODE_2DOT,  //      2点教导(窗口比较模式)           
  TEACH_MODE_3DOT,//3点教导(窗口比较模式)
  TEACH_MODE_UP_DIFF,     // 上升微分模式                
  TEACH_MODE_DOWN_DIFF,    //   下降微分模式  
  TEACH_MODE_TERMINAL             
} teach_mode_selection_type;
typedef enum
{

  SIMOUT_MODE_VOLT                         = 0x00, /*V.out”:电压输出（0V-5V） */
  SIMOUT_MODE_CURR                           /*I.out”:电流输出（4mA-20mA） */
  
} simout_mode_selection_type;  //模拟输出设定
typedef enum
{
  DELTS_MODE_MIN                         = 10, /*S BASE 10UM */
  DELTS_MODE_MAX                          = 5000 /*I.out”:电流输出（4mA-20mA） */
  
} DELTS_mode_selection_type;   //设定应差
typedef enum
{
  KEY_UNLOCK                         = 0, /*S BASE 10UM */
  KEY_LOCK                  /*I.out”:电流输出（4mA-20mA） */
  
} key_lock_selection_type;   //设定应差
typedef enum
{
 
  EXTIN_MODE_ADJZERO                         = 0, /*调零功能 */
  EXTIN_MODE_TEACH                         , /*教导 */
  EXTIN_MODE_STOP_R                         , /*停止投光功能 */
  EXTIN_MODE_TRIG                          /*触发功能 */
  
} extin_mode_selection_type;    //设定外部输入
typedef enum
{
 
  EXTIN_MODE_NO_T                        = 0, /*无定时 */
  EXTIN_MODE_DE_OFF                        , /*延迟断开*/
  EXTIN_MODE_DE_ACTION                     , /*延时动作*/
  EXTIN_MODE_SINGLE_T                         /*单次定时 */
  
} timer_mode_selection_type;    //设定定时器的动作。定时时间固定设为5ms。
typedef enum
{
 
  EXCH_DISP_MODE_NORM                 = 0, /*通常 */
  EXCH_DISP_MODE_R                       , /*反转*/
  EXCH_DISP_MODE_OFFSET                  /*偏移*/
} exch_disp_selection_type;    //可切换测量值的显示
typedef enum
{
 
  HOLD_OUT_MODE_OFF             = 0, /*保持OFF */
  HOLD_OUT_MODE_ON                   /*保持ON*/
} hold_out_selection_type;    //对发生测量错误（受光量不足、光量饱和、测量范围外）时的控制输出和模拟输出动作进行设定
typedef enum
{
 
  POWER_DOWN_MODE_OFF                        = 0, /*消耗电流OFF */
  POWER_DOWN_MODE_ON                        /*消耗电流ON*/
} power_down_selection_type;    //30秒内如未操作按键，则可使数字显示部分熄灯。可控制消耗电流
typedef enum
{
 
  RESET_MODE_NG                        = 0, /*复位NG */
  RESET_MODE_OK                        /*复位OK*/
} reset_selection_type;    //恢复至初始状态（出厂状态）
typedef enum
{
 
  EXTIN_SAVE_MODE_OFF                        = 0, /*内存保存无效*/
  EXTIN_SAVE_MODE_ON                        /*内存保存有效*/
} extin_save_selection_type;    //根据外部输入，调零的内存保存

typedef enum
{
 
  NONE_KEY_EVT                        = 0,
  DOWN_KEY_EVT                  =1,
  UP_KEY_EVT                    =2,
  TEACH_KEY_EVT                 =4,
  DOWN_3S_KEY_EVT               =8,
  TEACH_DOWN_3S_KEY_EVT         =0x10,
  TEACH_UP_3S_KEY_EVT           =0x20,
  UP_DOWN_3S_KEY_EVT            =0x40 , 
  TEACH_1S_KEY_EVT              =0x80,
  EXT_KEY_EVT                   =0x0100

} key_event_selection_type;    //30秒内如未操作按键，则可使数字显示部分熄灯。可控制消耗电流
typedef enum
{
  disp_Normal                         =  0X00,
  disp_teach1,
  disp_teach2,
  disp_teach3,
  disp_teach4,
  disp_good,
  disp_hard,
  disp_er41,
  
  //上升微分模式或下降微分模式的跨距调整
	disp_diff,
	disp_base,  //基准值微调功能
	disp_base_1sl,
	disp_base_2sl,
	 
	disp_peak,//峰值、谷值保持功能
	disp_h_hold,
	disp_b_hold,
	
	disp_lock,//按键锁定功能
	disp_lock_on,
	disp_lock_off,

  disp_sped,  //设定反应时间
  disp_10ms,
  disp_5ms,
  disp_1ms5,
  
  disp_L_d,  //输出动作设定
  disp_L_on,
  disp_d_on,

  disp_sens,//设定检测输出
  disp_sens_norm,
  disp_sens_teach1,
  disp_sens_teach2,
  disp_sens_teach3,
  disp_sens_raising,
  disp_sens_falling,

  disp_simout,//模拟输出设定
  disp_vout,
  disp_iout,

  disp_hys,  //设定应差
  disp_hys1,

  disp_extin,  //设定外部输入
  disp_zero_ex,
  disp_teach_ex,
  disp_L_ex,
  disp_trig_ex,
  disp_save_ex,
  disp_on_ex,
  disp_off_ex,

  disp_delay,//设定定时器
  disp_delay_no,
  disp_delay_off,
  disp_delay_action,
  disp_delay_single,


  disp_disp,//设定显示
  disp_disp_std,
  disp_disp_inv,
  disp_disp_offset,

  disp_hold,  //设定保持
  disp_hold_on,
  disp_hold_off,

  disp_eco,   //环保设定
  disp_eco_on,
  disp_eco_off,

  disp_reset, //恢复至初始状态
  disp_reset_no,
  disp_reset_yes

} dispout_tbl_selection_type;


typedef enum
{
  STAT_TEST                         =  0X00,
  STAT_TEACH,
  STAT_BASE_ADJ,
  STAT_PEAK_HOLD,
  STAT_ZERO,
  STAT_KEY_LOCK,
  STAT_BASE_CHK,
  STAT_PRO,
  MAIN_STAT_TERMINAL
} MAIN_STAT_selection_type;  //工作状态第一层

typedef enum
{
  SET_PRO_SPEED                         =0,  //设定反应时间//工作状态第二层
  SET_PRO_OUTACTION                         , //输出动作设定
  SET_PRO_SEL,
  SET_PRO_SIMOUT                          ,//设定检测输出
  SET_PRO_DELT_S                          ,//设定应差
  SET_PRO_EXTN                          ,//设定外部输入
  SET_PRO_TIMET                          ,//设定定时器
  SET_PRO_EXCH_DISP                          ,//设定显示
  SET_PRO_HOLD_OUT                          ,//设定保持
  SET_PRO_POWER_DOWN                         ,//环保设定
  SET_PRO_RESET                         ,//复位设定 恢复至初始状态（出厂状态）。
  PRO_TERMINAL
}PRO_STAT_selection_type;
 typedef enum
{
    flash_err_off =0,//保持OFF
    flash_hard ,
    flash_good ,
    flash_err_41 ,
    flash_terminal
 } flash_selection_type; 
typedef struct
{
  /**
    * @brief usart sts register; offset:0x00
    */
  union
  {
    __IO uint32_t disp_reg;
    struct
    {
      __IO uint32_t laser                 : 1; /* [0] */
      __IO uint32_t out                 : 1; /* [1] */
      __IO uint32_t teach                 : 1; /* [2] */
      __IO uint32_t pro               : 1; /* [3] */
    } disp_bit;
  };

    __IO uint32_t dispnum;
    __IO uint16_t keyid;
} disp_stru;



typedef struct
{
  uint16_t  speed                         ;  //设定反应时间
  uint16_t  outaction;                         //输出动作设定
  uint16_t  meas_mod;                         //测量模式
  uint16_t  simout                          ;//设定检测输出
  uint16_t  delt_s                          ;//设定应差
  uint16_t  extn                          ;//设定外部输入
  uint16_t  time                          ;//设定定时器
  uint16_t  exch_disp                          ;//设定显示
  uint16_t  hold_out                          ;//设定保持
  uint16_t  power_down                         ;//环保设定
  uint16_t  reset                         ;//复位设定 恢复至初始状态（出厂状态）。
  uint16_t  extin_save;                        //根据外部输入，调零的内存保存
}para_stru;

typedef enum
{
 
  LED_FUNC_LASER  = 1,
  LED_FUNC_OUT    =2,     
  LED_FUNC_TEACH   =4,   
  LED_FUNC_ZERO    =8,   
  LED_FUNC_PRO      =0X10               
} out_func_selection_type;    
typedef enum
{
 
  bforegroundcor   = 1,
  bbackgroundcor    =2,     
  bsetfinal         =4, 
 
             
} disp_stat_ctrl_selection_type;  
#define  s_set_bits(r,v,p,msk)    ((r&!(msk<<p))|(v<<p))

#define  s_is_bits(r,v,p,msk)     ((r&(v<<p))
#define  set_bits(r,msk,v)        ((r&!msk)|v)
#define  set_bits(r,v)        (r=(r|v))
#define  clr_bits(r,v)        (r=(r&!v))
#define  is_bits(r,v)             (r&v)
typedef struct
{
  u16 stat;   //工作状态第一层
  u16 statlayer0; //工作状态第二层
  u16 statlayer1; //工作状态第三层
  u16 statlayer2;
  para_stru param; //
  u16  evt;
  u16  keylock;
  u16  dispctrl;
  u16  dispbits;
	s16  dispdata;
  s16  testdata;
  s16  teach1;
  s16  teach2;
  s16  teach3;
  u32  flash;
  u32  func;
  u32  ct;
} app_stru;

/******************user data*******************/
#define TESTDATA_GOOD       10
#define TESTDATA_DISTANCE  30
#define TESTDATA_MIN    (s16)-(TESTDATA_DISTANCE*100)/2
#define TESTDATA_MAX    (s16)(TESTDATA_DISTANCE*100)/2
/*******************************************/

void mcu_Init();
void ctrl_loop(void);
#ifdef __cplusplus
}
#endif

#endif
