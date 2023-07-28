/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.7
  * @date     2022-04-02
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_I2S_halfduplex_interrupt I2S_halfduplex_interrupt
  * @{
  */

uint16_t i2s3_buffer_tx[32] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                               0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                               0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                               0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                               0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                               0x3D3E, 0x3F40};
uint16_t i2s2_buffer_rx[32];
__IO uint32_t tx_index = 0, rx_index = 0;
volatile error_status transfer_status1 = ERROR, transfer_status2 = ERROR;

static void gpio_config(void);
static void i2s_config(i2s_data_channel_format_type format, i2s_audio_sampling_freq_type freq);
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length);
error_status buffer_compare_24bits(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length);

/**
  * @brief  buffer_compare function.
  * @param  none
  * @retval the result of compare
  */
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return ERROR;
    }

    pbuffer1++;
    pbuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  buffer_compare_24bits function.
  * @param  none
  * @retval the result of compare
  */
error_status buffer_compare_24bits(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      if(*pbuffer1 != (*pbuffer2 & 0xFF00))
      {
        return ERROR;
      }
    }

    pbuffer1++;
    pbuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(i2s_data_channel_format_type format, i2s_audio_sampling_freq_type freq)
{
  i2s_init_type i2s_init_struct;

  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);

  nvic_irq_enable(SPI3_I2S3EXT_IRQn, 0, 0);
  nvic_irq_enable(SPI2_I2S2EXT_IRQn, 0, 0);
  spi_i2s_reset(SPI2);
  spi_i2s_reset(SPI3);
  i2s_default_para_init(&i2s_init_struct);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = format;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = freq;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(SPI3, &i2s_init_struct);

  i2s_init_struct.operation_mode =I2S_MODE_SLAVE_RX;
  i2s_init(SPI2, &i2s_init_struct);

  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, TRUE);
  spi_i2s_interrupt_enable(SPI3, SPI_I2S_TDBE_INT, TRUE);
  i2s_enable(SPI2, TRUE);
  i2s_enable(SPI3, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);

  /* master ws pin */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_initstructure);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE15, GPIO_MUX_6);

  /* master ck pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins           = GPIO_PINS_3;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE3, GPIO_MUX_6);

  /* master sd pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins           = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_initstructure);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE5, GPIO_MUX_6);

  /* master mck pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins           = GPIO_PINS_7;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE7, GPIO_MUX_6);

  /* slave ws pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins           = GPIO_PINS_0;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE0, GPIO_MUX_7);

  /* slave ck pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins           = GPIO_PINS_1;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE1, GPIO_MUX_6);

  /* slave sd pin */
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins           = GPIO_PINS_4;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE4, GPIO_MUX_6);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t index = 0;
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  at32_led_off(LED2);
  at32_led_off(LED3);
  at32_led_off(LED4);
  gpio_config();
  i2s_config(I2S_DATA_16BIT_CHANNEL_32BIT, I2S_AUDIO_FREQUENCY_48K);
  while(rx_index < 32);

  /* test result:the data check */
  transfer_status1 = buffer_compare(i2s2_buffer_rx, i2s3_buffer_tx, 32);

  for(index = 0; index < 32; index++)  i2s2_buffer_rx[index] = 0;
  tx_index = 0;
  rx_index = 0;

  i2s_config(I2S_DATA_24BIT_CHANNEL_32BIT, I2S_AUDIO_FREQUENCY_16K);
  while(rx_index < 32);

  /* test result:the data check */
  transfer_status2 = buffer_compare_24bits(i2s2_buffer_rx, i2s3_buffer_tx, 32);

  /* test result indicate:if success ,led2 lights */
  if((transfer_status1 == SUCCESS) && (transfer_status2 == SUCCESS))
  {
    at32_led_on(LED2);
  }
  else
  {
    at32_led_off(LED2);
  }

  while(1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */
