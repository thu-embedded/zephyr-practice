# ex17 - ARM Cortex-M Internals

Những gì xảy ra **bên dưới** Zephyr: exception model, NVIC, stack frame,
privilege levels, WFI. Đây là kiến thức phân biệt senior với junior.

Liên quan: ex02 (GPIO interrupt → NVIC), ex04 (thread → PSP/MSP),
ex05 (LDREX/STREX), ex14 (WFI trong idle thread).

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

## Câu hỏi kiểm tra

### Exception Model

**Q1.** Cortex-M có các exception types: Reset, NMI, HardFault, MemManage,
BusFault, UsageFault, SVCall, PendSV, SysTick, và External IRQ.
- PendSV được Zephyr dùng để làm gì? Tại sao phải có priority thấp nhất?
- SVCall (SVC) dùng cho mục đích gì?
- SysTick tạo ra gì trong Zephyr?

**Q2.** Khi một interrupt xảy ra, Cortex-M hardware tự động push 8 registers
lên stack. Trả lời:
- Push lên stack nào — MSP hay PSP? Phụ thuộc vào điều gì?
- Tại sao hardware push `xPSR` và `PC`? Để làm gì khi return?
- `LR` được push có giá trị đặc biệt `EXC_RETURN` — nó là gì?

**Q3.** Sau khi exception handler kết thúc, nó return bằng cách load `EXC_RETURN`
vào PC. Các giá trị `EXC_RETURN` phổ biến:
- `0xFFFFFFF9` — return to Thread mode, MSP
- `0xFFFFFFFD` — return to Thread mode, PSP
- `0xFFFFFFF1` — return to Handler mode, MSP

Zephyr context switch dùng giá trị nào? Tìm trong source:
`arch/arm/core/cortex_m/swap_helper.S`

### NVIC

**Q4.** STM32F405 implement 4 priority bits → 16 priority levels (0–15).
Cortex-M quy ước: số nhỏ = ưu tiên cao.
- Zephyr đặt SysTick ở priority nào?
- Zephyr đặt PendSV ở priority nào? Tại sao phải là cao nhất (hay thấp nhất)?
- Nếu một IRQ có priority **cao hơn** SysTick, nó có thể preempt SysTick không?

**Q5.** **Interrupt nesting**: nếu IRQ A (priority 3) đang chạy và IRQ B (priority 1)
xảy ra, điều gì xảy ra? Stack có bao nhiêu exception frame lúc này?

**Q6.** `BASEPRI` register là gì? Zephyr dùng `irq_lock()` / `irq_unlock()`
— chúng set register nào? Tại sao không dùng `PRIMASK` (disable all IRQ)?

### MSP / PSP / Privilege Levels

**Q7.** Cortex-M có 2 stack pointer: **MSP** (Main) và **PSP** (Process).
Zephyr dùng:
- MSP cho: ?
- PSP cho: ?

Khi thread A đang chạy và interrupt xảy ra, SP chuyển từ PSP sang MSP thế nào?
Ai làm điều đó — hardware hay software?

**Q8.** `CONTROL.SPSEL=1` nghĩa là gì? Zephyr set bit này khi nào?
Tìm trong `arch/arm/core/cortex_m/prep_c.c`.

**Q9.** Cortex-M có 2 privilege levels: **Privileged** và **Unprivileged**.
Zephyr userspace (`CONFIG_USERSPACE=y`) dùng unprivileged mode cho user threads.
Unprivileged code không thể làm gì? MPU liên quan thế nào?

### Context Switch

**Q10.** Zephyr context switch trên Cortex-M xảy ra theo chuỗi sau.
Điền vào chỗ trống:

```
SysTick interrupt
  → SysTick_Handler()
  → z_arm_pendsv()  [set PendSV pending]
  → SysTick return
  → PendSV fires (lowest priority → fires khi không còn IRQ nào)
  → PendSV_Handler() / z_arm_pendsv()
  → [1] Save R4-R11 của thread hiện tại lên ___
  → [2] Switch ___ sang thread mới
  → [3] Restore R4-R11 của thread mới từ ___
  → EXC_RETURN → hardware restore R0-R3, R12, LR, PC, xPSR
  → Thread mới chạy tiếp
```

**Q11.** Tại sao hardware chỉ tự động save R0–R3, R12, LR, PC, xPSR (8 regs)
mà không save R4–R11? Ai có trách nhiệm save R4–R11?

### WFI / WFE

**Q12.** `__WFI()` vs `__WFE()`:
- WFI: wake up khi có interrupt **pending** hay **taken**?
- WFE: thêm điều kiện gì để wake up?
- Zephyr idle thread dùng cái nào? Tìm trong `arch/arm/core/cortex_m/cpu_idle.S`
