#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_NODE    DT_ALIAS(led0)
#define BTN_NODE    DT_ALIAS(sw0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(BTN_NODE, gpios);

static struct gpio_callback btn_cb;

static void button_pressed(const struct device *dev, struct gpio_callback *cb,
			   uint32_t pins)
{
	gpio_pin_toggle_dt(&led);
	printk("Button pressed!\n");
}

int main(void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&btn, GPIO_INPUT);

	gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&btn_cb, button_pressed, BIT(btn.pin));
	gpio_add_callback(btn.port, &btn_cb);

	printk("Press the button...\n");

	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
