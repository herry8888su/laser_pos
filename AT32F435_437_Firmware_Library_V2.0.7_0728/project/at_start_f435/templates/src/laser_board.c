#include "user_config.h"
#include "laser_board.h"
/**
PB3
JTDO
SWO
TMR2_CH2  I2C2_SDA
SPI1_SCK
I2S1_CK
SPI3_SCK
I2S3_CK
USART1_RX
PB5
TMR3_CH2  I2C1_SMBA
SPI1_MOSI
I2S1_SDEX
T
SPI3_MOSI
I2S3_SDEX
T
USART1_CK
PB6
TMR4_CH1  I2C1_SCL  I2S1_MCK
SPI4_CS
I2S4_WS
USART1_TX   MUX7
PB7
TMR4_CH2  TMR8_BRK  I2C1_SDA
SPI4_SCK
I2S4_CK
USART1_RX  MUX7
***/
#define usart1_rx_pb3 3
#define usart1_tx_pb6 6
#define ext_input_pin 4
__IO uint32_t data_counter_end = 0; /* this variable should not be initialized to 0 */
// typedef enum
// {
//   GPIO_MODE_INPUT                        = 0x00, /*!< gpio input mode */
//   GPIO_MODE_OUTPUT                       = 0x01, /*!< gpio output mode */
//   GPIO_MODE_MUX                          = 0x02, /*!< gpio mux function mode */
//   GPIO_MODE_ANALOG                       = 0x03  /*!< gpio analog in/out mode */
// } gpio_mode_type;
/**************** define print uart ******************/
#define PRINT_UART USART1
#define PRINT_UART_CRM_CLK CRM_USART1_PERIPH_CLOCK
#define PRINT_UART_IRQn USART1_IRQn
// #define PRINT_UART_TX_PIN                GPIO_PINS_6
// #define PRINT_UART_TX_GPIO               GPIOB
// #define PRINT_UART_TX_GPIO_CRM_CLK       CRM_GPIOB_PERIPH_CLOCK
// #define PRINT_UART_TX_PIN_SOURCE         GPIO_PINS_SOURCE6
// #define PRINT_UART_TX_PIN_MUX_NUM        GPIO_MUX_8
#define rx_len 64
static u8 rx_da[rx_len] = {0};
static u16 rx_ct = 0;
static gpio_stru gpio_cfg[] = {
    {usart1_rx_pb3, GPIOB, GPIO_MODE_MUX, 0, GPIO_MUX_7},
    {usart1_tx_pb6, GPIOB, GPIO_MODE_MUX, 0, GPIO_MUX_7},
    {ext_input_pin, GPIOB, GPIO_MODE_INPUT, GPIO_PULL_UP, 0},
    {terminal_pin}};
/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
__asm(".global __use_no_semihosting\n\t");
void _sys_exit(int x)
{
  x = x;
}
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch)
{
  ch = ch;
}
FILE __stdout;
#else
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)
struct __FILE
{
  int handle;
};
FILE __stdout;
void _sys_exit(int x)
{
  x = x;
}
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch)
{
  ch = ch;
}
#endif
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
 * @brief  retargets the c library printf function to the usart.
 * @param  none
 * @retval none
 */
PUTCHAR_PROTOTYPE
{
  while (usart_flag_get(PRINT_UART, USART_TDBE_FLAG) == RESET)
    ;
  usart_data_transmit(PRINT_UART, ch);
	rx_ct=0;
  return ch;
}
void gpio_inits(gpio_stru *p)
{
  gpio_init_type gpio_init_structure;
  u8 stat = 0;
  gpio_default_para_init(&gpio_init_structure);
  while (p->nPin != terminal_pin)
  {

    if (p->gpio_x == GPIOA)
    {
      if (stat != 1)
      {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        stat = 1;
      }
    }
    if (p->gpio_x == GPIOB)
    {
      if (stat != 2)
      {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        stat = 2;
      }
    }
    if (p->gpio_x == GPIOC)
    {
      if (stat != 3)
      {
        crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
        stat = 3;
      }
    }
		if (p->gpio_x == GPIOD)
    {
      if (stat != 4)
      {
        crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
        stat = 4;
      }
    }
    if (p->gpio_mode == GPIO_MODE_INPUT)
    {

      // PB0:pwm out chk

      gpio_init_structure.gpio_mode = GPIO_MODE_INPUT;
      gpio_init_structure.gpio_pins = set_bitx(p->nPin);
      gpio_init_structure.gpio_pull = p->func; // (u32)(p->func<<1);
      gpio_init(p->gpio_x, &gpio_init_structure);
    }
    else if (p->gpio_mode == GPIO_MODE_OUTPUT)
    {
      gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
      gpio_init_structure.gpio_out_type = p->func;
      gpio_init_structure.gpio_mode = GPIO_MODE_OUTPUT;
      gpio_init_structure.gpio_pins = set_bitx(p->nPin);
			gpio_init_structure.gpio_pull =p->muxfunc;
      gpio_init(p->gpio_x, &gpio_init_structure);
    }
    else if (p->gpio_mode == GPIO_MODE_MUX)
    {
      gpio_init_structure.gpio_mode = GPIO_MODE_MUX;
      gpio_init_structure.gpio_pins = set_bitx(p->nPin);
      gpio_init_structure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
      gpio_init_structure.gpio_pull = GPIO_PULL_NONE;
      gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
      gpio_init(p->gpio_x, &gpio_init_structure);
      gpio_pin_mux_config(p->gpio_x, p->nPin, (u32)(p->muxfunc));
    }
    else if (p->gpio_mode == GPIO_MODE_ANALOG)
    {

      gpio_init_structure.gpio_mode = GPIO_MODE_ANALOG;
      gpio_init_structure.gpio_pins = set_bitx(p->nPin);
      gpio_init(p->gpio_x, &gpio_init_structure);
    }
    p++;
  }
}

/**
 * @brief  initialize uart
 * @param  baudrate: uart baudrate
 * @retval none
 USART 在 HALFSEL 位置 1 时选择使用单线双向半双工的方式进行数据通信，在此条件下，LINEN 位，
CLKEN 位，SCMEN 位以及 IRDAEN 位需置 0，此时在 USART 内部，RX 引脚无效，TX 引脚和 SW_RX
引脚互连，对 USART 来说，TX 引脚用于数据输出，SW_RX 用于数据输入，对外设来说，数据都从 TX
引脚映射的 IO 双向传输。
 */
void uart_print_init(uint32_t baudrate)
{
  gpio_init_type gpio_init_struct;
  /* config usart nvic interrupt */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(PRINT_UART_IRQn, 0, 0);

  /* enable the uart and gpio clock */
  crm_periph_clock_enable(PRINT_UART_CRM_CLK, TRUE);
  gpio_inits(gpio_cfg);
  /* configure uart param */
  usart_init(PRINT_UART, baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(PRINT_UART, TRUE);
  usart_receiver_enable(PRINT_UART, TRUE);
  usart_interrupt_enable(PRINT_UART, USART_RDBF_INT, TRUE);
  usart_enable(PRINT_UART, TRUE);
}
void UART5_IRQHandler(void)
{
  u8 a = 0;
  if (usart_flag_get(PRINT_UART, USART_RDBF_FLAG) != RESET)
  {
    usart_flag_clear(PRINT_UART, USART_RDBF_FLAG);
    /* read one byte from the receive data register */
    rx_da[rx_ct++] = usart_data_receive(PRINT_UART);
    rx_ct %= rx_len;
  }
}
static void tmr3_config(void)
{
  gpio_init_type gpio_initstructure;
  tmr_output_config_type tmr_oc_init_structure;
  crm_clocks_freq_type crm_clocks_freq_struct = {0};

  /* tmr1 hall interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR3_GLOBAL_IRQn, 1, 0);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* (systemclock/(systemclock/10000))/1000 = 10Hz(100ms) */
  tmr_base_init(TMR3, 999, (crm_clocks_freq_struct.sclk_freq / 1000000 - 1));
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);

  tmr_interrupt_enable(TMR3, TMR_OVF_INT, TRUE);

  tmr_counter_enable(TMR3, TRUE);
}
static u8 ct_m = 0;
void TMR3_GLOBAL_IRQHandler(void)
{
  if (tmr_flag_get(TMR3, TMR_OVF_FLAG) == SET)
  {
    /* add user code... */
    // at32_led_toggle(LED3);
    tmr_flag_clear(TMR3, TMR_OVF_FLAG);
    //at32_led_toggle(LED2);
    ct_m++;
  }
}
void rx_data()
{

  static u16 rx_datalen = 0, ct = 0;

  if (ct++ > 3)
  {
    ct = 0;

    if (rx_ct > 0)
    {
      rx_datalen = rx_ct;
      rx_ct = 0;

      printf(rx_da, rx_datalen);
    }
  }
}
u8 board_loop(void)
{
  static u8 stat = 0;
	static u16 ct = 0,ct1=0;
  u8 a = 0;
  if (stat == 0)
  {

    stat = 1;
    tmr3_config();
    uart_print_init(115200);
    printf("combine_mode_ordinary_smlt_twoslave_dma1 \r\n");
  }
  if (ct_m > 0)
  {
    a = ct_m;
    ct_m = 0;
    ct1++;
    rx_data();
		if(ct++>10000)
		{
			ct=0;
			printf("扫描次数:%d\r\n",ct1);
			
		}
  }

  return a;
}
#define nEDMA_STREAM EDMA_STREAM8
#define EDMA_STREAMn_2D EDMA_STREAM8_2D
#define EDMA_Streamn_IRQn EDMA_Stream8_IRQn
EDMA_STREAMn_2D_stru STREAMn_2D_tab[]=
{
   {4,4,1,0},
   {8,4,1,0},
   {12,4,1,0},
   {16,4,1,0},
};
void moveToux(u16 *dst, u16 *src,u16 len,u16 nType)
{
	edma_init_type edma_init_struct;
  /* enable dma1 clock */
  crm_periph_clock_enable(CRM_EDMA_PERIPH_CLOCK, TRUE);

  /* edma stream1 configuration */
  edma_default_para_init(&edma_init_struct);
  edma_init_struct.direction = EDMA_DIR_MEMORY_TO_MEMORY;
  edma_init_struct.buffer_size = (uint32_t)len;//BUFFER_SIZE;
  edma_init_struct.peripheral_data_width = EDMA_PERIPHERAL_DATA_WIDTH_WORD;
  edma_init_struct.peripheral_base_addr = (uint32_t)src;
  edma_init_struct.peripheral_inc_enable = TRUE;
  edma_init_struct.memory_data_width = EDMA_MEMORY_DATA_WIDTH_WORD;
  edma_init_struct.memory0_base_addr = (uint32_t)dst;
  edma_init_struct.memory_inc_enable = TRUE;
  edma_init_struct.peripheral_burst_mode = EDMA_PERIPHERAL_SINGLE;
  edma_init_struct.memory_burst_mode = EDMA_MEMORY_SINGLE;
  edma_init_struct.fifo_mode_enable = TRUE;
  edma_init_struct.fifo_threshold = EDMA_FIFO_THRESHOLD_FULL;
  edma_init_struct.priority = EDMA_PRIORITY_HIGH;
  edma_init_struct.loop_mode_enable = FALSE;
  edma_init(nEDMA_STREAM, &edma_init_struct);
  /* enable transfer full data intterrupt */
  edma_interrupt_enable(nEDMA_STREAM, EDMA_FDT_INT, TRUE);

  /* edma stream1 interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EDMA_Streamn_IRQn, 1, 0);

  /* two dimension mode init */
  /* source stride = 0x10, destination stride = 0x8, xcnt = 0x2, ycnt = 0x8 */
  STREAMn_2D_tab[nType].ycoutnt=len/2;
  edma_2d_init(EDMA_STREAMn_2D,STREAMn_2D_tab[nType].srcstd, STREAMn_2D_tab[nType].dststd,STREAMn_2D_tab[nType].xcount,STREAMn_2D_tab[nType].ycoutnt);
  edma_2d_enable(EDMA_STREAMn_2D, TRUE);

  edma_stream_enable(nEDMA_STREAM, TRUE);
  data_counter_end=1;
  /* wait the end of transmission */
  




}
void moveTou16(u16 *dst, u16 *src,u16 len)
{


   moveToux(dst, src,len,0);


}
u8 get_edma_2d()
{
  u8 a=0;
   if(data_counter_end==2)
   {
     edma_stream_enable(nEDMA_STREAM, FALSE);
     data_counter_end=0;

   }
   if(data_counter_end>2)data_counter_end=0;
   if(data_counter_end==0)a=1;
   return a;
}
/**
  * @brief  this function handles edma stream1 handler.
  * @param  none
  * @retval none
  */
void EDMA_Stream8_IRQHandler(void)
{
  if(edma_flag_get(EDMA_FDT8_FLAG) != RESET)
  {
    data_counter_end = 2;
    edma_flag_clear(EDMA_FDT8_FLAG);
  }
}
u8  read_ext_in()
{
   u8 a =gpio_input_data_bit_read(GPIOB, (1<<ext_input_pin));
   return a;
}