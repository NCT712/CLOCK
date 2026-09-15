# 🕐 DỰ ÁN ĐỒNG HỒ SỐ AT89S52 (DIGITAL CLOCK PROJECT)

Dự án phát triển hệ thống **Đồng hồ số đa chức năng (Hiển thị Giờ:Phút, Chỉnh giờ thực tế, Hẹn giờ báo thức, Còi chip, Timeout)** trên vi điều khiển **Atmel AT89S52** (Kit AT89S52 V2 New).

Hệ thống được thiết kế theo chuẩn kiến trúc nhúng phân tầng chuyên nghiệp (**Strict Layered Architecture**), áp dụng mô hình lập trình bất đồng bộ **Cooperative Multitasking / Flag-based Super Loop** (chuyển giao và chuẩn hóa từ dự án mẫu ARM Cortex-M0 sang kiến trúc MCS-51).

---

## 📂 Cấu Trúc Toàn Bộ Không Gian Dự Án (Repository Structure)

```
dongho/
├── README.md                           # [BẠN ĐANG XEM] Tài liệu tổng quan gốc của repository
├── avrdude.conf                        # Cấu hình nạp avrdude (đã bổ sung vi điều khiển AT89S52/AT89S51)
│
├── at89s52_clock_app/                  # 👉 THƯ MỤC DỰ ÁN BÀI TẬP CHÍNH (AT89S52)
│   ├── README.md                       # Tài liệu chi tiết dự án, sơ đồ mạch, phân công 3 người
│   ├── Makefile                        # Makefile biên dịch SDCC và nạp code avrdude
│   ├── build.bat                       # Script thực thi 1-click build & link trên Windows
│   ├── .gitignore                      # Bộ lọc bỏ qua tệp tạm build của SDCC
│   └── Source/                         # Mã nguồn C khung xương (Skeleton) kèm gợi ý chi tiết
│       ├── Driver/                     # Tầng điều khiển phần cứng (GPIO, Timer 0 1ms)
│       ├── Module/                     # Tầng linh kiện ngoại vi (Segment 4 số, Button, Buzzer, Led)
│       ├── UserAPP/                    # Tầng ứng dụng nghiệp vụ (clock_app, main)
│       └── Readme/                     # Ghi chú kỹ thuật nhanh
│
├── MCU_Project/                        # 📚 DỰ ÁN THAM CHIẾU GỐC (ARM CORTEX-M0)
│   ├── README.md                       # Tài liệu kiến trúc mẫu của bài toán đồng hồ gốc
│   └── exp12_clock_app/                # Dự án mẫu trên Kit SN32F407 EVK (Keil MDK-ARM)
│
└── ref/                                # 📖 TÀI LIỆU KỸ THUẬT & ĐỀ BÀI THAM KHẢO
    ├── PRJ KIT AT89S52 V2 NEW 190815.pdf # Sơ đồ nguyên lý mạch phần cứng Kit AT89S52 V2
    ├── doc1919-1369059.pdf             # Datasheet vi điều khiển Atmel AT89S52
    └── MCU-Sonix-Bài-tập-vòng-1-Board-EVK-SN32F407_2026.pdf # Đề bài toán gốc
```

---

## 🎯 Mục Tiêu Bài Toán & Đối Sánh Phần Cứng

Dự án chuyển đổi bài toán thiết kế thiết bị đồng hồ số từ vi điều khiển ARM sang vi điều khiển 8-bit AT89S52 với các điều chỉnh phù hợp với phần cứng thực tế:

| Tính năng yêu cầu | Đề bài gốc (SN32F407) | Hiện thực trên Kit AT89S52 V2 |
| :--- | :--- | :--- |
| **Hiển thị thời gian** | 4 LED 7 đoạn (`HH.MM`) | 4 LED 7 đoạn Anode chung (Cathode qua `P1`, Quét Anode `P2.0 - P2.3` qua PNP A1015) |
| **Chỉnh giờ thực (SETUP)** | Nút SW3 | Nút **BT1** (`P3.5`): Nhấp nháy 2 số giờ $\rightarrow$ 2 số phút $\rightarrow$ Lưu |
| **Hẹn giờ (ALARM)** | Nút SW16 | Nút **BT3** (`P3.4`): Nhấp nháy 2 số giờ hẹn $\rightarrow$ phút hẹn $\rightarrow$ Lưu |
| **Nút Tăng (+)** | Nút SW6 | Nút **BT2** (`P3.2`): Tăng giờ ($0..23$) hoặc tăng phút ($0..59$) |
| **Nút Giảm (-)** | Nút SW10 | Nút **BT4** (`P3.3`): Giảm giờ ($23..0$) hoặc giảm phút ($59..0$) |
| **Còi báo (BUZZER)** | CT16B0 PWM0 $\rightarrow$ P3.0 | NPN C1815 $\rightarrow$ **P3.6**: Bíp ngắn 300ms khi ấn phím / Báo thức 5s (0.5s ON - 0.5s OFF) |
| **Đèn LED chỉ báo** | LED D6 nhấp nháy 1s | LED chỉ báo kết nối chân **P2.4** (Active LOW) |
| **Lưu trữ EEPROM** | Lưu qua I2C0 vào EEPROM | **Lược bỏ lưu ngoại:** Do trên Kit không gắn chip EEPROM ngoài và AT89S52 không có EEPROM nội; giờ hẹn lưu vào biến RAM tĩnh |
| **Tự động Timeout** | 30s không thao tác tự thoát | Đếm ngược 30 giây không ấn nút $\rightarrow$ Hủy cài đặt, thoát về bình thường và bíp 300ms |

---

## 🛠️ Bộ Công Cụ (Techstack)

* **Trình biên dịch C:** [SDCC (Small Device C Compiler)](https://sdcc.sourceforge.net/) $\ge 4.2.0$.
* **Trình chuyển định dạng:** `packihx` (đi kèm bộ cài đặt SDCC).
* **Trình nạp vi điều khiển:** `avrdude` kết hợp mạch nạp USBasp / USBISP.
* **Driver USBasp:** Cài đặt qua công cụ [Zadig](https://zadig.akeo.ie/) (chọn driver `libusb-win32` hoặc `WinUSB`).

---

## ⚡ Hướng Dẫn Khởi Động Nhanh (Quick Start)

### 1. Di chuyển vào thư mục dự án AT89S52
```powershell
cd at89s52_clock_app
```

### 2. Biên dịch mã nguồn
* **Cách 1 (Sử dụng Make):**
  ```bash
  make clean
  make
  ```
* **Cách 2 (Sử dụng build.bat 1-click trên Windows):**
  Nhấp đúp chuột vào file [`build.bat`](file:///C:/Users/84333/projects/dongho/at89s52_clock_app/build.bat) hoặc gõ `build.bat` trong cmd/powershell.

*File nhị phân Intel HEX sẽ được tạo tại:* `at89s52_clock_app/build/clock_app.hex`.

### 3. Nạp code vào Kit AT89S52 qua USBasp
```bash
make flash
```
*Hoặc chạy lệnh trực tiếp:*
```bash
avrdude -C ..\avrdude.conf -c usbasp -p 89s52 -U flash:w:build\clock_app.hex:i
```

---

## 👥 Tài Liệu Hướng Dẫn Thực Hiện Dự Án

1. **Tài liệu hướng dẫn chi tiết dự án:** Xem tại [at89s52_clock_app/README.md](file:///C:/Users/84333/projects/dongho/at89s52_clock_app/README.md).
   - Sơ đồ mạch, nguyên lý chống rung, triệt tiêu bóng mờ LED 7 đoạn.
   - Bảng phân chia công việc chi tiết cho nhóm **3 người**.
   - Hướng dẫn cài đặt Zadig driver và xử lý toàn bộ các mã lỗi thường gặp khi nạp chip.
2. **Khung code bài tập (Lab Skeleton):** Toàn bộ các file trong thư mục `at89s52_clock_app/Source/` đã được thiết kế sẵn vỏ hàm (stub), chú thích input/output và gợi ý giải thuật từng bước để người thực hiện dễ dàng hoàn thiện từng hàm.
