# ex02 - Button Interrupt (GPIO Input)

Nhấn nút PC13 → toggle LED PB2. Dùng GPIO interrupt thay vì polling.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### GPIO Interrupt

**Q1.** `gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE)` nghĩa là gì?
Với board này (button khai báo `GPIO_ACTIVE_HIGH`), ngắt kích hoạt ở cạnh lên hay cạnh xuống?
Nếu đổi thành `GPIO_INT_EDGE_BOTH` thì sao?

**Q2.** `gpio_init_callback(&btn_cb, button_pressed, BIT(btn.pin))` — tham số thứ 3
`BIT(btn.pin)` là gì? Tại sao cần mask thay vì chỉ số pin?

**Q3.** Hàm `button_pressed()` chạy trong context nào — ISR hay thread?
Điều đó có nghĩa là bạn **không được** làm gì bên trong callback đó?

**Q4.** `gpio_pin_toggle_dt()` có thể gọi từ ISR context không? Tại sao có hoặc không?
Kiểm tra điều này bằng cách nào?

### Debounce

**Q5.** Khi nhấn nút vật lý, tín hiệu thường dao động (bounce) trong vài ms.
Code hiện tại có xử lý debounce không? Điều gì có thể xảy ra nếu không có debounce?

**Q6.** Zephyr có `CONFIG_GPIO_HOGS` và thư viện `input` hỗ trợ debounce.
Nhưng nếu làm thủ công trong callback, tại sao không nên dùng `k_sleep()` ở đó?
Bạn sẽ dùng cơ chế nào thay thế?

### Polling vs Interrupt

**Q7.** So sánh hai cách đọc button:
- **Polling:** `while(1) { if (gpio_pin_get_dt(&btn)) { ... } k_sleep(K_MSEC(10)); }`
- **Interrupt:** như code hiện tại

Khi nào nên dùng polling, khi nào nên dùng interrupt?

### k_sleep(K_FOREVER)

**Q8.** Tại sao main thread dùng `k_sleep(K_FOREVER)` thay vì `while(1) {}`?
Hai cách này khác nhau thế nào về mặt CPU usage?

**Q9.** Nếu xóa hoàn toàn vòng lặp `while(1)` trong `main()` và để hàm return,
GPIO interrupt có còn hoạt động không? Tại sao?

### Thực tế

**Q10.** Button trên board khai báo `GPIO_PULL_DOWN`. Nếu board không có pull resistor
nội tại và bạn quên config pull, điều gì xảy ra với chân input? Đây là lỗi gì?
