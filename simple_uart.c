/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>


#include "nrf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

uint8_t simple_uart_get(void)
{
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    // Wait for RXD data to be received

  }
  
  NRF_UART0->EVENTS_RXDRDY = 0;
  return (uint8_t)NRF_UART0->RXD;
}

uint8_t simple_uart_C_get(unsigned char *data)
{
	if(NRF_UART0->EVENTS_RXDRDY == 1)
	{
		NRF_UART0->EVENTS_RXDRDY = 0;
		*data = (uint8_t)NRF_UART0->RXD;
		return 1;
	}
	return 0;
}

bool simple_uart_get_with_timeout(int32_t timeout_ms, uint8_t *rx_data)
{
  bool ret = true;
  
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    if (timeout_ms-- >= 0)
    {
      // wait in 1ms chunk before checking for status
      nrf_delay_us(1000);
    }
    else
    {
      ret = false;
      break;
    }
  }  // Wait for RXD data to be received

  if (timeout_ms >= 0)
  {
    // clear the event and set rx_data with received byte
      NRF_UART0->EVENTS_RXDRDY = 0;
      *rx_data = (uint8_t)NRF_UART0->RXD;
  }

  return ret;
}

//2,8,10,16진수 음수까지 변환되는 코드
char* UsrItoaS(int32_t value, char *str, int n)
{//User Integer to Ascii String

	bool sign=false;

	if(value & 0x80000000)
	{//음수라면
		sign=true;
		value=-value; // 보수를 취해서 양수로 바꿈.
	}

	char *p=str;

	do
	{
		*p=value % n +'0'; // n값으로 나눈 나머지값을 버퍼에 저장

		if(*p>'9') // 9보다 크다면 (16진수를 대비한것)
			*p+=7; // +7을 해서 ABCDEF를 표시하도록함

		p++; // 다음 값을 저장을 위해 증가

	}while(0!=(value=value/n)); // value를n으로 나눔 , 그값이 0이면 루프 종료.

	if(sign) *p++='-'; // 음수였다면 - 추가
	*p=0;

	return UsrStrRev(str);// 거꾸로 저장되어있으므로 반전..
}

//문자열을 거꾸로 뒤집기
char* UsrStrRev(char *str)
{
	int strlength, halfstrlength, EndofStr,temp;
	int i, j;

	strlength = strlen(str);
	if(strlength == 0)
		return 0;	//문자열이 없으면 0을 리턴

	EndofStr = strlength - 1;	//j는 문자열의 끝문자를 indexing 함.
	halfstrlength = strlength>>1;	//오른쪽으로 1bit --> shift는 n/2와 동일

	j = EndofStr;
	//컴파일된 실행파일은 n/2 보다 n>>1이 더 빠르다.
	//하지만, 어차피 최적화옵션으로 컴파일시에 n/2는 n>>1과 같게 컴파일된다.
	for(i=0; i<halfstrlength; i++, j--)
	{// swapping
		temp = str[i];
		str[i]=str[j];
		str[j]=temp;
	}
	return str;
}


void simple_uart_put(uint8_t cr)
{
  NRF_UART0->TXD = (uint32_t)cr;

  while (NRF_UART0->EVENTS_TXDRDY!=1)
  {
    // Wait for TXD data to be sent
  }

  NRF_UART0->EVENTS_TXDRDY=0;
}

void simple_uart_putstring(const uint8_t *str)
{
  uint_fast8_t i = 0;
  uint8_t ch = str[i++];
  while (ch != '\0')
  {
    simple_uart_put(ch);
    ch = str[i++];
  }
}

void simple_uart_putinteger10(int32_t value)
{
	char str[20];
	simple_uart_putstring((const uint8_t *)UsrItoaS(value, str, 10));
}

void simple_uart_putinteger16(int32_t value)
{
	char str[20];
	simple_uart_putstring((const uint8_t *)UsrItoaS(value, str, 16));
}

void serialPrintFloatArr(float* arr, int length)
{
	float *data=arr;

	for(int i=0; i<length; i++)
	{
		data[i]=data[i]/1073741824.0f; // = 1024^3
		serialFloatPrint(data[i]);
		simple_uart_putstring((const uint8_t *)";"); // have to same as processing string format
	}
}

void serialFloatPrint(float f)
{//output from float to hexadecimal
	uint8_t* b = (uint8_t*) &f;

	for (int i=0; i<4; i++)
	{
		uint8_t b1 = (b[i] >> 4) & 0x0f;
		uint8_t b2 = (b[i] & 0x0f);

		uint8_t c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
		uint8_t c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;

		simple_uart_put(c1);
		simple_uart_put(c2);
	}
}

void simple_uart_config(  uint8_t rts_pin_number,
                          uint8_t txd_pin_number,
                          uint8_t cts_pin_number,
                          uint8_t rxd_pin_number,
                          bool hwfc)
{
  nrf_gpio_cfg_output(txd_pin_number);
  nrf_gpio_cfg_input(rxd_pin_number, NRF_GPIO_PIN_NOPULL);  

  NRF_UART0->PSELTXD = txd_pin_number;
  NRF_UART0->PSELRXD = rxd_pin_number;

  if (hwfc)
  {
    nrf_gpio_cfg_output(rts_pin_number);
    nrf_gpio_cfg_input(cts_pin_number, NRF_GPIO_PIN_NOPULL);
    NRF_UART0->PSELCTS = cts_pin_number;
    NRF_UART0->PSELRTS = rts_pin_number;
    NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
  }

  NRF_UART0->BAUDRATE         = (UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos);
  NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
  NRF_UART0->TASKS_STARTTX    = 1;
  NRF_UART0->TASKS_STARTRX    = 1;
  NRF_UART0->EVENTS_RXDRDY    = 0;
}
