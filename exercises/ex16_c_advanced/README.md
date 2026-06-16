# ex16 - C Nâng Cao (Dành cho Embedded)

Những C pattern mà Zephyr codebase dùng hàng ngày. Hiểu đây là hiểu
**tại sao** Zephyr được viết theo cách đó.

Liên quan: ex01 (GPIO register), ex02 (ISR), ex05 (semaphore LDREX/STREX),
ex10 (sensor driver API), ex11 (linker sections).

## Build

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### volatile

**Q1.** Nếu bỏ `volatile` khỏi con trỏ `sr` trong `uart_send_byte_raw()`:
```c
uint32_t *sr = (uint32_t *)UART_SR_ADDR;  /* không volatile */
while (!(*sr & UART_SR_TXE)) {}
```
Compiler có thể tối ưu loop này thành gì? Tại sao lại sai?
Disassemble để xem sự khác biệt: `arm-zephyr-eabi-objdump -d build/zephyr/zephyr.elf`

**Q2.** `volatile` và `const` có thể kết hợp không?
```c
volatile const uint32_t *ro_reg = (volatile const uint32_t *)0x40000000;
```
Ý nghĩa của khai báo này là gì? Khi nào dùng trong embedded?

**Q3.** Trong Zephyr, biến dùng giữa ISR và thread có cần `volatile` không?
Tại sao Zephyr dùng `atomic_t` thay vì `volatile int`? Sự khác biệt là gì?

### restrict

**Q4.** Compile cả hai hàm `my_memcpy` và `my_memcpy_restrict` với `-O2`.
Disassemble và so sánh: số instruction khác nhau thế nào?
`__restrict` báo cho compiler biết điều gì?

**Q5.** Nếu gọi `my_memcpy_restrict(buf, buf + 1, 10)` — hai pointer overlap.
Kết quả là undefined behavior hay lỗi runtime? Ai chịu trách nhiệm kiểm tra?

### struct alignment & padding

**Q6.** Vẽ memory layout của `struct bad_layout` byte-by-byte.
Tại sao CPU ARM cần alignment? Điều gì xảy ra nếu đọc `uint32_t` tại địa chỉ lẻ
trên Cortex-M3 không có unaligned access support?

**Q7.** `__attribute__((packed))` loại bỏ padding. Khi nào dùng?
Khi nào **không nên** dùng? (Hint: performance, unaligned access fault, DMA)

**Q8.** Zephyr struct `gpio_dt_spec`:
```c
struct gpio_dt_spec {
    const struct device *port;
    gpio_pin_t pin;
    gpio_dt_flags_t dt_flags;
};
```
Trên ARM 32-bit, `sizeof(struct gpio_dt_spec)` là bao nhiêu? Vẽ layout.

**Q9.** Struct được truyền vào message queue (ex06) bằng cách **copy**.
Nếu struct có padding bytes, các byte padding đó có giá trị xác định không?
Điều này ảnh hưởng thế nào nếu so sánh hai struct bằng `memcmp()`?

### Function Pointers & Driver API

**Q10.** `struct sensor_driver` trong code là pattern gì?
Tìm trong Zephyr source cấu trúc `struct gpio_driver_api` — nó có gì?
Tại sao Zephyr dùng pattern này thay vì virtual function của C++?

**Q11.** `const struct sensor_driver fake_sensor = { ... }` được đặt vào section
nào trong binary — `.data`, `.rodata`, hay `.bss`? Tại sao quan trọng với
embedded (hint: Flash vs RAM)?

### Bit Manipulation

**Q12.** `GENMASK(5, 3)` expand thành giá trị hex nào?
`__builtin_ctz(mask)` trả về gì với mask đó?
Verify bằng cách tính tay rồi kiểm tra với log output.

**Q13.** Viết macro `REG_GET_FIELD(reg, mask)` để đọc giá trị một field từ register.
(Ngược lại với `reg_set_field()` trong code)

**Q14.** Tại sao thứ tự:
```c
reg &= ~mask;    /* bước 1: clear */
reg |= val;      /* bước 2: set   */
```
phải là 2 bước atomic nếu register đang được ISR đọc đồng thời?
Trong Zephyr, cách nào để làm read-modify-write an toàn với ISR?
