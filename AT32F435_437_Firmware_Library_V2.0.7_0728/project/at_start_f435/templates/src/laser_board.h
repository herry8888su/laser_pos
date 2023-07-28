#ifndef __LASER_BOARD_H___
#define __LASER_BOARD_H___

#ifdef __cplusplus
extern "C" {
#endif
#define terminal_pin 0xff
#define set_bitx(x)  (u32)(1<<x)
typedef struct
{
  u32   nPin;
	gpio_type *gpio_x;
  u32  gpio_mode;            /*!< mode selection */
  u32  func;
  u32  muxfunc;
	
}gpio_stru;
/*----------------------------------------------------------------
DMA_Sx2DCNT 中的 XCOUNT：跳转到下一个跨步之前要传输的数据计数
DMA_Sx2DCNT 中的 YCOUTNT：迭代计数
DMA_Sx2DSTRIDE 中的 SRCSTD：源跨步值。该值应在源端迭代之前添加或减去
DMA_Sx2DSTRIDE 中的 DSTSTD：目的跨步值。该值应在目标端迭代之前添加或减

*/
typedef struct
{
  u16   srcstd;     //n*bytes
	u16   dststd;     //n*bytes
  u16   xcount;     //n*4          
  u16   ycoutnt;    //n*ts
}EDMA_STREAMn_2D_stru;
typedef enum
{

  U32_CPY  = 0, // 
  U32_2_1_CPY,  
  U32_3_1_CPY 

} EDMA_STREAMn_2D_type;
u8 board_loop(void);
void gpio_inits(gpio_stru *p);
void moveTou16(u16 *dst, u16 *src,u16 len);  //len = n*2 (bytes)
u8 get_edma_2d();
u8  read_ext_in();
/*******************************************/
#ifdef __cplusplus
}
#endif

#endif