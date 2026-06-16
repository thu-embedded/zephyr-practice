#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* CDC-ACM UART device được tạo tự động khi bật USB CDC */
#define CDC_ACM_NODE DT_NODELABEL(cdc_acm_uart0)

static const struct device *cdc_dev = DEVICE_DT_GET(CDC_ACM_NODE);

static void uart_irq_cb(const struct device *dev, void *user_data)
{
	uint8_t buf[64];
	int len;

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (!uart_irq_rx_ready(dev)) {
			continue;
		}

		len = uart_fifo_read(dev, buf, sizeof(buf));
		if (len > 0) {
			LOG_INF("USB CDC RX [%d]: %.*s", len, len, buf);
			/* Echo back */
			uart_fifo_fill(dev, buf, len);
		}
	}
}

int main(void)
{
	if (!device_is_ready(cdc_dev)) {
		LOG_ERR("CDC ACM device not ready");
		return -1;
	}

	uart_irq_callback_set(cdc_dev, uart_irq_cb);
	uart_irq_rx_enable(cdc_dev);

	LOG_INF("USB CDC-ACM ready. Connect USB cable to PA11/PA12.");
	LOG_INF("Open /dev/ttyACM0 on host.");

	while (1) {
		k_sleep(K_SECONDS(5));
		const char *msg = "Zephyr USB CDC heartbeat\r\n";
		uart_fifo_fill(cdc_dev, (const uint8_t *)msg, strlen(msg));
	}

	return 0;
}
