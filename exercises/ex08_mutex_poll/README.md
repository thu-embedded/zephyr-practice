# ex08 - Mutex & k_poll

Hai chủ đề nâng cao:
- **Mutex**: bảo vệ shared data giữa writer/reader thread
- **k_poll**: chờ nhiều event source cùng lúc không cần nhiều thread

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Mutex

**Q1.** Tại sao phải dùng mutex thay vì semaphore để bảo vệ `shared_data`?
Điều gì xảy ra nếu bỏ mutex và hai thread cùng truy cập `shared_data`?
Trên Cortex-M, read/write `int32_t` có atomic không?

**Q2.** `k_mutex_lock(&data_mutex, K_FOREVER)` — nếu mutex đang bị thread khác
giữ, thread hiện tại ở trạng thái nào? Nó có tiêu tốn CPU không?

**Q3.** **Priority Inversion** là gì? Cho ví dụ với 3 thread (H=high, M=medium, L=low priority)
và một mutex. Tại sao đây là vấn đề nguy hiểm trong RTOS?

**Q4.** **Priority Inheritance** giải quyết priority inversion thế nào?
Zephyr mutex có hỗ trợ tính năng này không? (`CONFIG_PRIORITY_CEILING`)

**Q5.** Nếu thread giữ mutex mà bị terminate (do lỗi), mutex có được release không?
Zephyr xử lý trường hợp này thế nào?

**Q6.** Trong code, writer lock mutex, write 2 fields, rồi unlock. Tại sao không
chỉ lock từng field riêng lẻ? (Hint: atomicity của cả struct)

### k_poll

**Q7.** `k_poll(events, 2, K_FOREVER)` — nếu cả `event_a` lẫn `event_b` đều
ready cùng lúc, `k_poll` return khi nào? Cả hai event có được xử lý trong một
lần gọi `k_poll` không?

**Q8.** Tại sao sau khi xử lý event phải reset `events[0].state = K_POLL_STATE_NOT_READY`?
Nếu quên, điều gì xảy ra ở vòng lặp tiếp theo?

**Q9.** `k_poll` có thể chờ những loại object nào ngoài semaphore?
(Liệt kê `K_POLL_TYPE_*`)

**Q10.** So sánh hai cách chờ nhiều event:
- **Cách 1**: 1 thread dùng `k_poll` chờ N semaphore
- **Cách 2**: N thread, mỗi thread chờ 1 semaphore

Tradeoff về RAM, complexity, và latency?

### Deadlock

**Q11.** **Deadlock** xảy ra khi nào với mutex? Cho ví dụ với 2 thread và 2 mutex.
Zephyr có cơ chế detect deadlock không? Làm thế nào để tránh?
