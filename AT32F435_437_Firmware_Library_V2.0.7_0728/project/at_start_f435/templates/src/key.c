#include "user_config.h"
#include "key.h"
#include "tm1668.h"
#include "laser_board.h"
//getkeyval_cb getkeyval_t=NULL;
key_stru key={0};

static u16  keyval=0;
#define KEY_TEACH   1
#define KEY_UP      2
#define KEY_DOWN    4 
#define KEY_EXTIN   8
#define KEYID_TEACH   0
#define KEYID_UP      1
#define KEYID_DOWN    2 
#define KEYID_EXTIN   3
#define KEY_DELAY     60             
static void key_teach_free  (); 
static void key_teach_press();
static void key_up_free();
static void key_up_press();
static void key_down_free();
static void key_down_press();
static void key_ex_free();
static void key_ex_press();






key_scan_stru key_tab[]=
{
   {0,0,0,0,{key_teach_free,key_teach_press}},
   {1,0,0,0,{key_up_free,key_up_press}},
   {2,0,0,0,{key_down_free,key_down_press}},
   {3,0,0,0,{key_ex_free,key_ex_press}},
   {0xff}
};
static u8 key_scan ()
{
  __IO u8 a=0,b=0;
  b=read_key();
  if(b&KEY_TEACH)a|=KEY_TEACH;
  if(b&KEY_UP)a|=KEY_UP   ;
  if(b&KEY_DOWN)a|=KEY_DOWN ;
  b=read_ext_in();
  if(b==0)a|=KEY_EXTIN;
}
void clr_key_ct(key_scan_stru *pkey)
{
            pkey->ct=0;
            pkey->up_ct=0;
    
}
static void key_teach_free  ()
{
    key_scan_stru* pkey=&key_tab[KEYID_TEACH];
    
    if(keyval&KEY_TEACH)
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=1;
          //down evt
       } 
       else
       {clr_key_ct(pkey);}
    }

}
static void key_teach_press()
{
     key_scan_stru* pkey=&key_tab[KEYID_TEACH];
    if(keyval&KEY_TEACH)
    {
       if(pkey->ct<20000)pkey->ct++;
       //press evt
    }
    else
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=0;
          //up evt
          key.keyevt=TEACH_KEY_EVT;
           // key.keyevt=UP_KEY_EVT;
          //  key.keyevt=DOWN_KEY_EVT;
           clr_key_ct(pkey);


       } 


    }
}
static void key_up_free()
{
        key_scan_stru* pkey=&key_tab[KEYID_UP];
    
    if(keyval&KEY_UP)
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=1;
          //down evt
       } 
       else
       {clr_key_ct(pkey);}
    }
}
static void key_up_press()
{
     key_scan_stru* pkey=&key_tab[KEYID_UP];
    if(keyval&KEY_UP)
    {
       if(pkey->ct<20000)pkey->ct++;
       //press evt
    }
    else
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=0;
          //up evt
         // key.keyevt=TEACH_KEY_EVT;
            key.keyevt=UP_KEY_EVT;
          //  key.keyevt=DOWN_KEY_EVT;
           clr_key_ct(pkey);


       } 


    }

}
static void key_down_free()
{
        key_scan_stru* pkey=&key_tab[KEYID_DOWN];
    
    if(keyval&KEY_DOWN)
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=1;
          //down evt
       } 
       else
       {clr_key_ct(pkey);}
    }
}
static void key_down_press()
{

    key_scan_stru* pkey=&key_tab[KEYID_DOWN];
    if(keyval&KEY_DOWN)
    {
       if(pkey->ct<20000)pkey->ct++;
       //press evt
       


    }
    else
    {
       if(pkey->up_ct++>KEY_DELAY)
       {
          pkey->stat=0;
          //up evt
          //  key.keyevt=TEACH_KEY_EVT;
           // key.keyevt=UP_KEY_EVT;
            key.keyevt=DOWN_KEY_EVT;
            clr_key_ct(pkey);
       } 


    }

}
static void key_ex_free()
{
        key_scan_stru* pkey=&key_tab[KEYID_EXTIN];
    
    if(keyval&KEY_EXTIN)
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=1;
          //down evt
       } 
       else
       {clr_key_ct(pkey);}
    }
}
static void key_ex_press()
{
     key_scan_stru* pkey=&key_tab[KEYID_EXTIN];
    if(keyval&KEY_EXTIN)
    {
       if(pkey->ct<20000)pkey->ct++;
       //press evt
    }
    else
    {
       if(pkey->ct++>KEY_DELAY)
       {
          pkey->stat=0;
          //up evt
          key.keyevt=EXT_KEY_EVT;
          clr_key_ct(pkey);

       } 


    }

}
void  press_key()
{
    static u8 pressed=0;
    __IO u8 a=0;
    
    if(key_tab[KEYID_TEACH].stat==1)a|=KEY_TEACH;
    if(key_tab[KEYID_UP].stat==1)a|=KEY_UP;
    if(key_tab[KEYID_DOWN].stat==1)a|=KEY_DOWN;

    if(a==0)
    {
      pressed=0;
      return;
    }
    if(pressed==1)return;
    if((key_tab[KEYID_TEACH].ct>=1000)&&(a==KEY_TEACH))
    {
      key.keyevt|=TEACH_1S_KEY_EVT;
      pressed=1;
      return;
    }
    else if((key_tab[KEYID_DOWN].ct>=3000)&&(a==KEY_DOWN))
    {
      key.keyevt|=DOWN_3S_KEY_EVT;
      pressed=1;
      return;
    }
    else if((key_tab[KEYID_TEACH].ct>=3000)&&(key_tab[KEYID_DOWN].ct>=3000)&&(a==KEY_DOWN|KEY_TEACH))
    {
      key.keyevt|=TEACH_DOWN_3S_KEY_EVT;
      pressed=1;
      return;
    }
     else if((key_tab[KEYID_TEACH].ct>=3000)&&(key_tab[KEYID_UP].ct>=3000)&&(a==KEY_UP|KEY_TEACH))
    {
      key.keyevt|=TEACH_UP_3S_KEY_EVT;
      pressed=1;
      return;
    }
     else if((key_tab[KEYID_DOWN].ct>=3000)&&(key_tab[KEYID_UP].ct>=3000)&&(a==KEY_UP|KEY_DOWN))
    {
      key.keyevt|=UP_DOWN_3S_KEY_EVT;
      pressed=1;
      return;
    }
    
}
u32 key_open(void)
{
    static s8 stat=0;
    u8 i=0;
    if(stat==0)
    {
			
			
				stat=1;
    }
    keyval=key_scan ();
    for(i=0;i<4;i++)
    {
       key_tab[i].stat_process_t[key_tab[i].stat];
    }
    press_key();
    return (key.keyevt);
}
void clr_key_evt(u32 evt)
{
	
	key.keyevt&=~evt;
}
