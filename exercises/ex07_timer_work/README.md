# ex07 - Timer & Work Queue

Ba pattern quan trọng nhất khi làm việc với thời gian trong Zephyr:
- `k_timer` periodic: nhấp nháy LED mỗi 500ms
- `k_timer` one-shot + `k_work`: log sau 3 giây
- `k_work_delayable`: debounce button 50ms

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### k_timer

**Q1.** `k_timer_start(&blink_timer, K_MSEC(500), K_MSEC(500))` — tham số 2 và 3
là gì? Nếu tham số 3 là `K_NO_WAIT`, timer chạy mấy lần?

**Q2.** Hàm `blink_timer_fn()` chạy trong context nào — ISR hay thread?
Tại sao `gpio_pin_toggle_dt()` được phép gọi ở đây nhưng `LOG_INF()` thì không?

**Q3.** `k_timer_init()` nhận 3 tham số: timer, expiry_fn, stop_fn.
`stop_fn` được gọi khi nào? Dùng để làm gì?

**Q4.** Nếu timer callback mất nhiều thời gian hơn chu kỳ timer (ví dụ callback
mất 600ms nhưng period là 500ms), điều gì xảy ra? Zephyr xử lý thế nào?

### k_work & System Work Queue

**Q5.** System work queue là gì? Nó chạy trong thread nào, priority bao nhiêu?
`k_work_submit()` có block không? Nó làm gì chính xác?

**Q6.** Tại sao trong `oneshot_timer_fn()` gọi `k_work_submit()` thay vì gọi
`LOG_INF()` trực tiếp? (Gợi ý: timer callback context)

**Q7.** Nếu `k_work_submit()` được gọi 5 lần liên tiếp trước khi work queue kịp
xử lý, `log_work_fn` sẽ chạy mấy lần?

### k_work_delayable

**Q8.** `k_work_reschedule(&debounce_work, K_MSEC(50))` khác `k_work_schedule()`
ở điểm nào? Tại sao debounce cần `reschedule` thay vì `schedule`?

**Q9.** Phân tích debounce flow: button bị bounce 5 lần trong 30ms, sau đó ổn định.
Với `reschedule(K_MSEC(50))`, `debounce_fn` sẽ chạy mấy lần và khi nào?

### So sánh cơ chế timing

**Q10.** So sánh 4 cách tạo delay/timing trong Zephyr:

| Cơ chế | Chạy trong | Block thread? | Độ chính xác | Dùng khi |
|--------|-----------|--------------|-------------|---------|
| `k_sleep()` | ? | ? | ? | ? |
| `k_timer` | ? | ? | ? | ? |
| `k_work_delayable` | ? | ? | ? | ? |
| Hardware timer (counter driver) | ? | ? | ? | ? |

**Q11.** `k_timer` dùng kernel tick. Nếu `CONFIG_SYS_CLOCK_TICKS_PER_SEC=100`
(mặc định thường là 100 hoặc 1000), độ phân giải tối thiểu của `k_timer` là bao nhiêu ms?
Điều này ảnh hưởng thế nào đến `K_MSEC(1)`?
