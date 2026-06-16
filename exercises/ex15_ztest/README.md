# ex15 - Ztest Framework

Unit test một ring buffer đơn giản. Chạy trên QEMU (không cần board thật)
hoặc trên board thật qua UART.

## Build & Test trên QEMU

```bash
west build -b qemu_cortex_m3 .
west build -t run
```

## Build & Test trên board thật

```bash
west build -b weact_stm32f405_core .
west flash
# Xem output qua minicom /dev/ttyACM0
```

## Output mong đợi

```
Running TESTSUITE ring_buffer_tests
===================================================================
START - test_init
 PASS - test_init in 0ms
START - test_put_get_single
 PASS - test_put_get_single in 0ms
...
===================================================================
TESTSUITE ring_buffer_tests succeeded
```

## Câu hỏi kiểm tra

### Ztest Basics

**Q1.** `ZTEST_SUITE(ring_buffer_tests, NULL, NULL, NULL, NULL, NULL)` — 6 tham số.
Tham số 2 đến 6 là gì? (predict, setup, teardown, ...) Khi nào nên dùng `setup`?

**Q2.** `ZTEST(ring_buffer_tests, test_init)` đăng ký test vào section nào trong binary?
Ztest runner tìm và chạy tests thế nào — có cần khai báo hàm `main()` không?

**Q3.** Sự khác nhau giữa:
- `zassert_equal(a, b, msg)`
- `zassert_ok(ret, msg)`
- `zassert_not_null(ptr, msg)`
- `zassert_true(condition, msg)`

Khi một assert fail, test tiếp tục chạy không?

### Test Design

**Q4.** `test_wrap_around` là test quan trọng nhất — tại sao? Loại bug nào nó phát hiện
mà các test đơn giản hơn bỏ qua?

**Q5.** Hiện tại `ring_buf` là static variable dùng chung giữa các test.
Vấn đề gì có thể xảy ra nếu test trước fail ở giữa chừng? Sửa bằng cách dùng
`setup` function thế nào?

**Q6.** Thêm test case để kiểm tra `ring_buf_put()` với `byte = 0x00` và `byte = 0xFF`.
Đây là loại test gì (boundary testing)?

### Chạy trên Native POSIX

**Q7.** `west build -b native_sim .` cho phép chạy test như một Linux process.
Ưu điểm so với chạy trên QEMU hoặc board thật là gì?
Khi nào **phải** chạy trên hardware thật?

**Q8.** Ztest có hỗ trợ **mock** không? Nếu `ring_buf_put()` bên trong gọi
một hardware register, làm thế nào để test mà không cần hardware?

### CI/CD

**Q9.** Làm thế nào để tích hợp Ztest vào CI pipeline (GitHub Actions)?
Lệnh nào để chạy test tự động và parse kết quả pass/fail?

**Q10.** Ngoài Ztest, Zephyr còn hỗ trợ test framework nào?
`twister` là gì và nó liên quan đến Ztest thế nào?

### Coverage

**Q11.** Ring buffer của chúng ta có bao nhiêu % code coverage với 6 test case hiện tại?
Có line/branch nào chưa được cover? Thêm test case nào để đạt 100% branch coverage?
