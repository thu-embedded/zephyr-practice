#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define UART_NODE   DT_NODELABEL(usart2)
#define RX_BUF_SIZE 64
#define MSG_QUEUE_LEN 4

static const struct device *uart = DEVICE_DT_GET(UART_NODE);

static uint8_t rx_buf[RX_BUF_SIZE];
static uint8_t rx_buf2[RX_BUF_SIZE];

struct uart_msg {
	uint8_t data[RX_BUF_SIZE];
	size_t  len;
};

K_MSGQ_DEFINE(uart_msgq, sizeof(struct uart_msg), MSG_QUEUE_LEN, 4);

static void uart_cb(const struct device *dev, struct uart_event *evt,
		    void *user_data)
{
	struct uart_msg msg;

	switch (evt->type) {
	case UART_RX_RDY:
		if (evt->data.rx.len < RX_BUF_SIZE) {
			memcpy(msg.data,
			       evt->data.rx.buf + evt->data.rx.offset,
			       evt->data.rx.len);
			msg.len = evt->data.rx.len;
			k_msgq_put(&uart_msgq, &msg, K_NO_WAIT);
		}
		break;

	case UART_RX_BUF_REQUEST:
		uart_rx_buf_rsp(dev, rx_buf2, sizeof(rx_buf2));
		break;

	case UART_RX_BUF_RELEASED:
		break;

	case UART_RX_DISABLED:
		uart_rx_enable(dev, rx_buf, sizeof(rx_buf), 100 * USEC_PER_MSEC);
		break;

	default:
		break;
	}
}

int main(void)
{
	if (!device_is_ready(uart)) {
		LOG_ERR("UART not ready");
		return -1;
	}

	uart_callback_set(uart, uart_cb, NULL);
	uart_rx_enable(uart, rx_buf, sizeof(rx_buf), 100 * USEC_PER_MSEC);

	LOG_INF("UART ready on USART2 (PA2/PA3). Type something:");

	struct uart_msg msg;
	while (1) {
		if (k_msgq_get(&uart_msgq, &msg, K_FOREVER) == 0) {
			LOG_INF("RX [%d bytes]: %.*s", msg.len, msg.len, msg.data);

			/* Echo back */
			uart_tx(uart, msg.data, msg.len, SYS_FOREVER_US);
		}
	}

	return 0;
}
