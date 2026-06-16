# ex14 - Power Management

Hệ thống vào low-power idle khi không có việc làm, thức dậy khi:
- Button được nhấn (GPIO interrupt)
- Timeout 10 giây

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### STM32 Power States

**Q1.** STM32F405 có các power state: **Run, Sleep, Stop, Standby**.
Mô tả sự khác biệt về:
- Clock nào còn chạy
- RAM còn giữ không
- Thời gian wakeup
- Dòng điện tiêu thụ (µA)

**Q2.** `CONFIG_PM=y` bật Zephyr Power Management. Khi **tất cả thread đang sleep**,
Zephyr làm gì? Ai quyết định vào power state nào?

**Q3.** Zephyr PM **policy** là gì? Có những policy nào built-in? Làm thế nào để
viết custom policy (ví dụ: không cho vào Stop mode nếu UART đang nhận)?

### Zephyr PM Flow

**Q4.** Khi `k_sem_take(&wakeup_sem, K_SECONDS(10))` được gọi, trace luồng:
1. Thread nào chạy tiếp theo?
2. Scheduler làm gì khi run queue rỗng?
3. PM subsystem được gọi ở đâu?
4. Hardware vào state nào?
5. Wakeup source là gì trong trường hợp này?

**Q5.** Tại sao `LOG_INF()` **trước** khi vào sleep mode có thể bị mất?
(Hint: UART TX cần clock, deferred logging cần work queue)
Giải pháp?

### Device Power Management

**Q6.** `CONFIG_PM_DEVICE=y` cho phép suspend/resume từng device riêng lẻ.
Khi vào Stop mode, GPIO driver có cần làm gì trước khi sleep không?
Interrupt từ GPIO có wake STM32 từ Stop mode không?

**Q7.** `pm_device_action_run(dev, PM_DEVICE_ACTION_SUSPEND)` làm gì?
Có driver nào trong Zephyr tự động suspend khi PM vào sleep không?

### Wakeup Sources

**Q8.** Với STM32F405, liệt kê các nguồn có thể wakeup từ Stop mode:
(GPIO EXTI, RTC alarm, USART, I2C, SPI, WakeUp pin...)
Config nào cần bật trong Zephyr/DTS?

**Q9.** RTC alarm wakeup: hệ thống vào Stop, thức dậy sau đúng 30 giây để
đọc sensor rồi ngủ lại. Cần những component nào? (RTC, alarm, PM)

### Đo lường

**Q10.** Làm thế nào để đo dòng điện thực tế của board khi vào Stop mode?
Kết quả mong đợi với STM32F405 là bao nhiêu µA?
Những thành phần nào trên WeAct board có thể tăng dòng leak (LDO, LED, crystal)?
