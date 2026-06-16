# ex18 - Linker Script & Memory Map

Hiểu binary được tổ chức trong Flash và RAM thế nào.
Đây là kiến thức cần thiết để debug firmware size, stack overflow,
và hiểu tại sao `.data` cần được copy từ Flash sang RAM lúc boot.

Liên quan: ex11 (NVS flash partition), ex17 (MSP/PSP stack addresses),
ex04 (thread stack placement).

## Build & Phân tích

```bash
west build -b weact_stm32f405_core .
west flash

# Xem memory usage
arm-zephyr-eabi-size build/zephyr/zephyr.elf

# Xem tất cả sections
arm-zephyr-eabi-objdump -h build/zephyr/zephyr.elf

# Xem địa chỉ từng symbol
arm-zephyr-eabi-nm --numeric-sort build/zephyr/zephyr.elf | head -50

# Xem linker script Zephyr tạo ra
cat build/zephyr/linker.cmd
```

## Câu hỏi kiểm tra

### Sections

**Q1.** Giải thích 5 section chính và vị trí của chúng:

| Section | Trong Flash? | Trong RAM? | Nội dung |
|---------|-------------|-----------|---------|
| `.text` | ? | ? | ? |
| `.rodata` | ? | ? | ? |
| `.data` | ? | ? | ? |
| `.bss` | ? | ? | ? |
| `.noinit` | ? | ? | ? |

**Q2.** Biến `.data` nằm trong RAM khi runtime, nhưng giá trị khởi tạo
(`boot_count = 0xDEAD`) phải lưu ở đâu? Ai copy từ Flash sang RAM?
Tìm trong Zephyr: `arch/arm/core/cortex_m/prep_c.c` hoặc startup assembly.

**Q3.** `.bss` được zero-fill — bởi ai, khi nào? Nếu startup code bị lỗi
không zero `.bss`, điều gì xảy ra với `error_count` trong code?

**Q4.** `.noinit` không bị xóa khi reset. Điều này cho phép gì?
`reset_magic` hoạt động thế nào để phân biệt warm reset vs cold boot?
Cẩn thận: `.noinit` có vấn đề gì sau khi power-off hoàn toàn?

### STM32F405 Memory Map

**Q5.** STM32F405 có memory map:
```
0x00000000  — aliased (Flash hoặc RAM tùy BOOT pin)
0x08000000  — Flash (1MB)
0x20000000  — SRAM1 (112KB)
0x2001C000  — SRAM2 (16KB)
0x10000000  — CCM RAM (64KB, chỉ CPU access, không có DMA)
0x40000000  — Peripheral registers
0xE0000000  — Cortex-M core peripherals (NVIC, SysTick, SCB...)
```

`fast_buffer` được đặt vào `.dtcm` (CCM RAM tại 0x10000000).
Tại sao CCM nhanh hơn SRAM thông thường? Khi nào nên dùng?

**Q6.** Vector table (interrupt vector) nằm ở địa chỉ nào sau khi flash?
Làm thế nào Cortex-M biết địa chỉ của vector table?
Register nào chứa địa chỉ đó? (`SCB->VTOR`)

### Linker Script

**Q7.** Mở `build/zephyr/linker.cmd`. Tìm section `.text` và giải thích:
```
.text : {
    *(.text*)
    *(.isr_vector)
    ...
} > FLASH
```
`*(.text*)` nghĩa là gì? `> FLASH` nghĩa là gì?

**Q8.** Zephyr đặt Zephyr kernel objects (threads, semaphores, v.v.) vào
section đặc biệt như `_k_thread_list`. Tại sao không dùng mảng thông thường?
`K_THREAD_DEFINE` macro đặt struct vào section nào?

**Q9.** Zephyr có `ITCM` section (Instruction TCM) trên một số MCU.
Code trong ITCM chạy nhanh hơn thế nào so với code trong Flash?
Khi nào nên đặt function vào ITCM?

### Phân tích binary

**Q10.** Chạy `arm-zephyr-eabi-size build/zephyr/zephyr.elf`.
Output có 3 cột: `text`, `data`, `bss`. Cột nào tốn Flash? Cột nào tốn RAM?
Công thức: **Flash used = ? , RAM used = ?**

**Q11.** `arm-zephyr-eabi-nm --numeric-sort zephyr.elf | grep -E "^20"` liệt kê
tất cả symbol trong RAM (địa chỉ bắt đầu bằng 0x20...).
Tìm symbol lớn nhất — đó là gì? (Hint: thường là logging buffer hoặc stack)

**Q12.** Nếu build báo:
```
region `FLASH' overflowed by 2048 bytes
```
Bạn sẽ làm gì để giảm Flash usage? Liệt kê ít nhất 4 kỹ thuật.
