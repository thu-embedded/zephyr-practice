#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* --- Periodic timer: toggle LED mỗi 500ms --- */
static struct k_timer blink_timer;

static void blink_timer_fn(struct k_timer *timer)
{
	gpio_pin_toggle_dt(&led);
}

/* --- One-shot timer + work: in log sau 3s --- */
static struct k_timer oneshot_timer;
static struct k_work  log_work;

static void log_work_fn(struct k_work *work)
{
	LOG_INF("3 seconds elapsed (from work queue, not ISR)");
}

static void oneshot_timer_fn(struct k_timer *timer)
{
	k_work_submit(&log_work);
}

/* --- Delayable work: debounce button --- */
static struct k_work_delayable debounce_work;

static void debounce_fn(struct k_work *work)
{
	LOG_INF("Button stable (debounced)");
}

static struct gpio_callback btn_cb;
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

static void button_isr(const struct device *dev, struct gpio_callback *cb,
		       uint32_t pins)
{
	k_work_reschedule(&debounce_work, K_MSEC(50));
}

int main(void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&btn, GPIO_INPUT);
	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&btn_cb, button_isr, BIT(btn.pin));
	gpio_add_callback(btn.port, &btn_cb);

	k_work_init(&log_work, log_work_fn);
	k_work_init_delayable(&debounce_work, debounce_fn);

	k_timer_init(&blink_timer, blink_timer_fn, NULL);
	k_timer_start(&blink_timer, K_MSEC(500), K_MSEC(500));

	k_timer_init(&oneshot_timer, oneshot_timer_fn, NULL);
	k_timer_start(&oneshot_timer, K_SECONDS(3), K_NO_WAIT);

	LOG_INF("Timers started");
	return 0;
}
