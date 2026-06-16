#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* ================================================================
 * Các loại biến và section tương ứng
 * ================================================================ */

/* .rodata — Flash, read-only */
static const uint8_t lookup_table[8] = {0, 1, 4, 9, 16, 25, 36, 49};
static const char    device_name[]   = "WeAct-STM32F405";

/* .data — khởi tạo trong Flash, copy sang RAM lúc boot */
static uint32_t boot_count = 0xDEAD;
static int32_t  gain       = 100;

/* .bss — không khởi tạo (zero-filled bởi startup code) */
static uint8_t  rx_buffer[256];
static uint32_t error_count;

/* .noinit — không bị xóa khi reset (dùng để detect reset reason) */
static uint32_t reset_magic __attribute__((section(".noinit")));
#define RESET_MAGIC 0xCAFEBABEU

/* Đặt vào CCM (Core Coupled Memory) — zero wait state trên STM32F4 */
static uint32_t fast_buffer[32] __attribute__((section(".dtcm")));

/* ================================================================
 * Đọc thông tin memory layout từ linker symbols
 * ================================================================ */

/* Linker tự động tạo các symbol này */
extern uint32_t _flash_used;
extern uint32_t _image_rom_start;
extern uint32_t _image_rom_end;

static void print_memory_layout(void)
{
	/* Địa chỉ của các section — đọc từ linker symbols */
	extern uint32_t __text_region_start, __text_region_end;
	extern uint32_t __rodata_region_start, __rodata_region_end;
	extern uint32_t _data_start, _data_end;
	extern uint32_t __bss_start, __bss_end;

	LOG_INF("=== Memory Layout ===");
	LOG_INF(".text  : 0x%08X - 0x%08X (%u bytes)",
		(uint32_t)&__text_region_start,
		(uint32_t)&__text_region_end,
		(uint32_t)(&__text_region_end - &__text_region_start) * 4);
	LOG_INF(".rodata: 0x%08X - 0x%08X",
		(uint32_t)&__rodata_region_start,
		(uint32_t)&__rodata_region_end);
	LOG_INF(".data  : 0x%08X - 0x%08X (in RAM, init from Flash)",
		(uint32_t)&_data_start,
		(uint32_t)&_data_end);
	LOG_INF(".bss   : 0x%08X - 0x%08X",
		(uint32_t)&__bss_start,
		(uint32_t)&__bss_end);

	LOG_INF("=== Variables ===");
	LOG_INF("lookup_table @ 0x%08X (Flash/.rodata)", (uint32_t)lookup_table);
	LOG_INF("boot_count   @ 0x%08X (RAM/.data)",     (uint32_t)&boot_count);
	LOG_INF("rx_buffer    @ 0x%08X (RAM/.bss)",      (uint32_t)rx_buffer);
	LOG_INF("reset_magic  @ 0x%08X (RAM/.noinit)",   (uint32_t)&reset_magic);
	LOG_INF("fast_buffer  @ 0x%08X (CCM/.dtcm)",     (uint32_t)fast_buffer);
}

static void demo_reset_detection(void)
{
	if (reset_magic == RESET_MAGIC) {
		LOG_WRN("Warm reset detected! (not first boot)");
	} else {
		LOG_INF("Cold boot (first time)");
		reset_magic = RESET_MAGIC;
	}
}

int main(void)
{
	print_memory_layout();
	demo_reset_detection();

	LOG_INF("device_name: %s @ 0x%08X",
		device_name, (uint32_t)device_name);
	LOG_INF("boot_count: %u, gain: %d", boot_count, gain);
	LOG_INF("error_count: %u (should be 0, zeroed by startup)", error_count);

	return 0;
}
