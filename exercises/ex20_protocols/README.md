# ex20 - Protocol Deep Dive (UART / I2C / SPI / CAN)

Hiểu protocol ở mức signal + frame, không chỉ gọi API.
Khi đi phỏng vấn, interviewer thường yêu cầu vẽ timing diagram.

Liên quan: ex09 (UART async), ex10 (I2C sensor API vs raw).

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

Dùng **logic analyzer** (sigrok, PulseView) để capture và verify timing.

## Câu hỏi kiểm tra

### UART Timing

**Q1.** Vẽ timing diagram của byte `0x55` (0b01010101) ở 115200 baud, 8N1:
- Đánh dấu: START bit, 8 data bits (LSB first), STOP bit
- Tính thời gian của mỗi bit
- Tổng thời gian để truyền 1 byte?

**Q2.** STM32 USART1 clock = 84 MHz. Tính giá trị BRR register cho 115200 baud.
Baud rate thực tế sau khi làm tròn là bao nhiêu? Sai số bao nhiêu %?
Sai số tối đa cho phép để UART hoạt động đúng là bao nhiêu?

**Q3.** UART không có clock chung (asynchronous). Làm thế nào receiver
đồng bộ với transmitter? START bit đóng vai trò gì? Tại sao cần oversampling (x16)?

**Q4.** RS-232 vs TTL UART vs RS-485:
- Mức điện áp của mỗi loại?
- RS-485 có thể multi-drop (nhiều node trên 1 bus) — UART thông thường có không?
- STM32 USART output là loại nào?

### I2C Deep Dive

**Q5.** Vẽ timing diagram cho I2C transaction đọc 1 byte từ address `0x68`,
register `0x75`:
```
START → [0xD0] → ACK → [0x75] → ACK → REPEATED_START →
[0xD1] → ACK → [DATA] → NACK → STOP
```
- `0xD0` = ? (7-bit addr `0x68` << 1 | W)
- `0xD1` = ? (7-bit addr `0x68` << 1 | R)
- Tại sao receiver gửi NACK thay vì ACK cho byte cuối?

**Q6.** I2C clock stretching là gì? Slave có thể kéo SCL xuống LOW để làm gì?
Nếu master không hỗ trợ clock stretching và slave dùng nó, điều gì xảy ra?

**Q7.** I2C pull-up resistor: tại sao cần? Giá trị điển hình cho 400kHz (Fast mode)?
Nếu pull-up quá lớn (ví dụ 100kΩ), vấn đề gì xảy ra? Quá nhỏ (1kΩ)?

**Q8.** `i2c_transfer()` trong code dùng 2 `i2c_msg`. Nếu dùng `I2C_MSG_RESTART`
thay vì 2 message riêng, khác nhau thế nào? Khi nào cần REPEATED START?

### SPI Deep Dive

**Q9.** SPI có 4 mode (CPOL/CPHA). Vẽ timing diagram cho:
- Mode 0 (CPOL=0, CPHA=0): clock idle LOW, sample on rising edge
- Mode 3 (CPOL=1, CPHA=1): clock idle HIGH, sample on rising edge

MPU6050 SPI dùng mode nào? ST7789 LCD dùng mode nào?

**Q10.** SPI full-duplex: MOSI và MISO hoạt động **đồng thời**.
Nếu chỉ muốn đọc (không cần ghi), bạn phải gửi gì trên MOSI?
`spi_read()` vs `spi_transceive()` khác nhau thế nào?

**Q11.** CS (Chip Select) được Zephyr quản lý tự động nếu config đúng,
hoặc manual qua GPIO. Tại sao phải giữ CS LOW trong suốt transaction?
Nếu CS bị deassert giữa chừng, điều gì xảy ra?

### CAN Deep Dive

**Q12.** Vẽ cấu trúc một CAN 2.0A Standard Frame:
```
[SOF][ID:11bit][RTR][IDE][r0][DLC:4][DATA:0-8bytes][CRC:15][CRC_DEL][ACK][ACK_DEL][EOF]
```
Giải thích từng trường: SOF, RTR, IDE, DLC, ACK?

**Q13.** CAN bus dùng **differential signaling** (CANH/CANL).
- Dominant bit = ? (CANH-CANL = ?)
- Recessive bit = ? (CANH-CANL = ?)
- Tại sao dominant "wins" over recessive? (Wired-AND)

**Q14.** CAN **arbitration**: 3 node cùng gửi lúc t=0.
Node A: ID=0x100, Node B: ID=0x0FF, Node C: ID=0x200.
Node nào "wins"? Tại sao? Các node thua có mất frame không?

**Q15.** `can_send()` trả về lỗi nếu không có node nào ACK.
Tại sao CAN cần ACK từ ít nhất 1 node khác?
Trong test setup chỉ có 1 node, làm sao test CAN mà không bị lỗi?

### So sánh tổng quát

**Q16.** Điền bảng so sánh:

| | UART | I2C | SPI | CAN |
|--|------|-----|-----|-----|
| Số dây (min) | 2 | 2 | 4 | 2 |
| Multi-master | N | Y | N | Y |
| Max tốc độ | ? | ? | ? | ? |
| Max distance | ? | ? | ? | ? |
| Error detection | ? | ? | ? | ? |
| Dùng cho | ? | ? | ? | ? |
