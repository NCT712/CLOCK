/**
 * @file GPIO.h
 * @brief Định nghĩa và cấu hình phần cứng chân I/O trên Kit AT89S52 V2.
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @note Sơ đồ chân căn cứ theo schematic: PRJ KIT AT89S52 V2 NEW 190815.pdf
 * - Port 1 (P1.0 - P1.7): Điều khiển dữ liệu các thanh LED 7 đoạn (Cathode A..DP qua RP3, RP4).
 *   Mức tích cực: Mức 0 (LOW) = Sáng đoạn, Mức 1 (HIGH) = Tắt đoạn.
 * - Port 2 (P2.0 - P2.3): Điều khiển chọn LED 4 số (Anode chung qua transistor PNP A1015 Q1..Q4).
 *   Mức tích cực: Mức 0 (LOW) = Bật transistor PNP (cấp VCC cho LED), Mức 1 (HIGH) = Tắt.
 * - Port 2 (P2.4): LED chỉ báo chế độ hẹn giờ (Alarm LED).
 * - Port 3 (P3.2 - P3.5): 4 nút nhấn độc lập BT1, BT2, BT3, BT4 (kéo lên VCC qua trở R2..R5).
 *   Mức tích cực: Nhả = Mức 1 (HIGH), Nhấn = Mức 0 (LOW).
 * - Port 3 (P3.6): Điều khiển Còi (Buzzer qua transistor NPN C1815 Q6).
 *   Mức tích cực: Mức 1 (HIGH) = Kêu, Mức 0 (LOW) = Tắt.
 */

#ifndef __GPIO_H
#define __GPIO_H

#include <at89x52.h>
#include <stdint.h>

/*============================================================================
 * 1. ĐỊNH NGHĨA CHÂN ĐIỀU KHIỂN LED 7 ĐOẠN (PORT 1 & PORT 2)
 *===========================================================================*/
#define SEGMENT_DATA_PORT       P1          /**< Port xuất dữ liệu 8 thanh (A..DP) */

#define DIGIT_PORT              P2          /**< Port quét 4 vị trí hiển thị */
#define PIN_DIGIT_1             P2_3        /**< Digit 1: Chục Giờ (HH_H) - Q1 PNP */
#define PIN_DIGIT_2             P2_2        /**< Digit 2: Đơn vị Giờ (HH_L) - Q2 PNP */
#define PIN_DIGIT_3             P2_1        /**< Digit 3: Chục Phút (MM_H) - Q3 PNP */
#define PIN_DIGIT_4             P2_0        /**< Digit 4: Đơn vị Phút (MM_L) - Q4 PNP */

/* Mức điều khiển chọn số (PNP A1015: 0 = BẬT, 1 = TẮT) */
#define DIGIT_ENABLE            0
#define DIGIT_DISABLE           1

/*============================================================================
 * 2. ĐỊNH NGHĨA CHÂN NÚT NHẤN (PORT 3)
 *===========================================================================*/
#define PIN_BTN_SETUP           P3_5        /**< BT1: Nút SETUP (Chỉnh giờ) */
#define PIN_BTN_ALARM           P3_4        /**< BT3: Nút ALARM (Hẹn giờ) */
#define PIN_BTN_UP              P3_2        /**< BT2: Nút UP (Tăng +) */
#define PIN_BTN_DOWN            P3_3        /**< BT4: Nút DOWN (Giảm -) */

#define BTN_PRESSED             0           /**< Trạng thái nút được bấm (Active LOW) */
#define BTN_RELEASED            1           /**< Trạng thái nút được nhả */

/*============================================================================
 * 3. ĐỊNH NGHĨA CHÂN CÒI VÀ ĐÈN BÁO
 *===========================================================================*/
#define PIN_BUZZER              P3_6        /**< Còi chíp (NPN C1815: 1 = KÊU, 0 = TẮT) */
#define BUZZER_ON               1
#define BUZZER_OFF              0

#define PIN_LED_ALARM           P2_4        /**< LED báo trạng thái Alarm (Active LOW) */
#define LED_ALARM_ON            0
#define LED_ALARM_OFF           1

/*============================================================================
 * 4. HÀM KHỞI TẠO VÀ ĐIỀU KHIỂN GPIO
 *===========================================================================*/

/**
 * @brief Khởi tạo trạng thái ban đầu cho toàn bộ các chân I/O trên kit.
 *
 * @details
 * - Tắt còi (P3_6 = 0).
 * - Tắt toàn bộ 4 Digit LED 7 đoạn (P2.0 - P2.3 = 1).
 * - Tắt dữ liệu LED 7 đoạn (P1 = 0xFF).
 * - Đặt các chân nút nhấn về mức 1 (P3.2 - P3.5 = 1) để sẵn sàng đọc ngõ vào.
 * - Tắt LED báo hẹn giờ (P2_4 = 1).
 *
 * @param None
 * @return None
 */
void GPIO_Init(void);

#endif /* __GPIO_H */
