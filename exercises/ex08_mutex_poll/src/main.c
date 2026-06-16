#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* --- Shared resource protected by mutex --- */
static K_MUTEX_DEFINE(data_mutex);

static struct {
	int32_t value;
	uint32_t updated_at_ms;
} shared_data;

static void writer_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	int i = 0;
	while (1) {
		k_mutex_lock(&data_mutex, K_FOREVER);
		shared_data.value      = i++;
		shared_data.updated_at_ms = k_uptime_get_32();
		k_mutex_unlock(&data_mutex);

		LOG_DBG("Writer: wrote %d", i - 1);
		k_sleep(K_MSEC(300));
	}
}

static void reader_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		k_mutex_lock(&data_mutex, K_FOREVER);
		int32_t  val = shared_data.value;
		uint32_t ts  = shared_data.updated_at_ms;
		k_mutex_unlock(&data_mutex);

		LOG_INF("Reader: value=%d (written at %ums)", val, ts);
		k_sleep(K_MSEC(700));
	}
}

/* --- k_poll: chờ semaphore HOẶC msgq cùng lúc --- */
static K_SEM_DEFINE(event_a, 0, 1);
static K_SEM_DEFINE(event_b, 0, 1);

static void poll_thread_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	struct k_poll_event events[] = {
		K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_SEM_AVAILABLE,
						K_POLL_MODE_NOTIFY_ONLY,
						&event_a, 0),
		K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_SEM_AVAILABLE,
						K_POLL_MODE_NOTIFY_ONLY,
						&event_b, 0),
	};

	while (1) {
		k_poll(events, ARRAY_SIZE(events), K_FOREVER);

		if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
			k_sem_take(&event_a, K_NO_WAIT);
			LOG_INF("k_poll: event_a fired");
			events[0].state = K_POLL_STATE_NOT_READY;
		}
		if (events[1].state == K_POLL_STATE_SEM_AVAILABLE) {
			k_sem_take(&event_b, K_NO_WAIT);
			LOG_INF("k_poll: event_b fired");
			events[1].state = K_POLL_STATE_NOT_READY;
		}
	}
}

static void trigger_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		k_sleep(K_MSEC(1000));
		k_sem_give(&event_a);
		k_sleep(K_MSEC(600));
		k_sem_give(&event_b);
	}
}

K_THREAD_DEFINE(writer,      512, writer_fn,      NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(reader,      512, reader_fn,      NULL, NULL, NULL, 6, 0, 0);
K_THREAD_DEFINE(poll_thread, 768, poll_thread_fn, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(trigger,     512, trigger_fn,     NULL, NULL, NULL, 7, 0, 0);

int main(void)
{
	LOG_INF("Mutex + k_poll demo started");
	return 0;
}
