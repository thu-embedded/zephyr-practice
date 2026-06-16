#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/* ================================================================
 * Code có bug tiềm ẩn — dùng để luyện debug với GDB/SystemView
 * ================================================================ */

static uint32_t g_counter;
static uint8_t  g_buffer[16];

/* Bug 1: off-by-one → stack/heap corruption */
static void fill_buffer(uint8_t val, size_t count)
{
	/* BUG: count có thể = 17 → buffer overflow */
	for (size_t i = 0; i < count && i < sizeof(g_buffer); i++) {
		g_buffer[i] = val + i;
	}
}

/* Bug 2: integer overflow */
static uint16_t compute(uint8_t a, uint8_t b)
{
	/* BUG: nếu a=200, b=100 → overflow trước khi assign */
	uint8_t tmp = a + b;
	return tmp;
}

/* Bug 3: null pointer — gây HardFault */
static void maybe_null_deref(bool trigger)
{
	uint32_t *ptr = NULL;

	if (trigger) {
		*ptr = 0xDEAD; /* HardFault! */
	}
}

static struct k_thread monitor_thread;
K_THREAD_STACK_DEFINE(monitor_stack, 512);

static void monitor_fn(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);

	while (1) {
		g_counter++;
		LOG_DBG("counter=%u, buf[0]=0x%02X", g_counter, g_buffer[0]);
		k_sleep(K_MSEC(500));
	}
}

int main(void)
{
	LOG_INF("Debug demo — attach GDB or SystemView");

	fill_buffer(0xA0, 8);
	LOG_INF("compute(100,50)=%u (expected 150)", compute(100, 50));
	LOG_INF("compute(200,100)=%u (expected 300, BUG!)", compute(200, 100));

	k_thread_create(&monitor_thread, monitor_stack,
			K_THREAD_STACK_SIZEOF(monitor_stack),
			monitor_fn, NULL, NULL, NULL,
			5, 0, K_NO_WAIT);
	k_thread_name_set(&monitor_thread, "monitor");

	/* Uncomment để trigger HardFault và luyện fault analysis: */
	/* maybe_null_deref(true); */

	return 0;
}
