# ex03 - Logging Subsystem

Dùng Zephyr Logging để in log có phân cấp mức độ (ERR/WRN/INF/DBG).

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Log Levels

**Q1.** Zephyr có 4 log level: ERR=1, WRN=2, INF=3, DBG=4.
Nếu set `CONFIG_LOG_DEFAULT_LEVEL=2` trong `prj.conf`, những macro nào sẽ **không** in ra?
`LOG_DBG` bị loại bỏ lúc nào — compile time hay runtime?

**Q2.** `LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG)` — tham số thứ 2 là gì?
Nó có thể override `CONFIG_LOG_DEFAULT_LEVEL` không? Hướng nào (cao hơn hay thấp hơn)?

**Q3.** Sự khác nhau giữa `LOG_INF("hello")` và `printk("hello\n")` về:
- Overhead tại điểm gọi
- Thông tin kèm theo (timestamp, module name)
- Khả năng tắt/bật không cần sửa code

### Deferred vs Immediate

**Q4.** Zephyr Logging có hai chế độ: **deferred** (mặc định) và **immediate**.
Trong chế độ deferred, message được xử lý ở đâu và khi nào?
`CONFIG_LOG_MODE_IMMEDIATE=y` thay đổi điều gì? Khi nào cần dùng immediate?

**Q5.** Trong chế độ deferred, nếu log buffer đầy (quá nhiều message trong thời gian ngắn),
điều gì xảy ra? Zephyr có báo dropped messages không?

**Q6.** Tại sao **không nên** gọi `LOG_DBG()` từ bên trong ISR khi đang ở chế độ deferred?
Nếu cần log từ ISR, phải làm gì?

### Compile-time Optimization

**Q7.** Khi `LOG_LEVEL_DBG` bị disable tại compile time, macro `LOG_DBG(...)` expand thành gì?
Điều này có nghĩa là string format và arguments có còn tốn flash không?

**Q8.** `LOG_INF("Value: %d", value)` — trong chế độ deferred, `value` được copy vào
log buffer lúc gọi hay lúc print? Tại sao điều này quan trọng với biến local?

### Thực tế

**Q9.** Bạn có 2 file: `sensor.c` và `main.c`, mỗi file muốn dùng logging với tên module
riêng. Cần thêm gì vào mỗi file?

**Q10.** Nếu muốn xem log qua **RTT** (SEGGER J-Link) thay vì UART, cần thêm những
config nào? Ưu điểm của RTT so với UART trong môi trường debug là gì?
