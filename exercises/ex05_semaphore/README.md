# ex05 - Semaphore (ISR → Thread signaling)

Button ISR dùng semaphore để báo hiệu cho thread xử lý LED.
Đây là pattern cơ bản nhất để đưa công việc từ ISR ra thread.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Semaphore cơ bản

**Q1.** `K_SEM_DEFINE(btn_sem, 0, 1)` — tham số thứ 2 (initial count = 0) và
thứ 3 (limit = 1) có nghĩa là gì? Nếu limit = 10 thì khác gì?

**Q2.** `k_sem_give()` trong ISR và `k_sem_take()` trong thread — phân tích
luồng thực thi: chuyện gì xảy ra từng bước khi button được nhấn?

**Q3.** `k_sem_take(&btn_sem, K_FOREVER)` — nếu đổi timeout thành `K_MSEC(100)`,
thread hoạt động thế nào khi không có button press? Giá trị trả về của
`k_sem_take()` có ý nghĩa gì?

### ISR vs Thread

**Q4.** Tại sao không xử lý LED trực tiếp trong `button_isr()` mà phải dùng
semaphore để chuyển sang thread? Liệt kê những gì **không được làm** trong ISR.

**Q5.** `k_sem_give()` có thể gọi từ ISR không? Zephyr xử lý race condition
trong hàm này thế nào (hint: interrupt lock)?

**Q6.** Nếu button được nhấn **10 lần liên tiếp** trước khi `led_task` kịp xử lý,
với limit=1 thì bao nhiêu lần LED toggle? Với limit=10 thì bao nhiêu?

### K_THREAD_DEFINE vs k_thread_create

**Q7.** `K_THREAD_DEFINE` ở file scope khác gì với `k_thread_create()` trong `main()`?
Thread được tạo lúc nào — compile time hay runtime? Nó start khi nào?

**Q8.** `K_THREAD_DEFINE(led_thread, 512, led_task, NULL, NULL, NULL, 5, 0, 0)` —
tham số cuối cùng `0` là delay (ms). Nếu đổi thành `1000`, thread bắt đầu chạy
sau bao lâu? Trong thời gian đó nếu button được nhấn thì sao?

### Mutex vs Semaphore

**Q9.** Phân biệt semaphore và mutex trong Zephyr:
- Dùng semaphore khi nào?
- Dùng mutex khi nào?
- Mutex có tính năng gì mà semaphore không có (hint: priority inheritance)?

### Thực tế

**Q10.** Pattern ISR → semaphore → thread rất phổ biến. Nhưng nếu cần truyền
thêm **dữ liệu** từ ISR sang thread (không chỉ báo hiệu), semaphore không đủ.
Bạn sẽ dùng cơ chế nào? (Xem trước ex06)
