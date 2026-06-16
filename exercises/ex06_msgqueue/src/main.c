#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

struct sensor_data {
	uint32_t timestamp_ms;
	int16_t  temperature;
	uint8_t  sensor_id;
};

#define QUEUE_LEN 8
K_MSGQ_DEFINE(sensor_queue, sizeof(struct sensor_data), QUEUE_LEN, 4);

static void producer_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	uint8_t id = 0;

	while (1) {
		struct sensor_data msg = {
			.timestamp_ms = k_uptime_get_32(),
			.temperature  = 200 + (id % 50),
			.sensor_id    = id++,
		};

		int ret = k_msgq_put(&sensor_queue, &msg, K_NO_WAIT);
		if (ret != 0) {
			LOG_WRN("Queue full! dropped sample %d", msg.sensor_id);
		} else {
			LOG_DBG("Produced: id=%d temp=%d.%d",
				msg.sensor_id,
				msg.temperature / 10,
				msg.temperature % 10);
		}

		k_sleep(K_MSEC(200));
	}
}

static void consumer_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	struct sensor_data msg;

	while (1) {
		k_msgq_get(&sensor_queue, &msg, K_FOREVER);
		LOG_INF("Consumed: id=%d temp=%d.%d°C at %ums",
			msg.sensor_id,
			msg.temperature / 10,
			msg.temperature % 10,
			msg.timestamp_ms);

		k_sleep(K_MSEC(500));
	}
}

K_THREAD_DEFINE(producer, 512, producer_fn, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(consumer, 512, consumer_fn, NULL, NULL, NULL, 6, 0, 0);

int main(void)
{
	LOG_INF("Producer/Consumer demo started");
	return 0;
}
