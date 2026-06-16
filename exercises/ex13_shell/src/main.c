#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

/* --- Shell commands --- */

static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
{
	gpio_pin_set_dt(&led, 1);
	shell_print(sh, "LED ON");
	return 0;
}

static int cmd_led_off(const struct shell *sh, size_t argc, char **argv)
{
	gpio_pin_set_dt(&led, 0);
	shell_print(sh, "LED OFF");
	return 0;
}

static int cmd_led_blink(const struct shell *sh, size_t argc, char **argv)
{
	if (argc < 2) {
		shell_error(sh, "Usage: led blink <count>");
		return -EINVAL;
	}

	int count = atoi(argv[1]);
	for (int i = 0; i < count; i++) {
		gpio_pin_set_dt(&led, 1);
		k_sleep(K_MSEC(200));
		gpio_pin_set_dt(&led, 0);
		k_sleep(K_MSEC(200));
	}
	shell_print(sh, "Blinked %d times", count);
	return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
	shell_print(sh, "Zephyr version: %s", KERNEL_VERSION_STRING);
	shell_print(sh, "Board: %s", CONFIG_BOARD);
	shell_print(sh, "Uptime: %lld ms", k_uptime_get());
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(led_cmds,
	SHELL_CMD(on,    NULL, "Turn LED on",          cmd_led_on),
	SHELL_CMD(off,   NULL, "Turn LED off",         cmd_led_off),
	SHELL_CMD(blink, NULL, "Blink LED N times",    cmd_led_blink),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(led,  &led_cmds, "LED control", NULL);
SHELL_CMD_REGISTER(info, NULL,      "System info",  cmd_info);

int main(void)
{
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	LOG_INF("Shell ready. Type 'help' in terminal.");
	return 0;
}
