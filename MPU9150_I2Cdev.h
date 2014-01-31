#include <stdlib.h>
#include <math.h>


#include "twi_master.h"


bool mpu9150_write_Bytes(uint8_t m_device_address, uint8_t register_address, uint8_t length, uint8_t *value);
int8_t mpu9150_read_Bytes(uint8_t m_device_address, uint8_t register_address, uint8_t number_of_bytes, uint8_t *destination);


