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


// Selection value type - by JKLee 20130817
//#define PACKETTXTEST
#define QAUTERNIONVALUETX
//end - by JKLee

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "twi_master.h"
#include "nrf.h"
#include "radio_config.h"
#include "nrf_gpio.h"
#include "simple_uart.h"
#include "boards/pca10005.h"
#include "nrf_delay.h"
#include "twi_master_config.h"
#include "RTC1_ms_timer.h"
#include "mpu9150Lib.h"


#define Max_Length 255
#define MPU_UPDATE_RATE (10)	//  MPU_UPDATE_RATE defines the rate (in Hz) at which the MPU updates the sensor data and DMP output
#define MPU_MAG_MIX_GYRO_AND_MAG (10)	// a good mix value

static uint8_t packet[PACKET_PAYLOAD_MAXSIZE];  ///< Packet to transmit


#ifdef QAUTERNIONVALUETX
float q[4];
#endif

void init(void)
{
//	/* Start 16 MHz crystal oscillator */
//	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;					//  Clear it so we can sense when it changes
//	NRF_CLOCK->TASKS_HFCLKSTART = 1;					//  Start oscillator
//
//	/* Wait for the external oscillator to start up */
//	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)			//  Wait for oscillator to start
//	{
//	}
//	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;					//  Clean up

	//Set 1ms timer
	setup_RTC1_1ms_timer();

	//Set LED GPIO
	nrf_gpio_range_cfg_output(LED_START, LED_STOP);
	nrf_delay_ms(3);

	// Initialize I2C
	if(1 != twi_master_init())
		simple_uart_putstring((const uint8_t *)"\n\rFailed initialize I2C !\n\r");
	nrf_delay_ms(3);

	// Set radio configuration parameters
	radio_configure();
	// Set payload pointer
	NRF_RADIO->PACKETPTR = (uint32_t)packet;

	// Set UART parameters
	simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);

	//MPU9150 initialize
	if (MPU9150_init(MPU_UPDATE_RATE, MPU_MAG_MIX_GYRO_AND_MAG, 10, 0) == true)
	{
		simple_uart_putstring((const uint8_t *)"\n\rMPU9150 Initialize success\n\r");
		nrf_delay_ms(50);

	}
	else
	{
		nrf_gpio_pin_toggle(LED1);
		nrf_delay_ms(50);
		simple_uart_putstring((const uint8_t *)"\n\r");
		simple_uart_putstring((const uint8_t *)"MPU9150 Initialize failed\n\r");
	}
}

// Transmit packet based interrupt
void Radio_Tx()
{
	while (NRF_RADIO->EVENTS_READY == 0U)
	{
	}

    // Start transmission and wait for end of packet.
    NRF_RADIO->TASKS_START = 1U;

	NRF_RADIO->EVENTS_END = 0U;
	while(NRF_RADIO->EVENTS_END == 0U)
	{
	}

	#ifdef QAUTERNIONVALUETX

	// TO DO : Insert your code !!

	#endif

	NRF_RADIO->EVENTS_DISABLED = 0U;

	// Disable radio
	NRF_RADIO->TASKS_DISABLE = 1U;
	while(NRF_RADIO->EVENTS_DISABLED == 0U)
	{
	}
}

//For packet transmit test
#ifdef PACKETTXTEST
void Packet_Tx_Test()
{
	nrf_gpio_pin_toggle(LED1);

	packet[0] = '0';
	simple_uart_putstring((const uint8_t *)"Tx packet>");
	simple_uart_put(packet[0]);
	nrf_delay_ms(100);
	Radio_Tx();


	packet[0] = '1';
	simple_uart_putstring((const uint8_t *)"Tx packet>");
	simple_uart_put(packet[0]);
	nrf_delay_ms(100);
	Radio_Tx();
}
#endif

int main(void)
{
	init();
	nrf_delay_ms(10);

	NRF_RADIO->EVENTS_READY = 0U;
	// Enable radio and wait for ready
	NRF_RADIO->TASKS_TXEN = 1;
	nrf_delay_ms(3);

	while(true)
	{
		// Transmit Packet
		Radio_Tx();


		if(MPU9150_read())
		{
			#ifdef PACKETTXTEST
			Packet_Tx_Test();
			#endif

			#ifdef QAUTERNIONVALUETX
//			serialPrintFloatArr((float *)m_rawQuaternion, 4);	// free IMU packet :  serial transmit to PC
//			simple_uart_putstring((const uint8_t *)"\n\r");
//			simple_uart_putstring((const uint8_t *)"m_rawQuaternion>");	// print the raw quaternion from the dmp
//			printQuaternionlong(m_rawQuaternion);
//			simple_uart_putstring((const uint8_t *)"\n\r");	//
			simple_uart_putstring((const uint8_t *)"Mag>");	// print the raw mag data
			printVectorshort(m_rawMag);
			simple_uart_putstring((const uint8_t *)"\n\r");	//
			simple_uart_putstring((const uint8_t *)"Gyro>");	// print the raw accel data
			printVectorshort(m_rawGyro);
			simple_uart_putstring((const uint8_t *)"\n\r");	//
			simple_uart_putstring((const uint8_t *)"Accel>");	// print the raw accel data
			printVectorshort(m_rawAccel);
			simple_uart_putstring((const uint8_t *)"\n\r");	//
//			simple_uart_putstring((const uint8_t *)"m_dmpEulerPose>");	// the Euler angles from the dmp quaternion
//			printAngles(m_dmpEulerPose);
//			simple_uart_putstring((const uint8_t *)"\n\r");	//
//			simple_uart_putstring((const uint8_t *)"m_fusedEulerPose>");	// print the output of the data fusion
//			printAngles(m_fusedEulerPose);
//			simple_uart_putstring((const uint8_t *)"\n\r");
			#endif

			nrf_gpio_pin_toggle(LED1);
			nrf_delay_ms(50);
		}
		else
		{//exception process
			simple_uart_putstring((const uint8_t *)"Tx packet>");
			simple_uart_putstring((const uint8_t *)"I cannot read mpu9150 register!\n\r");
			nrf_delay_ms(1000);
		}
	}
}


/**
 *@}
 **/
