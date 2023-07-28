#define STEP_DELAY_MS                    50
/* delay variable */
static __IO uint32_t fac_us;
static __IO uint32_t fac_ms;
static void delay_init()
{
  /* configure systick */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);
  fac_us = system_core_clock / (1000000U);
  fac_ms = fac_us * (1000U);
}

/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
static void delay_ms(uint16_t nms)
{
  uint32_t temp = 0;
  while(nms)
  {
    if(nms > STEP_DELAY_MS)
    {
      SysTick->LOAD = (uint32_t)(STEP_DELAY_MS * fac_ms);
      nms -= STEP_DELAY_MS;
    }
    else
    {
      SysTick->LOAD = (uint32_t)(nms * fac_ms);
      nms = 0;
    }
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
      temp = SysTick->CTRL;
    }while((temp & 0x01) && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
  }
}


static void start_nms(uint16_t nms)
{

      delay_init();
      SysTick->LOAD = (uint32_t)(nms * fac_ms);



}
static u8 check_nms(uint16_t nms)
{
    static u8 stat=0;
    u8    a=0;
   uint32_t temp = 0;
    if(stat==0)
    {

        stat=1;
        return a;
    }
   
   temp = SysTick->CTRL;
   if((temp & 0x01) && !(temp & (1 << 16))==0)
   {
      SysTick->VAL = 0x00;
      SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
      a=1;

   }




}
