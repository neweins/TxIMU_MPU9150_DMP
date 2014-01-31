TARGET_CHIP := NRF51822_QFAA_CA
BOARD := BOARD_PCA10005
CFLAGS = -Wall


C_SOURCE_FILES += main_tx.c
C_SOURCE_FILES += MPUQuaternion.c
C_SOURCE_FILES += MPU9150Lib.c
C_SOURCE_FILES += radio_config.c
C_SOURCE_FILES += simple_uart.c
C_SOURCE_FILES += twi_master.c
C_SOURCE_FILES += inv_mpu_dmp_motion_driver.c
C_SOURCE_FILES += inv_mpu.c
C_SOURCE_FILES += MPU9150_I2Cdev.c
C_SOURCE_FILES += MPUVector3.c
C_SOURCE_FILES += RTC1_ms_timer.c
C_SOURCE_FILES += nrf_delay.c

OUTPUT_FILENAME := nRF_TxIMU
SDK_PATH = ../../../../


#Uncomment correct line if you have softdevice programmed on the chip.
DEVICE_VARIANT := xxaa
#DEVICE_VARIANT := xxab

#USE_SOFTDEVICE := S110
#USE_SOFTDEVICE := S210

include $(SDK_PATH)Source/templates/gcc/Makefile.common
