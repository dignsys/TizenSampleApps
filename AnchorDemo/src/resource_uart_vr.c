/*
 * Copyright (c) 2019 DIGNSYS Inc.
 *
 * Contact: Hyobok Ahn (hbahn@dignsys.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <peripheral_io.h>
#include <system_info.h>
#include <unistd.h>
#include <app_common.h>
#include "main.h"
#include "vr3.h"


#define UART_PORT_ANCHOR3		1	// ANCHOR3 : UART1
#define MAX_TRY_COUNT			10
#define MAX_FRAME_LEN			32

int incoming_byte = 0;          // for incoming serial data
char frame_buf[MAX_FRAME_LEN];  // for save protocol data
int frame_len = MAX_FRAME_LEN;  // length of frame
int byte_position = 0;          // next byte position in frame_buf
unsigned int calc_checksum = 0; // to save calculated checksum value

static bool initialized = false;
static peripheral_uart_h g_uart_h;

/*
 * open UART port and set UART handle resource
 * set BAUD rate, byte size, parity bit, stop bit, flow control
 * Default baud rate：9600bps Check bit：None Stop bit：1 bit
 */
bool resource_serial_init(void)
{
	if (initialized) return true;

	LOGI("----- resource_serial_init -----");
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	// Opens the UART slave device
	ret = peripheral_uart_open(UART_PORT_ANCHOR3, &g_uart_h);
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("UART port [%d] open Failed, ret [%d]", UART_PORT_ANCHOR3, ret);
		return false;
	}
	// Sets baud rate of the UART slave device.
	ret = peripheral_uart_set_baud_rate(g_uart_h, PERIPHERAL_UART_BAUD_RATE_9600);	// The number of signal in one second is 9600
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("uart_set_baud_rate set Failed, ret [%d]", ret);
		return false;
	}
	// Sets byte size of the UART slave device.
	ret = peripheral_uart_set_byte_size(g_uart_h, PERIPHERAL_UART_BYTE_SIZE_8BIT);	// 8 data bits
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("byte_size set Failed, ret [%d]", ret);
		return false;
	}
	// Sets parity bit of the UART slave device.
	ret = peripheral_uart_set_parity(g_uart_h, PERIPHERAL_UART_PARITY_NONE);	// No parity is used
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("parity set Failed, ret [%d]", ret);
		return false;
	}
	// Sets stop bits of the UART slave device
	ret = peripheral_uart_set_stop_bits (g_uart_h, PERIPHERAL_UART_STOP_BITS_1BIT);	// One stop bit
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("stop_bits set Failed, ret [%d]", ret);
		return false;
	}
	// Sets flow control of the UART slave device.
	// No software flow control & No hardware flow control
	ret = peripheral_uart_set_flow_control (g_uart_h, PERIPHERAL_UART_SOFTWARE_FLOW_CONTROL_NONE, PERIPHERAL_UART_HARDWARE_FLOW_CONTROL_NONE);
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("flow control set Failed, ret [%d]", ret);
		return false;
	}

	initialized = true;
	return true;
}

/*
 * To write data to a slave device
 */
bool resource_write_data(uint8_t *data, uint32_t length)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	// write length byte data to UART
	ret = peripheral_uart_write(g_uart_h, data, length);
	if (ret != PERIPHERAL_ERROR_NONE) {
		LOGE("UART write failed, ret [%d]", ret);
		return false;
	}
	return true;
}

/*
 * To read data from a slave device
 */
bool resource_read_data(uint8_t *data, uint32_t length, bool blocking_mode)
{
	int try_again = 0;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	if (g_uart_h == NULL)
		return false;

	while (1) {
		// read length byte from UART
		ret = peripheral_uart_read(g_uart_h, data, length);
		if (ret == PERIPHERAL_ERROR_NONE)
			return true;

		// if data is not ready, try again
		if (ret == PERIPHERAL_ERROR_TRY_AGAIN) {
			// if blocking mode, read again
			if (blocking_mode == true) {
				usleep(100 * 1000);
				//LOGI(".");
				continue;
			} else {
				// if non-blocking mode, retry MAX_TRY_COUNT
				if (try_again >= MAX_TRY_COUNT) {
					LOGE("No data to receive");
					return false;
				} else {
					try_again++;
				}
			}
		} else {
			// if return value is not (PERIPHERAL_ERROR_NONE or PERIPHERAL_ERROR_TRY_AGAIN)
			// return with false
			LOGE("UART read failed, , ret [%d]", ret);
			return false;
		}
	}

	return true;
}

/*
 * close UART handle and clear UART resources
 */
void resource_serial_fini(void)
{
	LOGI("----- resource_serial_fini -----");
	if(initialized) {
		// Closes the UART slave device
		peripheral_uart_close(g_uart_h);
		initialized = false;
		g_uart_h = NULL;
	}
}

void uart_vr_test_main(void)
{
	int loop=0;
	bool ret = true;

	ret = resource_serial_init();
	if (ret == false) {
		LOGE("Failed to resource_serial_init");
		return;
	}

	/* setup Elechouse VR3 */
	resource_VR_setup();

	LOGI("Start Voice Recognition Test : speak...");
	for (loop=0; loop<50;loop++) {
		handle_VR_loop_check();
	}

	resource_serial_fini();
	LOGI("Voice Recognition Test Finished...");
}
