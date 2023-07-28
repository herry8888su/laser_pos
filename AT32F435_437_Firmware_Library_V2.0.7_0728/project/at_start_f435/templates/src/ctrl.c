#include "user_config.h"
#include "key.h"
#include "laser_adc.h"
#include "laser_board.h"
typedef void (*key_process)(void);  
typedef void (*mode_process)(void); 
app_stru  app;
typedef struct
{
  u16 stat;   //工作状态第一层
  u16 statlayer0; //工作状态第二层
  u16 statlayer1; //工作状态第三层
  u16 statlayer2;
	u16 evt;
	key_process  key_process_t;

	
}key_tbl_stru;
typedef struct
{
  u16 mode;   //模式处理
	mode_process  mode_process_t;
}mode_tbl_stru;
void  Teach_into(void);
void  base_into(void);
void  test_mode(void);
void 	peak_into(void);
void 	lock_into(void);
void 	pro_into	(void);
void 	base_conv_into(void);
void 	zero_into(void);
key_tbl_stru  key_process_tbl[]={
	
	{STAT_TEST,0,0,0,TEACH_KEY_EVT,Teach_into},
	{STAT_TEST,0,0,0,DOWN_KEY_EVT,base_into},
	{STAT_TEST,0,0,0,UP_KEY_EVT,base_into},
	{STAT_TEST,0,0,0,TEACH_UP_3S_KEY_EVT,peak_into},
	{STAT_TEST,0,0,0,TEACH_DOWN_3S_KEY_EVT,lock_into},
	{STAT_TEST,0,0,0,DOWN_3S_KEY_EVT,pro_into},
	{STAT_TEST,0,0,0,TEACH_1S_KEY_EVT,base_conv_into},
	{STAT_TEST,0,0,0,UP_DOWN_3S_KEY_EVT,zero_into}
	
	
};
void  Teach_into(void)
{
}
void  base_into(void)
{
	
}
void peak_into(void)
{
	
	
}
void lock_into(void)
{
	
}
void pro_into	(void)
{
	
}
void base_conv_into(void)
{
}
void zero_into(void)
{
	
	
}
void set_pro_sp			(void)
{
	
}
void set_pro_act    (void)
	{
	
}
void set_pro_selmod (void)
	{
	
}
void set_pro_so     (void)
	{
	
}
void set_pro_ds     (void)
	{
	
}
void set_pro_ex     (void)
	{
	
}
void set_pro_tim    (void)
	{
	
}
void set_pro_disp   (void)
	{
	
}
void set_pro_hold   (void)
	{
	
}
void set_pro_pwrd   (void)
	{
	
}
void set_pro_res    (void)
	{
	
}

mode_tbl_stru  pro_mode_tbl[]={
		{SET_PRO_SPEED      ,set_pro_sp			},
		{SET_PRO_OUTACTION  ,set_pro_act    },
		{SET_PRO_SEL        ,set_pro_selmod },
		{SET_PRO_SIMOUT     ,set_pro_so     },
		{SET_PRO_DELT_S     ,set_pro_ds     },
		{SET_PRO_EXTN       ,set_pro_ex     },
		{SET_PRO_TIMET      ,set_pro_tim    },
		{SET_PRO_EXCH_DISP  ,set_pro_disp   },
		{SET_PRO_HOLD_OUT   ,set_pro_hold   },
		{SET_PRO_POWER_DOWN ,set_pro_pwrd   },
		{SET_PRO_RESET      ,set_pro_res    }

};
void Teach_mode()
{
}
void base_mode()
{
}
void test_mode()
{
	
	
}
void peak_mode()
{
}
void lock_mode()
{
}
void pro_mode	()
{
	u8 i=0;
	for(i=0;i<MAIN_STAT_TERMINAL;i++)
	{
		if(app.statlayer0==pro_mode_tbl[i].mode)pro_mode_tbl[i].mode_process_t();
		
			
	}
}
void base_conv_mode()
{
}
void zero_mode()	
{
}
mode_tbl_stru  main_mode_tbl[]={
	 {STAT_TEST,test_mode},
   {STAT_TEACH,Teach_mode					},
   {STAT_BASE_ADJ,base_mode       },
   {STAT_PEAK_HOLD,peak_mode      },
   {STAT_ZERO,zero_mode           },
   {STAT_KEY_LOCK,lock_mode       },
	 {STAT_BASE_CHK,base_conv_mode  },
   {STAT_PRO,pro_mode}
	
};
void  run_mode(void)
{
	   
	u8 i=0;
	for(i=0;i<MAIN_STAT_TERMINAL;i++)
	{
		if(app.stat==main_mode_tbl[i].mode)main_mode_tbl[i].mode_process_t();	
	}
		
	
}

void mcu_Init()
{
	laser_Init();
	disp_init();
  app.stat=0;
  app.statlayer0=0;
	app.statlayer1=0;
	app.statlayer2=0;
	app.param.speed 	  =0;      
	app.param.outaction   =0;
	app.param.meas_mod    =0;
	app.param.simout      =0;
	app.param.delt_s      =0;
	app.param.extn        =0;
	app.param.time        =0;
	app.param.exch_disp   =0;
	app.param.hold_out    =0;
	app.param.power_down  =0;
	app.param.reset       =0;
	app.param.extin_save  =0;
	

}

void evt_process()
{
	  u8 i=0;
	  for (i = 0; i < 3; i++)
	  {
		if ((app.stat != key_process_tbl[i].stat) && (app.statlayer0 != key_process_tbl[i].statlayer0))
			continue;
		if (app.evt == key_process_tbl[i].evt)
		{
			if (app.statlayer1 == key_process_tbl[i].statlayer1)
			{
				key_process_tbl[i].key_process_t();
			}
		}
	  }
}
void ctrl_loop(void)
{

	if(board_loop())
	{
		app.evt=key_open();
		evt_process();
		run_mode();
		disp_out(&app);
		clr_key_evt(0xffff);
	}
}