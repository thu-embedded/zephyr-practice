#include <zephyr/kernel.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)
#define BTN_NODE DT_ALIAS(sw0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN_NODE, gpios);

static struct gpio_callback btn_cb;
static K_SEM_DEFINE(wakeup_sem, 0, 1);

static void btn_isr(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	k_sem_give(&wakeup_sem);
}

/*
 * pm_policy_state_lock_get/put: yêu cầu kernel không vào sleep state
 * khi đang xử lý tác vụ quan trọng.
 */
static void do_active_work(void)
{
	LOG_INF("Active: doing work...");
	gpio_pin_set_dt(&led, 1);
	k_sleep(K_MSEC(500));
	gpio_pin_set_dt(&led, 0);
	LOG_INF("Active: work done, going idle");
}

int main(void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&btn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&btn_cb, btn_isr, BIT(btn.pin));
	gpio_add_callback(btn.port, &btn_cb);

	LOG_INF("Power management demo");
	LOG_INF("Press button to wake from idle");

	while (1) {
		/*
		 * k_sleep cho phép kernel vào low-power state (sleep/stop)
		 * nếu PM được bật. Zephyr tự chọn state phù hợp.
		 */
		int ret = k_sem_take(&wakeup_sem, K_SECONDS(10));

		if (ret == 0) {
			LOG_INF("Woken by button");
		} else {
			LOG_INF("Woken by timeout (10s)");
		}

		do_active_work();
	}

	return 0;
}
