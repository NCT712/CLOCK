/**
 * @file Segment.h
 * @brief Module điều khiển và quét hiển thị LED 7 đoạn 4 số (Common Anode).
 * @author Team Clock AT89S52
 * @date 2026
 */

#ifndef __SEGMENT_H
#define __SEGMENT_H

#include <stdint.h>

/* Mã hiển thị tắt (tất cả các đoạn đều tắt) */
#define SEG_BLANK           0xFF

/* Mặt nạ bật dấu chấm DP (Bit 7 mức 0) */
#define SEG_DP_MASK         0x7F

/* Buffer chứa mã hiển thị của 4 LED 7 đoạn:
 * segment_buff[0]: Digit 1 (Chục giờ - HH_H)
 * segment_buff[1]: Digit 2 (Đơn vị giờ - HH_L, có dấu chấm ngăn cách)
 * segment_buff[2]: Digit 3 (Chục phút - MM_H)
 * segment_buff[3]: Digit 4 (Đơn vị phút - MM_L)
 */
extern uint8_t segment_buff[4];

/* Bảng tra mã LED 7 đoạn Anode chung (0 - 9) */
extern const uint8_t SEGMENT_TABLE[10];

/**
 * @brief Khởi tạo module hiển thị LED 7 đoạn.
 *
 * @param None
 * @return None
 */
void Digital_Init(void);

/**
 * @brief Cập nhật dữ liệu giờ và phút vào bộ đệm segment_buff theo định dạng HH.MM.
 *
 * @param[in] hour Giờ (0 - 23).
 * @param[in] minute Phút (0 - 59).
 * @param[in] dot_on Trạng thái dấu chấm ngăn cách giữa giờ và phút (1 = Bật, 0 = Tắt).
 * @return None
 */
void Digital_Display_Clock(uint8_t hour, uint8_t minute, uint8_t dot_on);

/**
 * @brief Hiển thị số nguyên thập phân 0 - 9999 lên 4 LED 7 đoạn.
 *
 * @param[in] dec Giá trị số cần hiển thị (0 - 9999).
 * @return None
 */
void Digital_DisplayDEC(uint16_t dec);

/**
 * @brief Tắt hiển thị một vị trí LED (dùng cho hiệu ứng chớp tắt khi SETUP).
 *
 * @param[in] digit_idx Chỉ số vị trí LED (0..3).
 * @return None
 */
void Digital_Set_Blank(uint8_t digit_idx);

/**
 * @brief Hàm quét đa công (Time-multiplexing) hiển thị 4 LED 7 đoạn.
 *
 * @details
 * - Hàm này được gọi định kỳ (ví dụ mỗi 1ms hoặc 2ms trong hàm ClockApp_Task_1ms).
 * - Mỗi lần gọi sẽ bật lần lượt 1 trong 4 LED để mắt người nhìn thấy như sáng liên tục.
 * - Triệt tiêu hiện tượng bóng mờ (ghosting) bằng kỹ thuật "blanking before change".
 *
 * @param None
 * @return None
 */
void Digital_Scan(void);

#endif /* __SEGMENT_H */
