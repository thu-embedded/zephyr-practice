#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static void task_sensor(void)
{
	static int value = 0;

	value++;
	LOG_DBG("Raw sensor value: %d", value);

	if (value > 90) {
		LOG_ERR("Sensor value out of range: %d", value);
	} else if (value > 70) {
		LOG_WRN("Sensor value high: %d", value);
	} else {
		LOG_INF("Sensor OK: %d", value);
	}
}

int main(void)
{
	LOG_INF("System started");

	for (int i = 0; i < 100; i++) {
		task_sensor();
		k_sleep(K_MSEC(100));
	}

	LOG_INF("Done");
	return 0;
}
