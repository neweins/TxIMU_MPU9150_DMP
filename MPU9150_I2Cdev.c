/*
 * 		1st revision : JKLee	2013.09.07
 * 		Description	: I2C communication for MPU9150
 */

#include "MPU9150_I2Cdev.h"

bool mpu9150_write_Bytes(uint8_t m_device_address, uint8_t register_address, uint8_t length, uint8_t *value)
{
	if(length == 1)
	{
		uint8_t w2_data[2];

		w2_data[0] = register_address;
		w2_data[1] = *value;
		return twi_master_transfer(m_device_address, w2_data, 2, TWI_ISSUE_STOP);
	}

	else if(length == 2)
	{
		uint8_t w3_data[3];

		w3_data[0] = register_address;
		w3_data[1] = *value;
		w3_data[2] = *(value+1);
		return twi_master_transfer(m_device_address, w3_data, 3, TWI_ISSUE_STOP);
	}

	else
	{//length > 2
		uint8_t wn_data[length+1];
		int byteCount=0;

		wn_data[0] = register_address;
		for(byteCount=1; byteCount<=length; byteCount++)
		wn_data[byteCount] = *(value++);


		return twi_master_transfer(m_device_address, wn_data, length+1, TWI_ISSUE_STOP);
	}
	return false;
}

int8_t mpu9150_read_Bytes(uint8_t m_device_address, uint8_t register_address, uint8_t number_of_bytes, uint8_t *destination)
{

	bool transfer_succeeded;
	transfer_succeeded = twi_master_transfer(m_device_address, &register_address, 1, TWI_DONT_ISSUE_STOP);	//write
	transfer_succeeded &= twi_master_transfer(m_device_address|TWI_READ_BIT, destination, number_of_bytes, TWI_ISSUE_STOP);	//read
	return transfer_succeeded;
}
