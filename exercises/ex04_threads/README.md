# ex04 - Threads (Đa luồng)

Tạo 2 thread với priority khác nhau, quan sát thứ tự thực thi.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Thread Creation

**Q1.** `k_thread_create()` nhận 9 tham số. Giải thích ý nghĩa của từng tham số.
`K_NO_WAIT` ở cuối có nghĩa là gì? Nếu đổi thành `K_MSEC(500)` thì điều gì xảy ra?

**Q2.** `K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE)` khác gì với khai báo
`static uint8_t stack_a[STACK_SIZE]`? Tại sao Zephyr cần macro riêng cho stack?

**Q3.** Hàm `thread_a_fn` có signature `void fn(void *p1, void *p2, void *p3)`.
Ba tham số này dùng để làm gì? Trong code này chúng là NULL — nếu muốn truyền
một cấu trúc config vào thread khi tạo, làm thế nào?

### Priority & Scheduling

**Q4.** Zephyr dùng số priority **nhỏ hơn = ưu tiên cao hơn**. Thread với priority 3
và priority 7 — cái nào chạy trước? Điều gì xảy ra khi cả hai đều ở trạng thái READY?

**Q5.** Zephyr có 2 loại thread: **preemptible** (priority 0..CONFIG_NUM_PREEMPT_PRIORITIES-1)
và **cooperative** (priority < 0). Thread trong code này thuộc loại nào?
Cooperative thread có thể bị preempt không?

**Q6.** Khi `thread_a` gọi `k_sleep(K_MSEC(1000))`, nó chuyển sang trạng thái nào?
Vẽ sơ đồ trạng thái của một Zephyr thread (RUNNING → SLEEPING → READY → ...).

**Q7.** Nếu cả hai thread đều **không** gọi `k_sleep()` và chạy `while(1){}` vô tận,
thread nào chiếm CPU? Thread còn lại có bao giờ chạy không? Đây là vấn đề gì?

### Stack

**Q8.** `STACK_SIZE = 512` byte. Nếu thread dùng nhiều local variable hoặc đệ quy sâu
vượt quá 512 byte, điều gì xảy ra? Zephyr phát hiện stack overflow bằng cách nào?
Config nào cần bật?

**Q9.** Làm thế nào để biết một thread đang dùng bao nhiêu stack?
(Gợi ý: `CONFIG_THREAD_ANALYZER` hoặc shell command)

### Luồng code

**Q10.** Sau khi `main()` return (không có `while` ở cuối), `thread_a` và `thread_b`
có còn tiếp tục chạy không? Tại sao? main thread kết thúc có làm hệ thống dừng không?

**Q11.** Thứ tự output log của hai thread có đảm bảo theo thứ tự nhất định không?
Nếu cả hai cùng muốn ghi log, có xảy ra race condition trong logging subsystem không?
