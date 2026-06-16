# ex21 - Git + West + Twister + CI/CD

Professional workflow cho Zephyr project: quản lý dependency với west,
chạy test tự động với twister, tích hợp GitHub Actions.

Liên quan: ex15 (Ztest — viết test), tất cả các bài trước (build được CI).

## Thực hành ngay

```bash
# Chạy test trên QEMU không cần board
cd /home/thule/rtos-zephyr-workspaces

# Một test cụ thể
west build -b qemu_cortex_m3 zephyr-practice/exercises/ex21_ci
west build -t run

# Toàn bộ exercises có testcase.yaml dùng twister
./zephyr/scripts/twister \
    -T zephyr-practice/exercises/ex21_ci \
    -p qemu_cortex_m3 native_sim \
    --inline-logs
```

## Câu hỏi kiểm tra

### West Manifest

**Q1.** `west.yml` của project này:
```yaml
projects:
  - name: zephyr
    url: https://github.com/zephyrproject-rtos/zephyr
    path: zephyr
    west-commands: scripts/west-commands.yml
```
`west update` làm gì chính xác? Nó dùng git hay pip?
Nếu team có 5 người, làm thế nào đảm bảo ai cũng dùng cùng version Zephyr?

**Q2.** `revision: v3.7.0` vs `revision: main`:
- Khi nào nên pin version? Khi nào theo main?
- Nếu Zephyr release fix một bug quan trọng sau v3.7.0, bạn update thế nào
  mà không làm hỏng những gì đang hoạt động?

**Q3.** West workspace có thể có nhiều project ngoài Zephyr (ví dụ: HAL, modules).
Nếu bạn có library nội bộ `my-drivers` muốn thêm vào west.yml, cần thêm gì?
Làm thế nào để west tìm Kconfig và CMake modules của nó?

### Twister

**Q4.** `testcase.yaml` định nghĩa test scenario. Giải thích:
```yaml
platform_allow:
  - qemu_cortex_m3
  - native_sim
harness: ztest
```
`platform_allow` và `platform_exclude` dùng khi nào?
`harness: ztest` nghĩa là twister biết parse output thế nào?

**Q5.** Chạy twister với `-p qemu_cortex_m3 -p native_sim` cùng lúc.
Hai platform này chạy song song hay tuần tự? Twister dùng bao nhiêu CPU core?
Flag nào để giới hạn số job chạy song song?

**Q6.** Twister output `twister-out/` chứa gì? File `testplan.json` và
`twister.json` dùng để làm gì? CI pipeline đọc chúng thế nào để biết pass/fail?

**Q7.** Một số test cần hardware thật (ex10 I2C sensor). Twister có thể
điều khiển hardware thật qua **hardware map** không? Cần setup gì?

### GitHub Actions

**Q8.** Trong `.github/workflows/ci.yml`:
```yaml
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]
```
Workflow chạy khi nào? Làm thế nào để chỉ chạy khi file trong
`exercises/` thay đổi (không chạy khi chỉ sửa README)?

**Q9.** Bước "Install Zephyr SDK" download ~500MB mỗi lần CI chạy.
Làm thế nào để **cache** SDK giữa các runs?
```yaml
- uses: actions/cache@v4
  with:
    path: zephyr-sdk-*
    key: zephyr-sdk-${{ hashFiles('zephyr/SDK_VERSION') }}
```
`hashFiles()` đảm bảo điều gì?

**Q10.** CI pipeline fail khi twister báo test FAILED. Nhưng trong thực tế,
có những test "flaky" (đôi khi pass, đôi khi fail do timing).
Twister có option retry không? Làm thế nào handle flaky tests đúng cách?

### Git Workflow cho Embedded

**Q11.** West dùng git để quản lý Zephyr. Nếu cần **patch** Zephyr để fix bug
chưa được upstream merge, có 3 cách:
1. `git cherry-pick` commit vào local branch
2. `west.yml` point đến fork của Zephyr
3. Zephyr **module** override

Mỗi cách có ưu/nhược điểm gì? Khi nào dùng cách nào?

**Q12.** Bạn đang phát triển driver mới cho sensor XYZ. Muốn contribute
ngược lại vào Zephyr upstream. Quy trình là gì?
(Hint: Zephyr dùng GitHub PR + CI + maintainer review + DCO sign-off)

### Kết hợp tất cả

**Q13.** Design một CI pipeline hoàn chỉnh cho team 5 người develop firmware
Zephyr cho sản phẩm IoT. Pipeline cần:
- Build cho board thật (weact_stm32f405_core)
- Chạy unit tests trên QEMU
- Static analysis (clang-tidy hoặc sparse)
- Check coding style (checkpatch)
- Generate firmware artifact (.hex, .bin)
- Notify Slack khi fail

Vẽ sơ đồ và giải thích mỗi bước.
