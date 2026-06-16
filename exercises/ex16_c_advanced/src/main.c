#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdint.h>
#include <string.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* ================================================================
 * 1. volatile: memory-mapped IO register
 * ================================================================ */

/* Giả lập UART status register tại địa chỉ cố định */
#define UART_SR_ADDR  0x40011000UL
#define UART_DR_ADDR  0x40011004UL
#define UART_SR_TXE   BIT(7)

static inline void uart_send_byte_raw(uint8_t byte)
{
	volatile uint32_t *sr = (volatile uint32_t *)UART_SR_ADDR;
	volatile uint32_t *dr = (volatile uint32_t *)UART_DR_ADDR;

	while (!(*sr & UART_SR_TXE)) {
		/* chờ TX empty — compiler KHÔNG được optimize loop này */
	}
	*dr = byte;
}

/* ================================================================
 * 2. restrict: tối ưu memcpy
 * ================================================================ */

/* Không có restrict — compiler phải assume src/dst có thể overlap */
void my_memcpy(uint8_t *dst, const uint8_t *src, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		dst[i] = src[i];
	}
}

/* Có restrict — compiler biết chắc không overlap, optimize tốt hơn */
void my_memcpy_restrict(uint8_t * __restrict dst,
			const uint8_t * __restrict src, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		dst[i] = src[i];
	}
}

/* ================================================================
 * 3. struct alignment & padding
 * ================================================================ */

struct bad_layout {
	uint8_t  a;      /* 1 byte */
	/* 3 bytes padding */
	uint32_t b;      /* 4 bytes */
	uint8_t  c;      /* 1 byte */
	/* 3 bytes padding */
};                   /* total: 12 bytes */

struct good_layout {
	uint32_t b;      /* 4 bytes */
	uint8_t  a;      /* 1 byte */
	uint8_t  c;      /* 1 byte */
	/* 2 bytes padding */
};                   /* total: 8 bytes */

struct packed_layout {
	uint8_t  a;
	uint32_t b;
	uint8_t  c;
} __attribute__((packed)); /* total: 6 bytes — nhưng b có thể unaligned! */

/* ================================================================
 * 4. Function pointers — callback pattern giống Zephyr driver API
 * ================================================================ */

typedef void (*sensor_ready_cb_t)(int16_t value, void *user_data);

struct sensor_driver {
	const char *name;
	int  (*init)(void);
	int  (*read)(int16_t *out);
	void (*set_callback)(sensor_ready_cb_t cb, void *user_data);
};

static sensor_ready_cb_t g_cb;
static void *g_user_data;

static int fake_sensor_init(void) { return 0; }
static int fake_sensor_read(int16_t *out) { *out = 2500; return 0; }
static void fake_sensor_set_cb(sensor_ready_cb_t cb, void *user_data)
{
	g_cb = cb;
	g_user_data = user_data;
}

static const struct sensor_driver fake_sensor = {
	.name         = "fake_temp",
	.init         = fake_sensor_init,
	.read         = fake_sensor_read,
	.set_callback = fake_sensor_set_cb,
};

static void on_sensor_ready(int16_t value, void *user_data)
{
	const char *label = (const char *)user_data;
	LOG_INF("[%s] sensor value: %d.%02d", label, value / 100, value % 100);
}

/* ================================================================
 * 5. Bit manipulation
 * ================================================================ */

/* Bitmask operations thường gặp trong register manipulation */
#define REG_CTRL  0x00
#define REG_EN    BIT(0)
#define REG_MODE  GENMASK(2, 1)   /* bits [2:1] */
#define REG_SPEED GENMASK(5, 3)   /* bits [5:3] */

static uint8_t reg_set_field(uint8_t reg, uint8_t mask, uint8_t val)
{
	return (reg & ~mask) | ((val << __builtin_ctz(mask)) & mask);
}

int main(void)
{
	/* struct sizes */
	LOG_INF("bad_layout  size: %zu", sizeof(struct bad_layout));
	LOG_INF("good_layout size: %zu", sizeof(struct good_layout));
	LOG_INF("packed      size: %zu", sizeof(struct packed_layout));

	/* function pointer callback */
	fake_sensor.init();
	fake_sensor.set_callback(on_sensor_ready, (void *)"ch0");

	int16_t val;
	fake_sensor.read(&val);
	if (g_cb) {
		g_cb(val, g_user_data);
	}

	/* bit manipulation */
	uint8_t ctrl = 0;
	ctrl = reg_set_field(ctrl, REG_MODE,  2);  /* set mode = 2 */
	ctrl = reg_set_field(ctrl, REG_SPEED, 5);  /* set speed = 5 */
	ctrl |= REG_EN;
	LOG_INF("ctrl reg: 0x%02X", ctrl);

	return 0;
}
