# ex09 - UART Async

Nhận dữ liệu từ USART2 (PA2 TX / PA3 RX) dùng async API với double buffer,
đưa vào message queue, rồi echo lại.

## Kết nối

```
STM32F405  ←→  USB-UART adapter
PA2 (TX)   →   RX
PA3 (RX)   ←   TX
GND        —   GND
```

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
minicom -D /dev/ttyUSB0 -b 115200
```

## Câu hỏi kiểm tra

### Ba chế độ UART trong Zephyr

**Q1.** Zephyr UART có 3 API: **polling**, **interrupt-driven**, **async**.
So sánh:

| | Polling | Interrupt | Async (DMA) |
|--|---------|-----------|-------------|
| CPU usage khi chờ | ? | ? | ? |
| Config cần | ? | ? | ? |
| Phù hợp với | ? | ? | ? |

**Q2.** `CONFIG_UART_ASYNC_API=y` bật gì? Driver STM32 UART có dùng DMA không
khi async API được bật?

### Double Buffer

**Q3.** Code dùng `rx_buf` và `rx_buf2` — đây là kỹ thuật **double buffering**.
Giải thích tại sao cần 2 buffer thay vì 1. Điều gì xảy ra nếu chỉ có 1 buffer
và DMA vẫn đang ghi vào nó khi callback được gọi?

**Q4.** `UART_RX_BUF_REQUEST` event xảy ra khi nào? Nếu không cung cấp buffer
mới trong callback này, điều gì xảy ra với dữ liệu đến?

**Q5.** `uart_rx_enable(..., 100 * USEC_PER_MSEC)` — tham số cuối là **idle timeout**.
Nó hoạt động thế nào? Khi nào `UART_RX_RDY` được trigger nếu không đủ `RX_BUF_SIZE` bytes?

### Callback Context

**Q6.** `uart_cb` chạy trong context nào? Tại sao dùng `k_msgq_put(..., K_NO_WAIT)`
thay vì `K_FOREVER`?

**Q7.** `UART_RX_DISABLED` event xảy ra khi nào? Tại sao trong handler này gọi lại
`uart_rx_enable()`? Nếu không làm vậy, điều gì xảy ra?

### TX

**Q8.** `uart_tx(uart, msg.data, msg.len, SYS_FOREVER_US)` — nếu TX buffer của
UART đang bận, hàm này block không? Khi nào `UART_TX_DONE` event được gọi?

**Q9.** Nếu gọi `uart_tx()` hai lần liên tiếp mà lần đầu chưa xong, lần thứ hai
trả về lỗi gì? Cần xử lý thế nào?

### Thực tế

**Q10.** Console (LOG, printk) đang dùng USART1 (PA9/PA10). Code này dùng USART2.
Nếu cần dùng cả hai cùng lúc, có cần thêm config gì không?

**Q11.** Trong ứng dụng nhận lệnh ASCII (ví dụ `"LED ON\r\n"`), timeout 100ms có
hợp lý không? Bạn sẽ chọn timeout thế nào cho giao thức binary fixed-length packet?
