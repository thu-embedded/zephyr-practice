# ex10 - I2C Sensor (MPU6050)

Đọc gia tốc, con quay hồi chuyển và nhiệt độ từ MPU6050 qua I2C1.
Dùng **Sensor API** của Zephyr — abstraction layer trên I2C thô.

## Kết nối

```
MPU6050   →   STM32F405
VCC       →   3.3V
GND       →   GND
SCL       →   PB6 (I2C1_SCL)
SDA       →   PB7 (I2C1_SDA)
AD0       →   GND  (I2C addr = 0x68)
```

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Device Tree Overlay

**Q1.** File `boards/weact_stm32f405_core.overlay` làm gì? Tại sao không sửa thẳng
vào file `.dts` của board? Khi nào nên dùng overlay vs sửa DTS gốc?

**Q2.** `mpu6050@68` — số `68` là gì? Nếu AD0 pin của MPU6050 được kéo lên VCC,
địa chỉ thay đổi thế nào và bạn cần sửa gì trong overlay?

**Q3.** `compatible = "invensense,mpu6050"` — chuỗi này dùng để làm gì trong
quá trình build? Zephyr match driver nào với node này?

### Sensor API vs I2C raw

**Q4.** `sensor_sample_fetch()` rồi `sensor_channel_get()` — tại sao cần 2 bước?
`sensor_sample_fetch()` làm gì bên dưới (hint: I2C transaction)?

**Q5.** Nếu gọi `sensor_channel_get()` mà không gọi `sensor_sample_fetch()` trước,
data trả về là gì?

**Q6.** `struct sensor_value` có 2 trường: `val1` (integer) và `val2` (micro-fraction).
`sensor_value_to_double()` tính thế nào? Tại sao Zephyr không dùng `float` trực tiếp?

**Q7.** Tại sao `sensor_sample_fetch()` không nhận channel cụ thể (fetch tất cả cùng lúc)?
Ưu/nhược điểm so với fetch từng channel riêng?

### I2C internals

**Q8.** Một lần `sensor_sample_fetch()` với MPU6050 tạo ra bao nhiêu I2C transaction?
(Gợi ý: cần đọc accel XYZ, gyro XYZ, temp — mỗi thứ bao nhiêu byte?)

**Q9.** `clock-frequency = <I2C_BITRATE_FAST>` trong DTS là bao nhiêu Hz?
Thời gian tối thiểu để transfer 14 bytes accel+gyro+temp là bao lâu?

### Trigger (nâng cao)

**Q10.** MPU6050 có chân INT. Sensor API hỗ trợ **trigger mode** — thay vì polling
mỗi 500ms, sensor tự báo khi có data mới qua interrupt. Cần thêm gì trong overlay
và code để dùng trigger? Ưu điểm về power consumption là gì?
