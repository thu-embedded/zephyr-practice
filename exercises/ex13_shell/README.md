# ex13 - Shell Subsystem

Interactive CLI qua UART. Gõ lệnh để điều khiển LED và xem thông tin hệ thống.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
minicom -D /dev/ttyACM0 -b 115200   # hoặc USART1 PA9/PA10
```

## Thử ngay

```
uart:~$ help
uart:~$ led on
uart:~$ led off
uart:~$ led blink 5
uart:~$ info
uart:~$ kernel threads     # xem tất cả threads
uart:~$ kernel stacks      # xem stack usage
```

## Câu hỏi kiểm tra

### Shell Architecture

**Q1.** Shell chạy trong thread riêng hay dùng system work queue?
Priority của shell thread là bao nhiêu? Config nào thay đổi được?

**Q2.** `SHELL_CMD_REGISTER` là macro — nó đặt data vào section nào trong binary?
Tại sao đây là cách tốt hơn so với dùng một mảng command[] trong main()?

**Q3.** `SHELL_STATIC_SUBCMD_SET_CREATE` tạo subcommand tree. Zephyr shell có hỗ trợ
**tab completion** không? Điều kiện để tab completion hoạt động?

### Command Handler

**Q4.** `cmd_led_blink()` gọi `k_sleep()` bên trong — điều này có vấn đề gì?
Trong thời gian blink, shell có nhận được lệnh mới không?

**Q5.** `shell_print()` vs `printk()` vs `LOG_INF()` — khi dùng trong shell command,
cái nào đảm bảo output đúng terminal của shell đang gọi lệnh? Nếu có 2 shell
(UART + USB CDC), `printk()` in ra đâu?

**Q6.** `argc` và `argv` hoạt động giống C `main()`. Với lệnh `led blink 5 fast`,
`argc` = ? và `argv[]` = ?

### Built-in Commands

**Q7.** `kernel threads` là built-in command. Thử chạy và giải thích output:
các cột `name`, `prio`, `state`, `stack size`, `used` có nghĩa gì?

**Q8.** `kernel stacks` hiển thị stack watermark. Watermark được tính thế nào?
(Gợi ý: pattern filling) Tại sao không phải là số chính xác?

### Shell Backends

**Q9.** Zephyr shell hỗ trợ nhiều backend: Serial, RTT, Telnet, USB CDC.
Nếu muốn shell qua USB CDC (không cần USB-UART adapter), cần thêm config nào?
Kết hợp với ex12, làm thế nào để có cả shell lẫn log riêng biệt?

### Thực tế

**Q10.** Trong production firmware, shell có nên được bật không?
Nếu cần bật để debug nhưng tắt trong release, dùng Kconfig thế nào?

**Q11.** `atoi(argv[1])` không báo lỗi khi input không phải số. Viết lại
`cmd_led_blink()` dùng `shell_strtol()` để xử lý input invalid đúng cách.
