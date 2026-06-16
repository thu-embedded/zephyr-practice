#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/*
 * Dùng sensor driver API (generic) thay vì gọi I2C trực tiếp.
 * Gắn cảm biến MPU6050 (gyro/accel) vào I2C1: SDA=PB7, SCL=PB6.
 *
 * Cần thêm overlay file: boards/weact_stm32f405_core.overlay
 */
#define SENSOR_NODE DT_NODELABEL(mpu6050)

static const struct device *sensor = DEVICE_DT_GET(SENSOR_NODE);

static void print_sensor_data(void)
{
	struct sensor_value accel[3];
	struct sensor_value gyro[3];
	struct sensor_value temp;

	sensor_sample_fetch(sensor);

	sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);
	sensor_channel_get(sensor, SENSOR_CHAN_GYRO_XYZ,  gyro);
	sensor_channel_get(sensor, SENSOR_CHAN_DIE_TEMP,  &temp);

	LOG_INF("Accel: X=%.2f Y=%.2f Z=%.2f m/s²",
		sensor_value_to_double(&accel[0]),
		sensor_value_to_double(&accel[1]),
		sensor_value_to_double(&accel[2]));

	LOG_INF("Gyro:  X=%.2f Y=%.2f Z=%.2f rad/s",
		sensor_value_to_double(&gyro[0]),
		sensor_value_to_double(&gyro[1]),
		sensor_value_to_double(&gyro[2]));

	LOG_INF("Temp:  %.1f °C", sensor_value_to_double(&temp));
}

int main(void)
{
	if (!device_is_ready(sensor)) {
		LOG_ERR("MPU6050 not ready. Check wiring and overlay.");
		return -1;
	}

	LOG_INF("MPU6050 ready");

	while (1) {
		print_sensor_data();
		k_sleep(K_MSEC(500));
	}

	return 0;
}
