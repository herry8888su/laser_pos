#ifndef __USER_KEY_H___
#define __USER_KEY_H___

#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"
#include "at32f435_437.h"
#include "at32f435_437_conf.h"
/***********user define**********************/


typedef void (*stat_process_cb)(void); 

typedef struct
{
  uint16_t  keyevt; 
               
} key_stru;
 
typedef struct
{
  u8   key_id;
  u8   stat;
  u16  ct;
  u16  up_ct;

  stat_process_cb  stat_process_t[2];
}key_scan_stru;
u32 key_open(void);
void clr_key_evt(u32 evt);










/*******************************************/
#ifdef __cplusplus
}
#endif

#endif