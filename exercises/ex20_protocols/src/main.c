#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* ================================================================
 * UART: tính baud rate và framing
 * ================================================================
 *
 * Frame: [START(1)] [DATA(8)] [PARITY(0/1)] [STOP(1/2)]
 * Baud rate = 115200 → 1 bit = 8.68 µs
 * 1 byte (8N1) = 10 bits = 86.8 µs
 *
 * STM32 USART BRR register:
 *   BRR = fCK / baud = 84MHz / 115200 ≈ 729.16
 *   Mantissa = 729, Fraction = 0.16 * 16 ≈ 3
 *   BRR = (729 << 4) | 3 = 0x2D93
 */

/* ================================================================
 * I2C: raw transaction thay vì sensor API
 * ================================================================ */

static const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c1));

static int i2c_read_register(uint8_t dev_addr, uint8_t reg, uint8_t *val)
{
	/*
	 * I2C transaction để đọc 1 register:
	 * 1. START
	 * 2. Address + W (write)
	 * 3. Register address byte
	 * 4. REPEATED START
	 * 5. Address + R (read)
	 * 6. Data byte
	 * 7. NACK + STOP
	 */
	struct i2c_msg msgs[] = {
		{
			.buf   = &reg,
			.len   = 1,
			.flags = I2C_MSG_WRITE,       /* write register address */
		},
		{
			.buf   = val,
			.len   = 1,
			.flags = I2C_MSG_READ | I2C_MSG_STOP,  /* read + stop */
		},
	};
	return i2c_transfer(i2c, msgs, ARRAY_SIZE(msgs), dev_addr);
}

/* ================================================================
 * SPI: raw transaction
 * ================================================================ */

static const struct device *spi = DEVICE_DT_GET(DT_NODELABEL(spi1));

static const struct spi_config spi_cfg = {
	.frequency = 1000000,   /* 1 MHz */
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER,
	.slave     = 0,
	.cs        = NULL,      /* CS manual via GPIO */
};

static int spi_read_write(uint8_t *tx, uint8_t *rx, size_t len)
{
	struct spi_buf tx_buf = { .buf = tx, .len = len };
	struct spi_buf rx_buf = { .buf = rx, .len = len };
	struct spi_buf_set tx_set = { .buffers = &tx_buf, .count = 1 };
	struct spi_buf_set rx_set = { .buffers = &rx_buf, .count = 1 };

	return spi_transceive(spi, &spi_cfg, &tx_set, &rx_set);
}

/* ================================================================
 * CAN: gửi và nhận frame
 * ================================================================ */

static const struct device *can = DEVICE_DT_GET(DT_NODELABEL(can1));

static void can_rx_cb(const struct device *dev, struct can_frame *frame,
		      void *user_data)
{
	LOG_INF("CAN RX: ID=0x%03X len=%d data[0]=0x%02X",
		frame->id, frame->dlc, frame->data[0]);
}

static int demo_can(void)
{
	/* Filter: chỉ nhận frame ID 0x123 */
	const struct can_filter filter = {
		.id      = 0x123,
		.mask    = CAN_STD_ID_MASK,
		.flags   = 0,
	};
	can_add_rx_filter(can, can_rx_cb, NULL, &filter);

	/* Gửi frame */
	struct can_frame tx = {
		.id  = 0x456,
		.dlc = 4,
		.data = {0x01, 0x02, 0x03, 0x04},
	};
	return can_send(can, &tx, K_MSEC(100), NULL, NULL);
}

int main(void)
{
	LOG_INF("Protocol deep dive demo");

	/* I2C: đọc WHO_AM_I của MPU6050 (register 0x75, expected 0x68) */
	if (device_is_ready(i2c)) {
		uint8_t who_am_i;
		int rc = i2c_read_register(0x68, 0x75, &who_am_i);
		if (rc == 0) {
			LOG_INF("MPU6050 WHO_AM_I: 0x%02X (expect 0x68)", who_am_i);
		} else {
			LOG_WRN("I2C read failed: %d (sensor connected?)", rc);
		}
	}

	/* CAN */
	if (device_is_ready(can)) {
		can_start(can);
		int rc = demo_can();
		LOG_INF("CAN send: %s", rc == 0 ? "OK" : "FAIL (no ACK?)");
	}

	return 0;
}
