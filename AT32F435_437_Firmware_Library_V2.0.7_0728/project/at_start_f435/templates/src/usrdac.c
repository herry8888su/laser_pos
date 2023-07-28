#include "user_config.h"
#include "usrdac.h"
gpio_init_type  gpio_init_struct = {0};
dma_init_type dma_init_struct;
crm_clocks_freq_type crm_clocks_freq_struct = {0};
const uint16_t sine12bit[32] = {2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056,
                                4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
                                2047, 1647, 1263, 909,  599,  344,  155,  38,
                                0,    38,   155,  344,  599,  909,  1263, 1647};
uint32_t dualsine12bit[32];
uint32_t idx = 0;
void dma_dac12_Init()
{
	gpio_init_type gpio_init_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};
     /* enable dac/tmr2/gpioa clock */
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* once the dac is enabled, the corresponding gpio pin is automatically
     connected to the dac converter. in order to avoid parasitic consumption,
     the gpio pin should be configured in analog */
  gpio_init_struct.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* (systemclock/(systemclock/1000000))/100 = 10KHz */
  tmr_base_init(TMR2, 99, (crm_clocks_freq_struct.sclk_freq/1000000 - 1));
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);

  /* primary tmr2 output selection */
  tmr_primary_mode_select(TMR2, TMR_PRIMARY_SEL_OVERFLOW);

  /* dac1 and dac2 configuration */
  dac_trigger_select(DAC1_SELECT, DAC_TMR2_TRGOUT_EVENT);
  dac_trigger_select(DAC2_SELECT, DAC_TMR2_TRGOUT_EVENT);

  dac_trigger_enable(DAC1_SELECT, TRUE);
  dac_trigger_enable(DAC2_SELECT, TRUE);

  dac_wave_generate(DAC1_SELECT, DAC_WAVE_GENERATE_NONE);
  dac_wave_generate(DAC2_SELECT, DAC_WAVE_GENERATE_NONE);

  dac_output_buffer_enable(DAC1_SELECT, FALSE);
  dac_output_buffer_enable(DAC2_SELECT, FALSE);

  dac_dma_enable(DAC1_SELECT, TRUE);
  dac_dma_enable(DAC2_SELECT, TRUE);

  /* fill sine32bit table */
  for(idx = 0; idx < 32; idx++)
  {
    dualsine12bit[idx] = (sine12bit[idx] << 16) + (sine12bit[idx]);
  }

  /* dma1 channel1 configuration */
  dma_reset(DMA1_CHANNEL1);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)dualsine12bit;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_WORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)0x40007420;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_WORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  /* enable dmamux function */
  dmamux_enable(DMA1, TRUE);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_DAC2);

  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* enable dac1: once the dac1 is enabled, pa.04 is
     automatically connected to the dac converter. */
  dac_enable(DAC1_SELECT, TRUE);

  /* enable dac2: once the dac2 is enabled, pa.05 is
     automatically connected to the dac converter. */
  dac_enable(DAC2_SELECT, TRUE);

  /* enable tmr2 */
  tmr_counter_enable(TMR2, TRUE);


}
void dac12_Init()
{

  /* enable dac/tmr2/gpioa clock */
  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* once the dac is enabled, the corresponding gpio pin is automatically
     connected to the dac converter. in order to avoid parasitic consumption,
     the gpio pin should be configured in analog */
  gpio_init_struct.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* (systemclock/(systemclock/1000000))/100 = 10KHz */
  tmr_base_init(TMR2, 99, (crm_clocks_freq_struct.sclk_freq/1000000 - 1));
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);

  /* primary tmr2 output selection */
  tmr_primary_mode_select(TMR2, TMR_PRIMARY_SEL_OVERFLOW);

  /* dac1 and dac2 configuration */
  dac_trigger_select(DAC1_SELECT, DAC_TMR2_TRGOUT_EVENT);
  dac_trigger_select(DAC2_SELECT, DAC_TMR2_TRGOUT_EVENT);

  dac_trigger_enable(DAC1_SELECT, TRUE);
  dac_trigger_enable(DAC2_SELECT, TRUE);

  dac_wave_generate(DAC1_SELECT, DAC_WAVE_GENERATE_TRIANGLE);
  dac_wave_generate(DAC2_SELECT, DAC_WAVE_GENERATE_TRIANGLE);

  dac_mask_amplitude_select(DAC1_SELECT, DAC_LSFR_BIT90_AMPLITUDE_1023);
  dac_mask_amplitude_select(DAC2_SELECT, DAC_LSFR_BITA0_AMPLITUDE_2047);

  dac_output_buffer_enable(DAC1_SELECT, FALSE);
  dac_output_buffer_enable(DAC2_SELECT, FALSE);

  dac_dma_enable(DAC1_SELECT, TRUE);
  dac_dma_enable(DAC2_SELECT, TRUE);

  /* enable dac1: once the dac1 is enabled, pa.04 is
     automatically connected to the dac converter. */
  dac_enable(DAC1_SELECT, TRUE);

  /* enable dac2: once the dac2 is enabled, pa.05 is
     automatically connected to the dac converter. */
  dac_enable(DAC2_SELECT, TRUE);
  dac_dual_data_set(DAC_DUAL_12BIT_RIGHT, 0x100, 0x100);

  /* enable tmr2 */
  tmr_counter_enable(TMR2, TRUE);
  }

	
	void set_dac1(u16 v)
	{
		
		dac_1_data_set(DAC1_12BIT_RIGHT, v);
	}
	
	void set_dac2(u16 v)
	{
		dac_2_data_set(DAC2_12BIT_RIGHT, v);
		
	}
	
#define SETv_STB(x)  ((x==1)?(GPIOA->odt|=GPIO_PINS_9):(GPIOA->odt&=!GPIO_PINS_9))
#define SETv_CLK(x)  ((x==1)?(GPIOA->odt|=GPIO_PINS_8):(GPIOA->odt&=!GPIO_PINS_8))
#define SETv_DATA(x) ((x==1)?(GPIOB->odt|=GPIO_PINS_15):(GPIOB->odt&=!GPIO_PINS_15))
#define SETi_STB(x)  ((x==1)?(GPIOB->odt|=GPIO_PINS_14):(GPIOB->odt&=!GPIO_PINS_14))
#define SETi_CLK(x)  ((x==1)?(GPIOB->odt|=GPIO_PINS_13):(GPIOB->odt&=!GPIO_PINS_13))
#define SETi_DATA(x) ((x==1)?(GPIOB->odt|=GPIO_PINS_12):(GPIOB->odt&=!GPIO_PINS_12))
/*SET_DATA_IO(DA_PIN_OUTPUT) */
//#define SET_DATA_IO(x) (s_set_bits(GPIOB->cfgr,x,PIN_DIO,PIN_DIR_MSK))  //(r,v,p,msk) 	
#define SET_STB(s,x)  ((s==1)?SETi_STB(x):SETv_STB(x))
#define SET_CLK(s,x)  ((s==1)?SETi_CLK(x):SETv_CLK(x))
#define SET_DATA(s,x) ((s==1)?SETi_DATA(x):SETv_DATA(x))

#define DAC_V  0
#define DAC_I  1
typedef enum
{
 
  NORM_DAC   = 0,
  DAC_OUTPUT_1K,     
  DAC_OUTPUT_100K, 
  DAC_OUTPUT_HighZ, 
             
} dacvi_stat_ctrl_selection_type;  
void delay_dac(u16 de)
{
  while(de>0)de--;


}
void DAC_VI_Write(u8 nDac,u8 mode,u16 da)
{
    u8 i=0;
    da=(da&0x3FFF)|(u16)(mode<<14);
    SET_STB(nDac,0);
    delay_dac(20);
    for(;i<16;i++)
    {
       
       if(da&1){SET_DATA(nDac,1);}
       else
       {SET_DATA(nDac,0);}
       delay_dac(10);
       SET_CLK(nDac,0);
       delay_dac(10);
       SET_CLK(nDac,1);
       delay_dac(10);
       da>>=1;

    }


    SET_DATA(nDac,1);
    delay_dac(20);
    SET_STB(nDac,1);
}
void set_dacv(u16 da)
{
   DAC_VI_Write(DAC_V,0,da);



}
void set_daci(u16 da)
{
   DAC_VI_Write(DAC_I,0,da);



}
void set_dacv_1k(u16 da)
{
   DAC_VI_Write(DAC_OUTPUT_1K,0,da);



}
void set_daci_1k(u16 da)
{
   DAC_VI_Write(DAC_OUTPUT_1K,0,da);



}
	void Dac114_init()
{

	// Pb9 dio , pa10 clk , pa11 ,stb  pa12
   // gpio_init_type gpio_init_struct;

		/* enable the led clock */
		crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
		crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
		/* set default parameter */
		gpio_default_para_init(&gpio_init_struct);
		/* configure the led gpio */
		gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
		gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
		gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
		gpio_init_struct.gpio_pins = GPIO_PINS_9|GPIO_PINS_8;
		gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
		gpio_init(GPIOA, &gpio_init_struct);
		gpio_init_struct.gpio_pins = GPIO_PINS_15|GPIO_PINS_14|GPIO_PINS_13|GPIO_PINS_12;
		gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
		gpio_init(GPIOB, &gpio_init_struct);
		// dio((x==1):(GPIOB->odt|=GPIO_PINS_9):(GPIOB->odt&=!GPIO_PINS_9))
		// clk((x==1):(GPIOA->odt|=GPIO_PINS_10):(GPIOA->odt&=!GPIO_PINS_10))
		// stb((x==1):(GPIOB->odt|=GPIO_PINS_11):(GPIOA->odt&=!GPIO_PINS_11))
    SET_STB(1,1);
    SET_CLK(1,1);
    SET_DATA(1,1);
    SET_STB(0,1);
    SET_CLK(0,1);
    SET_DATA(0,1);
    set_dacv_1k(500);
    set_daci_1k(200);
}
	