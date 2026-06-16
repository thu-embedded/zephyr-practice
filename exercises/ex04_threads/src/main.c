#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define STACK_SIZE 512
#define PRIO_HIGH  3
#define PRIO_LOW   7

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

static struct k_thread thread_a;
static struct k_thread thread_b;

static void thread_a_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		LOG_INF("[A] running (prio %d)", PRIO_HIGH);
		k_sleep(K_MSEC(1000));
	}
}

static void thread_b_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		LOG_INF("[B] running (prio %d)", PRIO_LOW);
		k_sleep(K_MSEC(1500));
	}
}

int main(void)
{
	LOG_INF("Starting threads");

	k_thread_create(&thread_a, stack_a, STACK_SIZE,
			thread_a_fn, NULL, NULL, NULL,
			PRIO_HIGH, 0, K_NO_WAIT);
	k_thread_name_set(&thread_a, "thread_a");

	k_thread_create(&thread_b, stack_b, STACK_SIZE,
			thread_b_fn, NULL, NULL, NULL,
			PRIO_LOW, 0, K_NO_WAIT);
	k_thread_name_set(&thread_b, "thread_b");

	return 0;
}
