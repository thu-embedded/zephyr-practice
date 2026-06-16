# ex01 - Blinky (GPIO Output)

Nhấp nháy LED trên PB2 mỗi 500ms dùng Device Tree alias và GPIO driver.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Device Tree & Driver

**Q1.** `DT_ALIAS(led0)` trả về gì? Nó lấy thông tin từ đâu trong quá trình build?

**Q2.** `GPIO_DT_SPEC_GET(LED_NODE, gpios)` sinh ra struct `gpio_dt_spec` với những trường nào?
Tại sao cần cả `port` lẫn `pin` trong struct đó?

**Q3.** Trong file `.dts` của board, LED được khai báo:
```
gpios = <&gpiob 2 GPIO_ACTIVE_HIGH>;
```
`GPIO_ACTIVE_HIGH` ảnh hưởng thế nào đến hành vi của `gpio_pin_toggle_dt()` so với
`gpio_pin_toggle()` thông thường?

**Q4.** Nếu bạn gọi `gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE)` thì pin được set
mức nào lúc khởi động? Nếu đổi thành `GPIO_OUTPUT_ACTIVE` thì sao?

### Kernel & Scheduling

**Q5.** `k_sleep(K_MSEC(500))` làm gì với thread hiện tại? Thread đó ở trạng thái nào
trong thời gian chờ? Scheduler có thể chạy thread khác không?

**Q6.** Zephyr có một thread đặc biệt tên là `idle thread`. Nó làm gì khi không có thread
nào ready? Với STM32, nó có tắt clock để tiết kiệm điện không?

**Q7.** `main()` trong Zephyr chạy trong context nào? Stack size của nó được config bằng
Kconfig nào? Giá trị mặc định là bao nhiêu byte?

**Q8.** Nếu bỏ `while(1)` và để `main()` return, điều gì xảy ra với hệ thống?

### Thực tế

**Q9.** LED nhấp nháy ở tần số thực tế có đúng 1Hz (500ms on + 500ms off) không?
Điều gì có thể làm lệch timing? Nếu cần timing chính xác hơn thì dùng gì?

**Q10.** Nếu muốn nhấp nháy LED mà **không block** main thread (để main làm việc khác
đồng thời), bạn sẽ dùng cơ chế nào của Zephyr?
