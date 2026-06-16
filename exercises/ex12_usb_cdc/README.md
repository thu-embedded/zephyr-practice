# ex12 - USB CDC-ACM (Virtual COM Port)

Board xuất hiện là `/dev/ttyACM0` trên Linux / `COM` port trên Windows.
Không cần USB-UART adapter, dùng trực tiếp cổng USB OTG (PA11/PA12).

## Kết nối

```
STM32F405  →  PC
PA11 (DM)  →  USB D-
PA12 (DP)  →  USB D+
(dùng cable USB micro/type-C tùy board)
```

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
# Trên host:
minicom -D /dev/ttyACM0 -b 115200
```

## Câu hỏi kiểm tra

### USB Stack

**Q1.** CDC-ACM là gì? Viết tắt của gì? Tại sao nó được gọi là "virtual COM port"?
Host OS cần driver gì để nhận ra device này?

**Q2.** `zephyr_udc0` trong overlay trỏ đến node nào trong DTS gốc của board?
`usbotg_fs` là gì, khác gì `usbotg_hs`?

**Q3.** Khi cắm USB, host và device thực hiện quá trình gì? Tại sao sau khi flash
xong đôi khi cần rút/cắm lại USB?

### Interrupt-driven UART (cho CDC)

**Q4.** CDC-ACM trong Zephyr implement interface UART (interrupt-driven).
`uart_irq_rx_ready()` được gọi khi nào? Khác với async API (ex09) thế nào?

**Q5.** `uart_fifo_fill()` có block không nếu TX buffer đầy? Hàm này thread-safe không?

**Q6.** Nếu host không mở `/dev/ttyACM0`, data gửi từ device có bị mất không?
CDC-ACM có flow control không?

### Overlay & chosen

**Q7.** Overlay này đổi `zephyr,console` và `zephyr,shell-uart` sang `cdc_acm_uart0`.
Điều này có nghĩa là `LOG_INF()` và `printk()` sẽ in ra đâu?
USART1 (PA9/PA10) còn hoạt động không?

**Q8.** Nếu muốn vừa có log trên USART1 vừa có CDC-ACM cho shell, có thể không?
Cần config thế nào?

### Thực tế

**Q9.** So sánh debug qua USB CDC-ACM vs UART vs RTT (SEGGER):

| | USB CDC | UART | RTT |
|--|---------|------|-----|
| Hardware cần thêm | ? | ? | ? |
| Tốc độ tối đa | ? | ? | ? |
| Có thể dùng khi bootloader | ? | ? | ? |

**Q10.** WeAct STM32F405 board dùng USB OTG FS. Ngoài CDC-ACM, còn có thể implement
những USB class nào khác với Zephyr? (Liệt kê ít nhất 3)
