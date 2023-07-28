#include "user_config.h"
#include "tm1668.h"
#include "laser_board.h"
#define   CMD_DISP_MOD  0
typedef enum
{
 
    DISP_MOD_CMD  =0,//0  0  显示模式命令设置
    DATA_CMD,//0  1  数据命令设置
    CTRL_DISP_CMD,//1  0  显示控制命令设置
    ADDR_CMD//1  1  地址命令设置
} TM1668_CMD_mode_selection_type;    //在STB下降沿后由DIN输入的第一个字节作为指令

typedef enum
{
    B4SEG13=0,   //  0  4 位 13 段
    B5SEG12,//  0  0  1  5 位 12 段
    B6SEG11,//  0  1  0  6 位 11 段
    B7SEG10//  0  1  1  7 位 10 段
} DISP_MOD_CMD_selection_type;    //该指令用来设置选择段和位的个数（4～7 位,10～13 段）。当该指令被执行时,显示被强制关闭,上电时,默认显示模式为 7 位 10段



typedef enum
{
    CTRL_DAT_RW = 0,//写数据到显示寄存器
    CTRL_KEY_R = 2,//读键扫数据
    //CTRL_AUTO_ADDR_INC = 0,//自动地址增加
    //CTRL_FIX_ADDR = 0,//固定地址
    CTRL_ADDR   =4,
    //CTRL_NORM_MOD = 0,//普通模式
    //CTRL_TEST_MOD = 0,//测试模式
    CTRL__MOD   =8
} CTRL_MOD_CMD_selection_type;  //该指令用来设置显示的开关以及显示亮度调节。共有8级辉度可供选择进行调节
typedef enum
{
    DISP_PULSE_1 = 0,//设置脉冲宽度为 1/16
    DISP_PULSE_2 ,//1  0  0  0  1  设置脉冲宽度为 2/16
    DISP_PULSE_3 ,//1  0  0  1  0  设置脉冲宽度为 4/16
    DISP_PULSE_4 ,//1  0  0  1  1  设置脉冲宽度为 10/16
    DISP_PULSE_5 ,//1  0  1  0  0  设置脉冲宽度为 11/16
    DISP_PULSE_6 ,//1  0  1  0  1  设置脉冲宽度为 12/16
    DISP_PULSE_7 ,//1  0  1  1  0  设置脉冲宽度为 13/16
    DISP_PULSE_8 ,//1  0  1  1  1  设置脉冲宽度为 14/16
    DISP_PULSE_SET//显示开关设置

} CTRL_DISP_CMD_selection_type;  //该指令用来设置显示的开关以及显示亮度调节。共有8级辉度可供选择进行调节
typedef enum
{
    DISP_ADDR_0 =  0XC0,
    DISP_ADDR_1 =  0XC1,
    DISP_ADDR_2 =  0XC2,
    DISP_ADDR_3 =  0XC3,
    DISP_ADDR_4 =  0XC4,
    DISP_ADDR_5 =  0XC5,
    DISP_ADDR_6 =  0XC6,
    DISP_ADDR_7 =  0XC7,
    DISP_ADDR_8 =  0XC8,
    DISP_ADDR_9 =  0XC9,
    DISP_ADDR_10 =  0XCA,
    DISP_ADDR_11 =  0XCB,
    DISP_ADDR_12 =  0XCC,
    DISP_ADDR_13 =  0XCD
} ADDR_CMD_selection_type;  //该指令用来设置显示寄存器的地址。最多有效地址为14位(C0H-CDH)
#define   TM1668_CMD(CMD,VAL)  (u8)((CMD<<6)+VAL)

// 00：输入（复位后的模式）
// 01：通用输出
// 10：复用功能
// 11：模拟
#define   PIN_DIR_MSK  3  //PA11
#define   DA_PIN_INPUT   0  //PA11
#define   DA_PIN_OUTPUT  1  //PA11
#define   STB_PC13  13
#define   DIO_PB9   9
#define   CLK_PB8   8
#define   STB_PIN   (1<<STB_PC13)
#define   DIO_PIN   (1<<DIO_PB9 )
#define   CLK_PIN   (1<<CLK_PB8 )


#define SET_STB(x)  ((x==1)?(GPIOC->odt|=STB_PIN):(GPIOC->odt&=!STB_PIN))
#define SET_CLK(x)  ((x==1)?(GPIOB->odt|=CLK_PIN):(GPIOB->odt&=!CLK_PIN))
#define SET_DATA(x) ((x==1)?(GPIOB->odt|=DIO_PIN):(GPIOB->odt&=!DIO_PIN))
/*SET_DATA_IO(DA_PIN_OUTPUT) */
#define SET_DATA_IO(x) (s_set_bits(GPIOB->cfgr,x,DIO_PB9,PIN_DIR_MSK))  //(r,v,p,msk) 



#define RD_DATA     1
#define  DELAY_VAL 3
#define   BIT5_SEG12        TM1668_CMD(0,1)  //5 位 12 段
#define   ADDR_INC_W_MODE   TM1668_CMD(1,4)     //写数据到显示寄存器 自动地址增加
#define   KEY_INC_R_MODE    TM1668_CMD(1,4|1)     //写数据到显示寄存器 自动地址增加
#define   DISP_OFF    TM1668_CMD(2,4)     //写数据到显示寄存器 自动地址增加
#define   DISP_ON    TM1668_CMD(2,8|4)     //写数据到显示寄存器 自动地址增加
#define   ADDR_SET(X)    TM1668_CMD(3,X)     //写数据到显示寄存器 自动地址增加
static gpio_stru gpio_cfg[] = {
    {STB_PC13, GPIOC, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, 0},
    {CLK_PB8,  GPIOB, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, 0},
    {DIO_PB9,  GPIOB, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, 0},
    {terminal_pin}};
typedef struct
{
  u8       seg1;
  u8       seg1b;
  u8       seg2;
  u8       seg2b;
  u8       seg3;
  u8       seg3b;
  u8       seg4;
  u8       seg4b;
  u8       seg5;
  u8       seg5b;
} DISP_stru;
 typedef struct
{
  u16 nt;
	u16 duty;
	u16 t;
	disp_buff_stru fore;
  disp_buff_stru back;
}fix_disp_out_stru;
disp_buff_stru disp_buff={0};
DISP_stru      dispbuf={0};
u8 dig_tbl[]={d_0,d_1,d_2,d_3,d_4,d_5,d_6,d_7,d_8,d_9};
fix_disp_out_stru  dispout_tbl[]={
	{0,0,0,{SEGG,SEGG,SEGG,SEGG},{SEGG,SEGG,SEGG,SEGG}},
	{0,50,1000,{0},{d_t,d_c,d_h,d_1|d_dot}},
	{0,50,1000,{0},{d_t,d_c,d_h,d_2|d_dot}},
	{0,50,1000,{0},{d_t,d_c,d_h,d_3|d_dot}},
	{0,50,1000,{0},{d_t,d_c,d_h,d_4|d_dot}},
	{3,50,1000,{d_g,d_o,d_o,d_d},{0}},
	{3,50,1000,{d_H,d_a,d_r,d_d},{0}},
	{3,50,1000,{d_e,d_r,d_4,d_1},{0}},//执行教导时，未能正常测量
	
	//上升微分模式或下降微分模式的跨距调整
	{0,20,1000,{d_d,SEGH,0,0},{0}},
	{3,20,1000,{0},{0}},  //基准值微调功能
	{0,50,1000,{d_1,SEGH,d_s,d_L},{d_1,SEGH,d_s,d_L}},
	{0,50,1000,{d_2,SEGH,d_s,d_L},{d_2,SEGH,d_s,d_L}},
	 
	{0,50,1000,{d_p,d_o,d_f,d_f},{0}},//峰值、谷值保持功能
	{0,50,1000,{0,d_p,SEGH,d_H},{0}},
	{0,50,1000,{0,d_p,SEGH,d_b},{0}},
	
	{5,50,1000,{d_L,d_c,d_o,d_n},{0}},//按键锁定功能
	{5,50,1000,{d_L,d_c,d_o,d_f},{0}},
	{5,50,1000,{0,d_L,d_o,d_c},{0}},
	
	
	
//     {d_e,d_r,d_0,d_1},//闪存发生损坏，或已到使用寿命
//     {d_e,d_r,d_1,d_1},//检测输出的负荷短路形成的过大电流
//     {d_e,d_r,d_2,d_1},//半导体激光发生损坏，或者已到使用寿命
//     {d_e,d_r,d_3,d_1},//调零时，未能正常测量  由于显示设定设为偏移，因此不能使用调零功能。
//     {d_e,d_r,d_4,d_1},//执行教导时，未能正常测量
//     {d_e,d_r,d_9,d_0},//系统错误
//     {d_e,d_r,d_9,d_1},//系统错误
//     {d_e,d_r,d_9,d_2},//系统错误
//     {d_e,d_r,d_9,d_3}//系统错误
	
	{0,50,1000,{d_s,d_p,d_e,d_d},{d_s,d_p,d_e,d_d}}, //设定反应时间
	{0,50,1000,{d_H,d_r|d_dot,d_s,d_o},{d_H,d_r|d_dot,d_s,d_o}},
	{0,50,1000,{0,d_s,d_t,d_d},{0,d_s,d_t,d_d}},
	{0,50,1000,{d_f,d_a,d_s,d_t},{d_f,d_a,d_s,d_t}},
	
	{0,50,1000,{0,d_L,SEGH,d_d},{0,d_L,SEGH,d_d}},   //输出动作设定
	{0,50,1000,{d_L,SEGH,d_o,d_n},{d_L,SEGH,d_o,d_n}},
	{0,50,1000,{d_d,SEGH,d_o,d_n},{d_d,SEGH,d_o,d_n}},
	
	{0,50,1000,{d_s,d_e,d_n,d_s},{d_s,d_e,d_n,d_s}},//设定检测输出
	{0,50,1000,{SEGD,SEGH,d_T,SEGA},{SEGD,SEGH,d_T,SEGA}},
	{0,50,1000,{SEGD,d_M,SEGD,d_1|d_dot},{SEGD,d_M,SEGD,d_1|d_dot}},
	{0,50,1000,{SEGD,d_M,SEGD,d_2|d_dot},{SEGD,d_M,SEGD,d_2|d_dot}},
	{0,50,1000,{SEGD,d_M,SEGD,d_3|d_dot},{SEGD,d_M,SEGD,d_3|d_dot}},
	{0,50,1000,{d_d,0,SEGD,d_T},{d_d,0,SEGD,d_T}},
	{0,50,1000,{d_d,0,SEGA,d_L},{d_d,0,SEGA,d_L}},
	 
	{0,50,1000,{d_a,d_o|d_dot,d_u,d_t},{d_a,d_o|d_dot,d_u,d_t}},  //模拟输出设定
	{0,50,1000,{d_v,d_o|d_dot,d_u,d_t},{d_v,d_o|d_dot,d_u,d_t}},
	{0,50,1000,{d_i,d_o|d_dot,d_u,d_t},{d_i,d_o|d_dot,d_u,d_t}},
	
	
	
	
	
	{0,50,1000,{0,d_H,d_y,d_s},{0,d_H,d_y,d_s}},//设定应差
	 {0,50,1000,{0},{0,d_H,d_y,d_s}},
	
	
	
	{0,50,1000,{d_i,d_n,d_p,d_t},{d_i,d_n,d_p,d_t}},//设定外部输入
	{0,50,1000,{d_0,d_s,d_e,d_t},{d_0,d_s,d_e,d_t}},
	{0,50,1000,{d_t,d_e,d_c,d_H},{d_t,d_e,d_c,d_H}},
	{0,50,1000,{d_L,SEGH,d_o,d_f},{d_L,SEGH,d_o,d_f}},//L-0F
	{0,50,1000,{d_t,d_r,d_i,d_9},{d_t,d_r,d_i,d_9}},
	
	{0,50,1000,{d_s,d_a,d_v,d_e},{d_s,d_a,d_v,d_e}}, //设定外部输入,保存有效 
	{0,50,1000,{0,0,d_o,d_n},{0,0,d_o,d_n}},
	{0,50,1000,{0,d_o,d_f,d_f},{0,d_o,d_f,d_f}},
	
	
	
	{0,50,1000,{d_d,d_e,d_L,d_y},{d_d,d_e,d_L,d_y}},//设定定时器
	{0,50,1000,{0,d_n,d_o,d_n},{d_d,d_n,d_o,d_n}},
	{0,50,1000,{0,d_o,d_f,d_d},{0,d_o,d_f,d_d}},
	{0,50,1000,{0,d_o,d_n,d_d},{0,d_o,d_n,d_d}},
	{0,50,1000,{0,d_o,d_s,d_d},{0,d_o,d_s,d_d}},
	
	{0,50,1000,{d_d,d_i,d_s,d_p},{d_d,d_i,d_s,d_p}},//设定显示
	{0,50,1000,{0,d_s,d_t,d_d},{0,d_s,d_t,d_d}},
	{0,50,1000,{d_i,d_n,d_v,d_e},{d_i,d_n,d_v,d_e}},
	{0,50,1000,{d_o,d_f,d_s,d_t},{d_o,d_f,d_s,d_t}},
	
	{0,50,1000,{d_H,d_o,d_L,d_d},{d_H,d_o,d_L,d_d}},//设定保持
	{0,50,1000,{0,d_o,d_f,d_f},{0,d_o,d_f,d_f}},
	{0,50,1000,{0,0,d_o,d_n},{0,0,d_o,d_n}},
	
	{0,50,1000,{0,d_e,d_c,d_o},{0,d_e,d_c,d_o}},//环保设定
	{0,50,1000,{0,d_o,d_f,d_f},{0,d_o,d_f,d_f}},
	{0,50,1000,{0,0,d_o,d_n},{0,0,d_o,d_n}},
	
	{0,50,1000,{d_r,d_s,d_e,d_t},{d_r,d_s,d_e,d_t}},//设定复位
	{0,50,1000,{0,0,d_n,d_o},{0,0,d_n,d_o}},
	{0,50,1000,{0,d_y,d_e,d_s},{0,d_y,d_e,d_s}},

};
u16 norm_tbl[]={disp_Normal,disp_er41};
u16 teach1_tbl[]={disp_teach2,disp_good,disp_hard};
u16 teach2_tbl[]={disp_teach2,disp_good,disp_hard};
u16 teach3_tbl[]={disp_teach2,disp_teach3,disp_good,disp_hard};

u16 base_tbl[]={0};
u16 peak_tbl[]={0};
u16 zero_tbl[]={0};
u16 lock_key_tabl[]={0};

u16 pro_disp_sped_tbl[]={
  disp_sped,  //设定反应时间
  disp_10ms,
  disp_5ms,
  disp_1ms5
};
u16 pro_disp_ld_tbl[]={  
  disp_L_d,  //输出动作设定
  disp_L_on,
  disp_d_on
};
u16 pro_disp_sens_tbl[]={
	disp_sens,//设定检测输出
  disp_sens_norm,
  disp_sens_teach1,
  disp_sens_teach2,
  disp_sens_teach3,
  disp_sens_raising,
  disp_sens_falling,
};
u16 pro_disp_sim_tbl[]={
  disp_simout,//模拟输出设定
  disp_vout,
  disp_iout
};
u16 pro_disp_hys_tbl[]={
  disp_hys,  //设定应差
  disp_hys1,
};
u16 pro_disp_ex_tbl[]={
  disp_extin,  //设定外部输入
  disp_zero_ex,
  disp_teach_ex,
  disp_L_ex,
  disp_trig_ex,
  disp_save_ex,
  disp_on_ex,
  disp_off_ex
};
u16 pro_disp_delay_tbl[]={
  disp_delay,//设定定时器
  disp_delay_no,
  disp_delay_off,
  disp_delay_action,
  disp_delay_single
};
u16 pro_disp_disp_tbl[]={

  disp_disp,//设定显示
  disp_disp_std,
  disp_disp_inv,
  disp_disp_offset
};
u16 pro_disp_hold_tbl[]={
  disp_hold,  //设定保持
  disp_hold_on,
  disp_hold_off
};
u16 pro_disp_eco_tbl[]={
  disp_eco,   //环保设定
  disp_eco_on,
  disp_eco_off
};
u16 pro_disp_res_tbl[]={
  disp_reset, //恢复至初始状态
  disp_reset_no,
  disp_reset_yes,
	};
//u16* disp_l0_tabl[]={norm_tbl,teach_tbl,base_tbl,peak_tbl,zero_tbl,lock_key_tabl,pro_tbl};  //stat


u16* teach_tbl[]={teach1_tbl,teach2_tbl,teach3_tbl};
u16* norm_test_tbl[]={norm_tbl};
u16*  pro_tbl[]={pro_disp_sped_tbl,
	pro_disp_ld_tbl,
pro_disp_sens_tbl,
pro_disp_sim_tbl,
pro_disp_hys_tbl,
pro_disp_ex_tbl,
pro_disp_delay_tbl,
pro_disp_disp_tbl,
pro_disp_hold_tbl,
pro_disp_eco_tbl,
pro_disp_res_tbl};
u16** disp_l0_tabl[]={norm_test_tbl,teach_tbl,pro_tbl};



void moveTodisp_buff(u8 *p)//调用表格显示码
{
   disp_buff.seg1=*p++;
   disp_buff.seg2=*p++;
   disp_buff.seg3=*p++;
   disp_buff.seg4=*p++;

}
void hexTodecbuff(s16 da)  //数据转十进制再转显示码
{
    s16 a=da,b=0;
    u8  dot=0;
    
    if(a<0)
    {
      a=-a;
      dot=1;
    }
     a=a/1000;
     if(a==0)
     {
       
       if(dot==1)
       {
        
        disp_buff.seg1|=SEGG;

       }

     }
     else
     {
        disp_buff.seg1=dig_tbl[a];
       if(dot==1)
       {
        
        disp_buff.seg1|=d_dot;

       }




     }

    b=a%1000;
    a=b/100;
    disp_buff.seg2=dig_tbl[a];
    b=b%100;
    a=b/10;
    disp_buff.seg3=dig_tbl[a]|d_dot;
    b=b%10; 
    a=b;
    disp_buff.seg4=dig_tbl[a];
}
void get_dispdata(app_stru *p)
{
	static u16 ct=0,cts=0;
	fix_disp_out_stru *pd;
	  
	  pd=&dispout_tbl[disp_l0_tabl[p->stat][p->statlayer0][p->statlayer1]];
	 if(ct++<pd->t)
	 {
		 ct=0;
		 cts++;
	 }
	 if(ct<pd->t*pd->duty/100)
	 {
		 if(is_bits(p->dispctrl,bforegroundcor))
		 {
			 hexTodecbuff(p->dispdata);
			 if(p->dispbits==2)
			 {
				 disp_buff.seg1=pd->fore.seg1;
				 disp_buff.seg2=pd->fore.seg2;
			 }
			 if(p->dispbits==1)
			 {
				 disp_buff.seg1=pd->fore.seg1;
				 disp_buff.seg2=pd->fore.seg2;
				 disp_buff.seg3=pd->fore.seg3;
			 }
			 
			 
		 }
		 else
		 {
			 
			 moveTodisp_buff((u8*) &pd->fore);
		 }
		 
	 }
	 else
	 {
		  if(is_bits(p->dispctrl,bbackgroundcor))
		 {
			 hexTodecbuff(p->dispdata);
			 if(p->dispbits==2)
			 {
				 disp_buff.seg1=pd->back.seg1;
				 disp_buff.seg2=pd->back.seg2;
			 }
			 if(p->dispbits==1)
			 {
				 disp_buff.seg1=pd->back.seg1;
				 disp_buff.seg2=pd->back.seg2;
				 disp_buff.seg3=pd->back.seg3;
			 }
			 
			 
		 }
		 else
		 {
			 
			 moveTodisp_buff((u8*) &pd->back);
		 }
		 
	 }
	 if(pd->nt>0)
   {
     if(cts>=pd->nt)
		 {
			 
			 cts=0;
			 set_bits(p->dispctrl,bsetfinal);
		 }



	 }		 
	 
	  
}


u8 moveTodisbuff(app_stru *p)
{
	  static u8 seg5=0;
	  u8 a=0;
	  if(dispbuf.seg1!=disp_buff.seg1)a=1;
		if(dispbuf.seg2!=disp_buff.seg2)a=1;
		if(dispbuf.seg3!=disp_buff.seg3)a=1;
		if(dispbuf.seg4!=disp_buff.seg4)a=1;
		if(dispbuf.seg5!=seg5)a=1;
	  dispbuf.seg1=disp_buff.seg1;
		dispbuf.seg2=disp_buff.seg2;
		dispbuf.seg3=disp_buff.seg3;
		dispbuf.seg4=disp_buff.seg4;
    dispbuf.seg5=0;
    if(p->func&LED_FUNC_LASER)dispbuf.seg5|=LED_FUNC_LASER;
    if(p->func&LED_FUNC_OUT)dispbuf.seg5|=LED_FUNC_OUT;
    if(p->func&LED_FUNC_TEACH)dispbuf.seg5|=LED_FUNC_TEACH;
    if(p->func&LED_FUNC_ZERO)dispbuf.seg5|=LED_FUNC_ZERO;
    if(p->func&LED_FUNC_PRO)dispbuf.seg5|=LED_FUNC_PRO;
		 seg5=dispbuf.seg5;
		 return 0;
}

void delay_nt(u8 t)
{
    while(t>0)t--;
}

void write_byte(u8 n)
{
    u8 i=0;
    u8 txbuf=n;
            
    for(i=0;i<8;i++)
    {
       SET_CLK(0);
       delay_nt(DELAY_VAL);
       if(txbuf&1)
       {
        
        SET_DATA(1);
        delay_nt(DELAY_VAL);
        SET_CLK(1);
        delay_nt(DELAY_VAL);

       }
       else
       {
        SET_DATA(0);
        delay_nt(DELAY_VAL);
        SET_CLK(1);
        delay_nt(DELAY_VAL);


       }
       txbuf>>=1;

    }



}

void write_singlebyte(u8 da)
{
     SET_CLK(1);
     SET_STB(0);
     write_byte(da);
     SET_CLK(1);
     SET_STB(0);
}

void write_nbyte(u8 *p,u8 len)
{
    u8 i=0;
    
    SET_CLK(1);
    SET_STB(0);
        
    for(i=0;i<len;i++)
    {
       write_byte(*p++);
    }

    SET_CLK(1);
    SET_STB(1);

}

u8 read_byte(void)
{
    u8 i=0;
    u8 rxbuf=0;
        SET_DATA_IO(DA_PIN_INPUT);
        SET_CLK(1);
        SET_STB(0);
        
    for(i=0;i<8;i++)
    {
       
       SET_CLK(0);
       delay_nt(DELAY_VAL);
       SET_CLK(1);
       delay_nt(DELAY_VAL);
       rxbuf<<=1;
       if(RD_DATA&1)
       {
        rxbuf|=1;
       }
       delay_nt(DELAY_VAL);

    }
    SET_STB(1);
    SET_DATA_IO(DA_PIN_OUTPUT);

}
// #define LED2_PIN                         GPIO_PINS_13
// #define LED2_GPIO                        GPIOD
// #define LED2_GPIO_CRM_CLK                CRM_GPIOD_PERIPH_CLOCK
// #define LED3_PIN                         GPIO_PINS_14
// #define LED3_GPIO                        GPIOD
// #define LED3_GPIO_CRM_CLK                CRM_GPIOD_PERIPH_CLOCK
// #define LED4_PIN                         GPIO_PINS_15
// #define LED4_GPIO                        GPIOD
// #define LED4_GPIO_CRM_CLK                CRM_GPIOD_PERIPH_CLOCK
void disp_init()
{
	  gpio_inits(gpio_cfg);
    SET_CLK(1);
    SET_STB(1);
    SET_DATA(1);
    write_singlebyte(BIT5_SEG12);//设 置 显 示 模式 
    write_singlebyte(ADDR_INC_W_MODE);//设 置 写 显 存 的 数 据 命 令 ，采 用 地 址 自 动 加 1
}

u8 disp_seg_out(u8 *p)
{
  u8 len=sizeof(DISP_stru);
    write_singlebyte(BIT5_SEG12);//设 置 显 示 模式 
    write_singlebyte(DISP_OFF);
    write_singlebyte(ADDR_INC_W_MODE);//设 置 写 显 存 的 数 据 命 令 ，采 用 地 址 自 动 加 1
    write_singlebyte(ADDR_SET(0));//设 置 起 始 地址 
    write_nbyte((u8*)&dispbuf ,sizeof(DISP_stru));
    write_singlebyte(DISP_ON);

}

u8 read_key(void)
{
    u8 val[5]={0};
    write_singlebyte(KEY_INC_R_MODE);
    val[0] =read_byte();
    val[1] =read_byte();
    val[2] =read_byte();
    val[3] =read_byte();
    val[4] =read_byte();
    return val[4];
}


void disp8888_off()
{
   disp_buff.seg1=0;
   disp_buff.seg2=0;
   disp_buff.seg3=0;
   disp_buff.seg4=0;
}
#if 1



void disp_out(app_stru *p)
{   get_dispdata(p);
   if(moveTodisbuff(p))disp_seg_out((u8*)&dispbuf);
}
#endif