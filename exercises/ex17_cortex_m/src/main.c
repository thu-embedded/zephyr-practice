#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/arch/cpu.h>
#include <cmsis_core.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* ================================================================
 * 1. Đọc core registers
 * ================================================================ */

static void print_core_info(void)
{
	/* CPUID register */
	uint32_t cpuid = SCB->CPUID;
	LOG_INF("CPUID: 0x%08X", cpuid);
	LOG_INF("  Implementer : 0x%02X (0x41=ARM)", (cpuid >> 24) & 0xFF);
	LOG_INF("  Part number : 0x%03X (0xC24=Cortex-M4)", (cpuid >> 4) & 0xFFF);
	LOG_INF("  Revision    : r%dp%d",
		(cpuid >> 20) & 0xF, cpuid & 0xF);

	/* SysTick */
	LOG_INF("SysTick LOAD: %u (reload value)", SysTick->LOAD);
	LOG_INF("SysTick frequency: %u Hz", CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC);
}

/* ================================================================
 * 2. NVIC: xem priority của interrupt
 * ================================================================ */

static void print_nvic_info(void)
{
	/* USART1 IRQ number trên STM32F405 */
	IRQn_Type usart1_irq = 37; /* USART1_IRQn */

	/* Zephyr set priority trong arch/arm/core */
	uint32_t prio = NVIC_GetPriority(usart1_irq);
	LOG_INF("USART1 NVIC priority: %u", prio);

	/* Priority bits: STM32F405 implement 4 bits (16 levels) */
	LOG_INF("Priority bits implemented: %u",
		(uint32_t)__NVIC_PRIO_BITS);
}

/* ================================================================
 * 3. Stack frame khi exception xảy ra
 * ================================================================ */

/*
 * Khi Cortex-M vào exception, hardware tự động push lên stack:
 * xPSR, PC, LR, R12, R3, R2, R1, R0  (thứ tự từ cao xuống thấp)
 *
 * Exception frame (grows downward):
 * SP+0x1C: xPSR
 * SP+0x18: Return address (PC trước khi exception)
 * SP+0x14: LR
 * SP+0x10: R12
 * SP+0x0C: R3
 * SP+0x08: R2
 * SP+0x04: R1
 * SP+0x00: R0   <-- SP sau khi hardware push
 */
struct cortex_m_exception_frame {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;   /* địa chỉ instruction gây ra exception */
	uint32_t xpsr;
};

/* ================================================================
 * 4. Đọc MSP / PSP
 * ================================================================ */

static void print_stack_pointers(void)
{
	uint32_t msp = __get_MSP();
	uint32_t psp = __get_PSP();
	uint32_t control = __get_CONTROL();

	LOG_INF("MSP (Main Stack Pointer): 0x%08X", msp);
	LOG_INF("PSP (Process Stack Ptr) : 0x%08X", psp);
	LOG_INF("CONTROL register        : 0x%08X", control);
	LOG_INF("  SPSEL=%d (%s stack active)",
		(control >> 1) & 1,
		(control >> 1) & 1 ? "PSP" : "MSP");
	LOG_INF("  nPRIV=%d (%s mode)",
		control & 1,
		control & 1 ? "unprivileged" : "privileged");
}

/* ================================================================
 * 5. WFI — Wait For Interrupt (sleep instruction)
 * ================================================================ */

static void demo_wfi(void)
{
	LOG_INF("About to execute WFI...");
	/* Zephyr gọi WFI bên trong idle thread khi không có gì làm.
	 * Đây là cách CPU tiết kiệm điện mà không cần thay đổi clock. */
	__WFI();
	LOG_INF("Woken from WFI by SysTick interrupt");
}

int main(void)
{
	LOG_INF("=== ARM Cortex-M4 Internals Demo ===");
	print_core_info();
	print_nvic_info();
	print_stack_pointers();
	demo_wfi();
	return 0;
}
