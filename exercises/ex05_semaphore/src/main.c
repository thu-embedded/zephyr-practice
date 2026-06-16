#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define BTN_NODE DT_ALIAS(sw0)
#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN_NODE, gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static struct gpio_callback btn_cb;

K_SEM_DEFINE(btn_sem, 0, 1);

static void button_isr(const struct device *dev, struct gpio_callback *cb,
		       uint32_t pins)
{
	k_sem_give(&btn_sem);
}

static void led_task(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		k_sem_take(&btn_sem, K_FOREVER);
		gpio_pin_toggle_dt(&led);
		LOG_INF("LED toggled by button");
	}
}

K_THREAD_DEFINE(led_thread, 512, led_task, NULL, NULL, NULL, 5, 0, 0);

int main(void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&btn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&btn_cb, button_isr, BIT(btn.pin));
	gpio_add_callback(btn.port, &btn_cb);

	LOG_INF("Ready. Press button.");
	return 0;
}
