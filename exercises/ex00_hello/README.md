# ex00 - Hello Zephyr

Bài tập đầu tiên: in "Hello, Zephyr!" ra console bằng `printk`.

## Môi trường

| | Path |
|---|---|
| Zephyr base | `/home/thule/zephyrproject/zephyr` (v4.4.99) |
| Zephyr SDK | `/home/thule/zephyr-sdk-1.0.1` (chỉ có toolchain ARM) |
| Python venv | `/home/thule/zephyrproject/.venv` |

## Build & chạy trên QEMU

```bash
source /home/thule/zephyrproject/.venv/bin/activate
export ZEPHYR_BASE=/home/thule/rtos-zephyr-workspaces/zephyr

cd zephyr-practice/exercises/ex00_hello

west build -b qemu_cortex_m3 .

# Chạy trên QEMU (Ctrl+A rồi X để thoát)
west build -t run
```

Đổi board (thêm `-p` để xóa build cũ):

```bash
west build -p -b nrf52840dk/nrf52840 .
```

> **Lưu ý:** SDK chỉ cài toolchain `arm-zephyr-eabi` nên board phải là ARM.

## Flash lên board thật

```bash
west flash
```

## Kết quả mong đợi

```
*** Booting Zephyr OS build v4.4.0 ***
Hello, Zephyr!
```

## Giải thích code

| File | Mục đích |
|------|----------|
| `src/main.c` | Gọi `printk()` để in chuỗi ra UART/console |
| `CMakeLists.txt` | Tìm Zephyr qua `$ZEPHYR_BASE` và link source vào target `app` |
| `prj.conf` | Bật `CONFIG_PRINTK` (kernel print support) |
