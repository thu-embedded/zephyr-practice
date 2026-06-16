# ex19 - JTAG/SWD Debug

Luyện debug firmware thực tế: GDB qua SWD, watchpoint, HardFault analysis,
SEGGER SystemView để trace thread execution.

Liên quan: ex17 (exception frame, HardFault), ex04 (threads — xem trong GDB),
ex18 (symbols — GDB cần .elf).

## Setup phần cứng

```
STM32F405    ←→    ST-Link V2 / J-Link
SWDIO (PA13) ←→    SWDIO
SWDCLK(PA14) ←→    SWDCLK
GND          ←→    GND
3.3V         ←→    VTref (tùy adapter)
```

## Build (với debug info)

```bash
west build -b weact_stm32f405_core -- -DCONFIG_DEBUG=y
west flash
```

## GDB Session

```bash
# Terminal 1: OpenOCD server
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# Terminal 2: GDB client
arm-zephyr-eabi-gdb build/zephyr/zephyr.elf
(gdb) target remote :3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## Câu hỏi kiểm tra

### SWD vs JTAG

**Q1.** So sánh SWD (Serial Wire Debug) và JTAG:

| | JTAG | SWD |
|--|------|-----|
| Số pin | ? | ? |
| Tốc độ | ? | ? |
| Hỗ trợ multi-drop | ? | ? |
| Phổ biến với ARM | ? | ? |

**Q2.** PA13 và PA14 trên STM32 là SWDIO/SWDCLK. Nếu firmware vô tình
`gpio_pin_configure(PA13, GPIO_OUTPUT)`, điều gì xảy ra với debug connection?
Làm thế nào để recover?

### GDB Workflow

**Q3.** Với các lệnh GDB sau, giải thích mỗi lệnh làm gì:
```gdb
(gdb) break main.c:45
(gdb) watch g_counter
(gdb) info threads
(gdb) thread 2
(gdb) bt
(gdb) print g_buffer
(gdb) x/16xb g_buffer
(gdb) set g_counter = 0
(gdb) disassemble fill_buffer
```

**Q4.** **Hardware breakpoint** vs **software breakpoint**:
- Software breakpoint thay thế instruction bằng gì? (`BKPT 0xAB`)
- Hardware breakpoint dùng gì trong Cortex-M? Bao nhiêu cái?
- Khi nào bắt buộc phải dùng hardware breakpoint?

**Q5.** **Watchpoint** (data breakpoint): `watch g_counter` dừng execution
khi `g_counter` được **ghi**. Cortex-M có bao nhiêu watchpoint?
`rwatch` và `awatch` khác `watch` thế nào?

### HardFault Analysis

**Q6.** Uncomment `maybe_null_deref(true)` và flash. Board sẽ HardFault.
Zephyr in ra fault information. Giải thích output sau:
```
>>> ZEPHYR FATAL ERROR 0: CPU exception on CPU 0
Current thread: 0x20000500 (main)
Faulting instruction address: 0x080012A4
...
r0: 0x00000000  r1: 0x00000000  r2: 0x00000000  r3: 0x00000000
r12: 0x00000000 lr: 0x080012B1  pc: 0x080012A4  xpsr: 0x01000000
```
- `pc: 0x080012A4` — dùng GDB/objdump để tìm đây là dòng code nào
- `lr: 0x080012B1` — đây là gì?

**Q7.** Các loại fault trên Cortex-M:
- **HardFault**: ?
- **MemManage**: ?
- **BusFault**: ?
- **UsageFault**: ?

Null pointer deref gây ra fault loại nào? Tại sao?

**Q8.** Khi HardFault xảy ra trong một interrupt handler (không phải thread),
stack frame nằm trên MSP hay PSP? Làm thế nào GDB đọc được caller?

### SEGGER SystemView

**Q9.** SEGGER SystemView record event gì? (thread switch, ISR enter/exit,
semaphore give/take, ...) Cần thêm config nào vào Zephyr?

**Q10.** SystemView dùng RTT để transfer data. Điều này có ảnh hưởng đến
timing của firmware không? So sánh với UART logging (ex03).

### Thực chiến

**Q11.** Bug `compute(200, 100)` trả về 44 thay vì 300.
Dùng GDB để:
1. Set breakpoint tại `compute()`
2. Step vào trong
3. `print tmp` sau phép cộng
4. Giải thích tại sao bị overflow và fix code.
