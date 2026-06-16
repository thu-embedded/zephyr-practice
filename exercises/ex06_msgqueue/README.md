# ex06 - Message Queue (Producer/Consumer)

Thread producer tạo data mỗi 200ms, consumer xử lý mỗi 500ms.
Queue 8 phần tử làm buffer khi tốc độ hai bên không khớp.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Message Queue

**Q1.** `K_MSGQ_DEFINE(sensor_queue, sizeof(struct sensor_data), QUEUE_LEN, 4)` —
tham số cuối `4` là alignment. Tại sao cần alignment? Điều gì xảy ra nếu để `1`
với struct có trường `uint32_t`?

**Q2.** Message queue trong Zephyr **copy by value**, không phải by pointer.
Ưu điểm của cách này là gì? Trong code, struct `sensor_data` được copy bao nhiêu lần
từ lúc tạo đến lúc consumer đọc?

**Q3.** Producer gọi `k_msgq_put(..., K_NO_WAIT)`. Nếu đổi thành `K_FOREVER`,
điều gì xảy ra khi queue đầy? Producer thread sẽ ở trạng thái nào?

**Q4.** Consumer gọi `k_msgq_get(..., K_FOREVER)`. Nếu queue rỗng và không có data
mới trong 10 giây, consumer thread tiêu tốn CPU không?

### Producer/Consumer Timing

**Q5.** Producer chạy mỗi 200ms, consumer mỗi 500ms. Tính toán:
- Sau bao lâu queue sẽ đầy (8 slots)?
- Sau khi queue đầy, mỗi giây mất bao nhiêu sample?
- Làm thế nào để fix vấn đề này mà không thay đổi tốc độ hai bên?

**Q6.** `k_uptime_get_32()` trả về milliseconds kể từ khi boot. Giá trị này
overflow sau bao lâu? Điều gì xảy ra với timestamp khi overflow?

### So sánh IPC primitives

**Q7.** So sánh 3 cơ chế IPC trong Zephyr:

| | Semaphore | Message Queue | Pipe |
|---|---|---|---|
| Truyền data | ? | ? | ? |
| Số lượng chờ | ? | ? | ? |
| Dùng khi nào | ? | ? | ? |

**Q8.** Nếu `sensor_data` rất lớn (ví dụ 1KB), copy-by-value của message queue
lãng phí memory. Giải pháp là gì? (Gợi ý: memory slab + pointer trong queue)

### Race Condition & Thread Safety

**Q9.** Nếu có **2 producer** cùng gọi `k_msgq_put()` đồng thời, có xảy ra
race condition không? Zephyr đảm bảo thread-safety của message queue thế nào?

**Q10.** `k_msgq_purge()` dùng để làm gì? Trong hệ thống thực tế (ví dụ: đổi
config cảm biến), khi nào cần gọi purge?

### Luồng code tổng thể

**Q11.** Vẽ sơ đồ luồng thực thi của hệ thống này theo thời gian (timeline):
producer, consumer, và scheduler làm gì tại mỗi mốc thời gian t=0, 200, 400,
500, 600, 800, 1000ms?
