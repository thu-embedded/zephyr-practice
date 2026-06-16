#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define NVS_PARTITION        storage_partition
#define NVS_PARTITION_DEVICE FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define ID_BOOT_COUNT  1
#define ID_DEVICE_NAME 2
#define ID_THRESHOLD   3

static struct nvs_fs nvs;

static uint32_t load_boot_count(void)
{
	uint32_t count = 0;
	ssize_t rc = nvs_read(&nvs, ID_BOOT_COUNT, &count, sizeof(count));

	if (rc < 0) {
		LOG_INF("boot_count not found, starting at 0");
		count = 0;
	}
	return count;
}

int main(void)
{
	struct flash_pages_info info;
	const struct device *flash_dev = NVS_PARTITION_DEVICE;

	nvs.flash_device = flash_dev;
	nvs.offset       = NVS_PARTITION_OFFSET;
	flash_get_page_info_by_offs(flash_dev, nvs.offset, &info);
	nvs.sector_size  = info.size;
	nvs.sector_count = 3;

	int rc = nvs_mount(&nvs);
	if (rc) {
		LOG_ERR("NVS mount failed: %d", rc);
		return rc;
	}

	/* Boot counter */
	uint32_t boot_count = load_boot_count() + 1;
	nvs_write(&nvs, ID_BOOT_COUNT, &boot_count, sizeof(boot_count));
	LOG_INF("Boot count: %u", boot_count);

	/* Device name */
	char name[32] = "MyDevice";
	rc = nvs_read(&nvs, ID_DEVICE_NAME, name, sizeof(name));
	if (rc < 0) {
		nvs_write(&nvs, ID_DEVICE_NAME, name, strlen(name) + 1);
		LOG_INF("Device name initialized: %s", name);
	} else {
		LOG_INF("Device name loaded: %s", name);
	}

	/* Float threshold stored as int (x100) */
	int32_t threshold = 2550;
	rc = nvs_read(&nvs, ID_THRESHOLD, &threshold, sizeof(threshold));
	if (rc < 0) {
		nvs_write(&nvs, ID_THRESHOLD, &threshold, sizeof(threshold));
	}
	LOG_INF("Threshold: %d.%02d", threshold / 100, threshold % 100);

	return 0;
}
