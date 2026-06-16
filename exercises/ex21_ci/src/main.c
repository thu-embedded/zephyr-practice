/*
 * ex21: Bài này KHÔNG có code chạy trên board.
 * Tập trung vào tooling: west, twister, GitHub Actions.
 * Xem README.md để thực hành.
 */
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

/* Test đơn giản để twister có thể chạy */
ZTEST_SUITE(ci_demo, NULL, NULL, NULL, NULL, NULL);

ZTEST(ci_demo, test_kernel_version)
{
	zassert_true(KERNEL_VERSION_MAJOR >= 4, "Expected Zephyr 4.x");
}

ZTEST(ci_demo, test_uptime_increases)
{
	int64_t t0 = k_uptime_get();
	k_sleep(K_MSEC(10));
	int64_t t1 = k_uptime_get();
	zassert_true(t1 > t0, "Uptime should increase");
}
