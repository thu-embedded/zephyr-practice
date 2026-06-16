#include <zephyr/ztest.h>

/* ---- Module under test: simple ring buffer ---- */

#define RING_BUF_SIZE 8

struct ring_buf {
	uint8_t  data[RING_BUF_SIZE];
	uint32_t head;
	uint32_t tail;
	uint32_t count;
};

static void ring_buf_init(struct ring_buf *rb)
{
	rb->head = rb->tail = rb->count = 0;
}

static int ring_buf_put(struct ring_buf *rb, uint8_t byte)
{
	if (rb->count == RING_BUF_SIZE) {
		return -ENOMEM;
	}
	rb->data[rb->tail] = byte;
	rb->tail = (rb->tail + 1) % RING_BUF_SIZE;
	rb->count++;
	return 0;
}

static int ring_buf_get(struct ring_buf *rb, uint8_t *byte)
{
	if (rb->count == 0) {
		return -ENODATA;
	}
	*byte = rb->data[rb->head];
	rb->head = (rb->head + 1) % RING_BUF_SIZE;
	rb->count--;
	return 0;
}

/* ---- Test suite ---- */

static struct ring_buf rb;

ZTEST_SUITE(ring_buffer_tests, NULL, NULL, NULL, NULL, NULL);

ZTEST(ring_buffer_tests, test_init)
{
	ring_buf_init(&rb);
	zassert_equal(rb.count, 0, "Initial count should be 0");
	zassert_equal(rb.head, 0, "Initial head should be 0");
	zassert_equal(rb.tail, 0, "Initial tail should be 0");
}

ZTEST(ring_buffer_tests, test_put_get_single)
{
	uint8_t out;

	ring_buf_init(&rb);
	zassert_ok(ring_buf_put(&rb, 0xAB), "Put should succeed");
	zassert_equal(rb.count, 1, "Count should be 1 after put");

	zassert_ok(ring_buf_get(&rb, &out), "Get should succeed");
	zassert_equal(out, 0xAB, "Got wrong value");
	zassert_equal(rb.count, 0, "Count should be 0 after get");
}

ZTEST(ring_buffer_tests, test_fifo_order)
{
	uint8_t out;

	ring_buf_init(&rb);
	ring_buf_put(&rb, 1);
	ring_buf_put(&rb, 2);
	ring_buf_put(&rb, 3);

	ring_buf_get(&rb, &out);
	zassert_equal(out, 1, "Expected FIFO order");
	ring_buf_get(&rb, &out);
	zassert_equal(out, 2, "Expected FIFO order");
	ring_buf_get(&rb, &out);
	zassert_equal(out, 3, "Expected FIFO order");
}

ZTEST(ring_buffer_tests, test_full)
{
	ring_buf_init(&rb);

	for (int i = 0; i < RING_BUF_SIZE; i++) {
		zassert_ok(ring_buf_put(&rb, i), "Put #%d should succeed", i);
	}

	int ret = ring_buf_put(&rb, 0xFF);
	zassert_equal(ret, -ENOMEM, "Should fail when full");
	zassert_equal(rb.count, RING_BUF_SIZE, "Count should equal size");
}

ZTEST(ring_buffer_tests, test_empty)
{
	uint8_t out;

	ring_buf_init(&rb);
	int ret = ring_buf_get(&rb, &out);
	zassert_equal(ret, -ENODATA, "Should fail when empty");
}

ZTEST(ring_buffer_tests, test_wrap_around)
{
	uint8_t out;

	ring_buf_init(&rb);

	/* Fill half */
	for (int i = 0; i < 4; i++) {
		ring_buf_put(&rb, i);
	}
	/* Drain half — head now at 4 */
	for (int i = 0; i < 4; i++) {
		ring_buf_get(&rb, &out);
	}
	/* Fill again — tail wraps around */
	for (int i = 10; i < 18; i++) {
		zassert_ok(ring_buf_put(&rb, i), "Wrap-around put #%d", i);
	}
	/* Verify */
	for (int i = 10; i < 18; i++) {
		ring_buf_get(&rb, &out);
		zassert_equal(out, i, "Wrap-around value mismatch");
	}
}
