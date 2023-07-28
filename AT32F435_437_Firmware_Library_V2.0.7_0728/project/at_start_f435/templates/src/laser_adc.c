#include "user_config.h"

#include "laser_adc.h"
#include "laser_board.h"
static u16 adc1_overflow_flag=0,adc2_overflow_flag=0,adc3_overflow_flag=0,dma1_trans_complete_flag=0;
#define adc1_size (15+8+512)*2
static uint16_t adc1_ordinary_valuetab[adc1_size] = {0};
static uint16_t double_adc1_ordinary_valuetab[adc1_size]={0};
static s16 coms_valuetab[512]={0};
static s16 adc2_preempt_valuetab[8]= {0};
static uint16_t dma_trans_complete_flag = 0;
static int16_t preempt_conversion_count = 0;
static u16 cmos_count = 0;

static laser_cmos_stru gcmos={0};
/*
set_laser_cmos_bit(cmos_st_level,TRUE);
set_laser_cmos_bit(cmos_st_level,FALSE);
is_laser_cmos_bit(cmos_st_en)
*/
#define  set_laser_cmos_bit(stat) (gcmos.stat|=stat)
#define  clr_laser_cmos_bit(stat) (gcmos.stat&=(!stat))
#define  is_laser_cmos_bit(stat)  (gcmos.stat&stat)
//__IO uint32_t adc1_overflow_flag = 0;
__IO uint16_t edma_trans_complete_flag;
__IO uint16_t double_buffer_is_useful;
static void gpio_config(void);
static void tmr1_config(void);
static void dma_config(void);
static void adc_config(void);
#define  video_a0  		0
#define  video_a1  		1
#define  pdv_a3    		3
#define  clk_pa8    	8
#define  clk_pa11    	11
#define  clkin_pa2    2
#define  st_pa6    		6
#define  pwm_pa7    		7
#define  ld_ctrl_pb10    	10
#define  pd_out_pb0    	0
#define  Led2_pd13    	13
#define  Led3_pd14    	14
#define  Led4_pd15    	15
static gpio_stru gpio_cfg[]={
    {video_a0,			GPIOA,GPIO_MODE_ANALOG,0,0 },
		{video_a1,			GPIOA,GPIO_MODE_ANALOG,0,0 },
		{pdv_a3,  			GPIOA,GPIO_MODE_ANALOG,0,0},
		{st_pa6,  			GPIOA,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
		{pwm_pa7, 			GPIOA,GPIO_MODE_OUTPUT,GPIO_OUTPUT_OPEN_DRAIN,GPIO_PULL_UP},
		//{st_pa6,  			GPIOA,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
		{clkin_pa2,  		GPIOA,GPIO_MODE_INPUT,GPIO_PULL_NONE,0},
		{clk_pa8,  			GPIOA,GPIO_MODE_MUX,0,GPIO_MUX_1},
		//{clk_pa11,  		GPIOA,GPIO_MODE_MUX,0,GPIO_MUX_1},
    {ld_ctrl_pb10,  GPIOB,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
		{pd_out_pb0,  	GPIOB,GPIO_MODE_INPUT,GPIO_PULL_NONE,0},
		{Led2_pd13,  		GPIOD,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
		{Led3_pd14,  		GPIOD,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
		{Led4_pd15,  		GPIOD,GPIO_MODE_OUTPUT,GPIO_OUTPUT_PUSH_PULL,0},
    {terminal_pin} 
};
/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
		gpio_inits(gpio_cfg);
		
}
/**
  * @brief  edma configuration.
  * @param  none
  * @retval none
  */
static void edma_config(void)
{
  edma_init_type edma_init_struct;
  crm_periph_clock_enable(CRM_EDMA_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(EDMA_Stream1_IRQn, 0, 0);

  edma_reset(EDMA_STREAM1);
  edma_default_para_init(&edma_init_struct);
  edma_init_struct.buffer_size = adc1_size;
  edma_init_struct.direction = EDMA_DIR_PERIPHERAL_TO_MEMORY;
  edma_init_struct.memory0_base_addr = (uint32_t)adc1_ordinary_valuetab;
  edma_init_struct.memory_burst_mode = EDMA_MEMORY_SINGLE;
  edma_init_struct.memory_data_width = EDMA_MEMORY_DATA_WIDTH_HALFWORD;
  edma_init_struct.memory_inc_enable = TRUE;
  edma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  edma_init_struct.peripheral_burst_mode = EDMA_PERIPHERAL_SINGLE;
  edma_init_struct.peripheral_data_width = EDMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  edma_init_struct.peripheral_inc_enable = FALSE;
  edma_init_struct.priority = EDMA_PRIORITY_VERY_HIGH;
  edma_init_struct.loop_mode_enable = TRUE;
  edma_init_struct.fifo_threshold = EDMA_FIFO_THRESHOLD_1QUARTER;
  edma_init_struct.fifo_mode_enable = FALSE;
  edma_init(EDMA_STREAM1, &edma_init_struct);

  /* edmamux init and enable */
  edmamux_enable(TRUE);
  edmamux_init(EDMAMUX_CHANNEL1, EDMAMUX_DMAREQ_ID_ADC1);

  /* config double buffer mode */
  edma_double_buffer_mode_init(EDMA_STREAM1, (uint32_t)double_adc1_ordinary_valuetab, EDMA_MEMORY_0);

  /* enable the double memory mode */
  edma_double_buffer_mode_enable(EDMA_STREAM1, TRUE);

  /* enable edma full data transfer intterrupt */
  edma_interrupt_enable(EDMA_STREAM1, EDMA_FDT_INT, TRUE);
  edma_stream_enable(EDMA_STREAM1, TRUE);
}
/**
  * @brief  dma configuration.
  * @param  none
  * @retval none
  */
static void dma_config(void)
{
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(DMA1_Channel1_IRQn, 1, 0);

  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 64;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc1_ordinary_valuetab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  dmamux_enable(DMA1, TRUE);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_ADC1);

  /* enable dma transfer complete interrupt */
  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
}

/**
  * @brief  tmr1 configuration.
  * @param  none
  * @retval none
  */
 /* configuration
 PA8:TMR1_CH1
 CLKOUT1 / TMR1_CH1 / I2C3_SCL /
USART1_CK / USART2_TX /
OTGFS1_SOF / SDIO1_D1 / XMC_A4
 PA2 INPUT
 
 
 */
static void tmr1_config(void)
{
  gpio_init_type gpio_initstructure;
  tmr_output_config_type tmr_oc_init_structure;
  crm_clocks_freq_type crm_clocks_freq_struct = {0};

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);

  /* (systemclock/(systemclock/10000))/1000 = 10Hz(100ms) */
  tmr_base_init(TMR1, 29, (crm_clocks_freq_struct.sclk_freq/32000000 - 1));
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;//TMR_OUTPUT_ACTIVE_LOW;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1,15);
  tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
	
	tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_4, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_4,15);
  tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_4, TRUE);
  tmr_output_enable(TMR1, TRUE);
	 /* overflow interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_TRIGGER_INT, TRUE);

  /* tmr1 hall interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_TRG_HALL_TMR11_IRQn, 2, 0);
	tmr_counter_enable(TMR1, TRUE);
}
/*PA2: MUX1 TMR2_CH3
TMR2_CH3 / TMR5_CH3 / TMR9_CH1 /
USART2_TX / SDIO2_CK /
EMAC_MDIO / XMC_D4
*/
static void tmr2_config(void)
{
  gpio_init_type gpio_initstructure;
  tmr_output_config_type tmr_oc_init_structure;
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

    gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode =  GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = GPIO_PINS_8;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = GPIO_PINS_2;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_initstructure);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_1);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);

  /* (systemclock/(systemclock/10000))/1000 = 10Hz(100ms) */
  tmr_base_init(TMR2,31, (crm_clocks_freq_struct.sclk_freq/32000000 - 1));
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_polarity =TMR_OUTPUT_ACTIVE_HIGH;// TMR_OUTPUT_ACTIVE_LOW;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_3, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_3, 15);
  tmr_channel_enable(TMR2, TMR_SELECT_CHANNEL_3, TRUE);
  tmr_output_enable(TMR2, TRUE);
}
static void adc_config(void)
{
 adc_common_config_type adc_common_struct;
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC2_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC3_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(ADC1_2_3_IRQn, 1, 0);

  adc_common_default_para_init(&adc_common_struct);

  /* config combine mode */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* config division,adcclk is division by hclk */
  adc_common_struct.div = ADC_HCLK_DIV_2;

  /* config common dma mode,it's not useful in independent mode */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;

  /* config common dma request repeat */
  adc_common_struct.common_dma_request_repeat_state = FALSE;

  /* config adjacent adc sampling interval,it's useful for ordinary shifting mode */
  adc_common_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;

  /* config inner temperature sensor and vintrv */
  adc_common_struct.tempervintrv_state = FALSE;

  /* config voltage battery */
  adc_common_struct.vbat_state = FALSE;
  adc_common_config(&adc_common_struct);

  adc_base_default_para_init(&adc_base_struct);

  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 2;
  adc_base_config(ADC1, &adc_base_struct);
  adc_resolution_set(ADC1, ADC_RESOLUTION_12B);

  /* config ordinary channel */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_6_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_1, 2, ADC_SAMPLETIME_2_5);
   
  
  /* config ordinary trigger source and trigger edge */
  adc_ordinary_conversion_trigger_set(ADC1, ADC_ORDINARY_TRIG_TMR1CH1, ADC_ORDINARY_TRIG_EDGE_RISING);

  /* config dma mode,it's not useful when common dma mode is use */
  adc_dma_mode_enable(ADC1, TRUE);

  /* config dma request repeat,it's not useful when common dma mode is use */
   adc_dma_request_repeat_enable(ADC1,TRUE);//
//	 adc_preempt_channel_length_set(ADC1, 1);
//   adc_preempt_channel_set(ADC1, ADC_CHANNEL_1, 1, ADC_SAMPLETIME_2_5);
//   adc_preempt_conversion_trigger_set(ADC1, ADC_PREEMPT_TRIG_TMR1CH4, ADC_ORDINARY_TRIG_EDGE_NONE);
//	 adc_preempt_auto_mode_enable(ADC1, TRUE);
	
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 1;
  adc_base_config(ADC2, &adc_base_struct);
  adc_resolution_set(ADC2, ADC_RESOLUTION_12B);
  //adc_ordinary_channel_set(ADC2, ADC_CHANNEL_10, 1, ADC_SAMPLETIME_6_5);
  //adc_ordinary_conversion_trigger_set(ADC2, ADC_ORDINARY_TRIG_TMR1CH1, ADC_PREEMPT_TRIG_EDGE_RISING);

   /* config preempt channel */
  adc_preempt_channel_length_set(ADC2, 1);
  adc_preempt_channel_set(ADC2, ADC_CHANNEL_10, 1,ADC_SAMPLETIME_2_5);
  //adc_preempt_channel_set(ADC2, ADC_CHANNEL_0, 2, ADC_SAMPLETIME_6_5);
  //adc_preempt_channel_set(ADC1, ADC_CHANNEL_1, 3, ADC_SAMPLETIME_2_5);

  /* config preempt trigger source and trigger edge */
  adc_preempt_conversion_trigger_set(ADC2, ADC_PREEMPT_TRIG_TMR1CH4, ADC_PREEMPT_TRIG_EDGE_RISING);
	adc_preempt_auto_mode_enable(ADC2, FALSE);
  /* enable adc overflow interrupt */
  adc_interrupt_enable(ADC1, ADC_OCCO_INT, TRUE);

  /* enable adc preempt channels conversion end interrupt */
  adc_interrupt_enable(ADC2, ADC_PCCE_INT, TRUE);

  /* adc enable */
  adc_enable(ADC1, TRUE);
	adc_enable(ADC2, TRUE);
  while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);
  
  while(adc_flag_get(ADC2, ADC_RDY_FLAG) == RESET);

  /* adc calibration */
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
  /* adc calibration */
  adc_calibration_init(ADC2);
  while(adc_calibration_init_status_get(ADC2));
  adc_calibration_start(ADC2);
  while(adc_calibration_status_get(ADC2));
}


void laser_Init(void)
{
	 gpio_config();
	 tmr1_config();
   edma_config();
   adc_config();
	 //uart_print_init(115200);
   gcmos.dacval=DAC_VAL_H;
   set_bits(gcmos.stat,laser_sw_en);
}
/**
  * @brief  this function handles edma_stream1 handler.
  * @param  none
  * @retval none
  */
void EDMA_Stream1_IRQHandler(void)
{
	u32 status = EDMA->sts1;
	if(is_bits(gcmos.stat,laser_sw_en))
	{
	  clr_bits(gcmos.stat,cmos_st_level);
		GPIOA->odt|=(1<<pwm_pa7);
		if(adc2_preempt_valuetab[4]>DV_400MV){if(gcmos.dacval>DAC_VAL_L)gcmos.dacval--;}
    if(adc2_preempt_valuetab[4]<(DV_400MV-50)){if(gcmos.dacval<DAC_VAL_H)gcmos.dacval++;}
	}
	cmos_count=0;
//is_laser_cmos_bit(cmos_st_level)
  if(status&EDMA_FDT1_FLAG) 
  {
    if(EDMA_STREAM1->ctrl_bit.cm)//edma_memory_target_get(EDMA_STREAM1))
    {
      double_buffer_is_useful = 0;
			GPIOD->odt |= 0x00002000;
    }
    else
    {
      double_buffer_is_useful = 1;
			GPIOD->odt &= ~0x00002000;
    }
		EDMA->clr1 = EDMA_FDT1_FLAG;
    //edma_flag_clear(EDMA_FDT1_FLAG);
    edma_trans_complete_flag = 1;
  }
}
void DMA1_Channel1_IRQHandler(void)
{
  if(dma_flag_get(DMA1_FDT1_FLAG) != RESET)
  {
    dma_flag_clear(DMA1_GL1_FLAG);
    dma_trans_complete_flag = 1;
  }
}

/**
  * @brief  this function handles adc1_2_3 handler.
  * @param  none
  * @retval none
  */
void ADC1_2_3_IRQHandler(void)
{
  
	//volatile s16 val=0,a=0;
	
  //GPIOD->odt |= 0x00002000;
  if(ADC2->sts &ADC_PCCE_FLAG) 
  {
		  ADC2->sts &=~ADC_PCCE_FLAG;
		  adc2_preempt_valuetab[1]=adc2_preempt_valuetab[0];
      adc2_preempt_valuetab[0]= (uint16_t)(ADC2->pdt1_bit.pdt1);
		  if(is_bits(gcmos.stat,cmos_st_level))GPIOA->odt|=(1<<st_pa6);
		  else GPIOA->odt&=~(1<<st_pa6);
      if(is_bits(gcmos.stat,laser_sw_en))
      {
		  if(preempt_conversion_count++>=2)
			{
				TMR1->swevt|=TMR_TRIGGER_SWTRIG;
				preempt_conversion_count=0;		
			}
			
      if(cmos_count==14)
			{
				GPIOA->odt&=!(1<<pwm_pa7);
				set_bits(gcmos.stat,cmos_st_level);
			}
      }
		   cmos_count++;
  }
	if(ADC1->sts& ADC_OCCO_FLAG)
  {
		ADC1->sts = ~ADC_OCCO_FLAG;
    adc1_overflow_flag++;
  }
			
}
void cmos_calc()
{
	static u8 ct=0;
	u16 *p,i=0;
  if(is_bits(gcmos.stat,laser_sw_en))GPIOB->odt|=(1<<ld_ctrl_pb10);
  else GPIOB->odt&=!(1<<ld_ctrl_pb10);

	if(edma_trans_complete_flag==0)return;
	 edma_trans_complete_flag=0;
	 if(ct<2){ct++;return;}
	 
	 if(double_buffer_is_useful==0)
	 {
		 
		 p=&double_adc1_ordinary_valuetab[0];
		 
	 }
	 else
	 {
		 
		 p=&adc1_ordinary_valuetab[0];
	 }
	 for(i=12;(i-12)<512;i++)
	 {
		 coms_valuetab[i-12]=(*(p+2*i)+*(p+2*i+1))/2;
		 
	 }
	
	
}
void TMR1_TRG_HALL_TMR11_IRQHandler(void)
{
	s16 val=0;
	s16 a=0;
	//GPIOD->odt |= 0x00002000;
  if(TMR1->ists& TMR_TRIGGER_FLAG)
  {
		TMR1->ists &= ~TMR_TRIGGER_FLAG;
		val=(adc2_preempt_valuetab[1]+adc2_preempt_valuetab[0])>>1;
		a=val-adc2_preempt_valuetab[3];
		if(a>200)
		{
			adc2_preempt_valuetab[4]=a;
			
		}
		else if(a<-200)
		{
		   adc2_preempt_valuetab[5]=a;
			 clr_bits(gcmos.stat,cmos_st_level);
		}
		
   // tmr_flag_clear(TMR1, TMR_OVF_FLAG);
	//	GPIOD->odt &= ~0x00002000;
//		a=val-adc2_preempt_valuetab[3];
//				val=(adc2_preempt_valuetab[1]+adc2_preempt_valuetab[0])>>1;
				
//				adc2_preempt_valuetab[4]=a;
//				preempt_conversion_count=0;
		
  }
}
u8* open_laser_calc(void)
{
	    static u8 stat=0;
	    cmos_calc();
	    return (u8*)&gcmos;
}