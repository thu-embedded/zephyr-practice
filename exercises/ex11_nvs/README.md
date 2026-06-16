# ex11 - NVS (Non-Volatile Storage)

Lưu boot counter, tên thiết bị và config vào internal Flash.
Data tồn tại sau khi reset/power off.

## Build & Flash

```bash
west build -b weact_stm32f405_core .
west flash
```

Reset board nhiều lần — boot counter tăng dần.

## Câu hỏi kiểm tra

### NVS Internals

**Q1.** NVS (Non-Volatile Storage) của Zephyr hoạt động theo nguyên lý nào?
Tại sao không thể ghi đè trực tiếp lên Flash mà cần thuật toán đặc biệt?

**Q2.** `nvs.sector_count = 3` — NVS dùng 3 sector flash. Tại sao cần ít nhất 2 sector?
Sector thứ 3 dùng để làm gì?

**Q3.** Mỗi lần `nvs_write()` với cùng một ID, NVS **không xóa entry cũ** ngay.
Data cũ ở đâu? Khi nào nó mới thực sự được xóa?

**Q4.** Sau nhiều lần write, flash sector đầy. NVS làm gì lúc này?
Giải thích quá trình **garbage collection**.

### Flash Map & Partition

**Q5.** `FIXED_PARTITION_DEVICE(storage_partition)` và `FIXED_PARTITION_OFFSET` — 
`storage_partition` được định nghĩa ở đâu? Làm thế nào để xem partition map
của board hiện tại?

**Q6.** STM32F405 có Flash 1MB, chia thành sectors kích thước không đều
(16KB, 64KB, 128KB). Điều này ảnh hưởng thế nào đến việc chọn `sector_size`?

**Q7.** Tại sao lưu float `25.50°C` dưới dạng `int32_t threshold = 2550`
thay vì dùng `float` trực tiếp? Ở những nền tảng nào điều này đặc biệt quan trọng?

### Endurance & Wear Leveling

**Q8.** Flash STM32F405 chịu được bao nhiêu lần erase? Nếu boot counter tăng
mỗi 1 giây (hệ thống luôn hoạt động), sau bao lâu Flash có thể bị hỏng?
NVS có wear leveling không?

**Q9.** `nvs_read()` trả về `ssize_t`. Khi nào nó trả về giá trị âm?
Giá trị dương có ý nghĩa gì?

### Alternatives

**Q10.** So sánh các cách lưu data persistent trong Zephyr:

| | NVS | Settings subsystem | FAT/LittleFS |
|--|-----|-------------------|-------------|
| API | key-value (ID int) | ? | ? |
| Phù hợp với | config nhỏ | ? | ? |
| Cần hardware | internal flash | ? | ? |
