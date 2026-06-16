#include <zephyr/kernel.h>

int main(void)
{
	while (1) {
		printk("Hello, Zephyr!\n");
		k_sleep(K_SECONDS(5));
	}
	
	return 0;
}
