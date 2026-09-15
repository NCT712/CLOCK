/**
 * @file Segment.c
 * @brief Quét LED 7 đoạn 4 số Anode chung trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "Segment.h"
#include "../Driver/GPIO.h"

/*_____ V A R I A B L E S __________________________________________________*/

/* Bộ đệm 4 byte mã hiển thị cho 4 vị trí LED:
 * segment_buff[0]: Digit 1 (Chục Giờ - HH_H)
 * segment_buff[1]: Digit 2 (Đơn vị Giờ - HH_L, có dấu chấm DP)
 * segment_buff[2]: Digit 3 (Chục Phút - MM_H)
 * segment_buff[3]: Digit 4 (Đơn vị Phút - MM_L)
 */
uint8_t segment_buff[4] = {SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK};

/* Bảng tra mã LED 7 đoạn Anode chung (Cathode mức 0 = Sáng, mức 1 = Tắt)
 * Bit 0: A, Bit 1: B, Bit 2: C, Bit 3: D, Bit 4: E, Bit 5: F, Bit 6: G, Bit 7: DP
 */
const uint8_t SEGMENT_TABLE[10] = {
    0xC0,  /* '0' */
    0xF9,  /* '1' */
    0xA4,  /* '2' */
    0xB0,  /* '3' */
    0x99,  /* '4' */
    0x92,  /* '5' */
    0x82,  /* '6' */
    0xF8,  /* '7' */
    0x80,  /* '8' */
    0x90   /* '9' */
};

/* Chỉ số LED hiện tại đang được quét (0..3) */
static uint8_t s_current_digit = 0;

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Khởi tạo trạng thái ban đầu cho bộ đệm hiển thị LED 7 đoạn.
 *
 * YÊU CẦU:
 * - Nạp dữ liệu ban đầu "00.00" vào bộ đệm segment_buff.
 * - Đặt chỉ số quét s_current_digit về vị trí bắt đầu.
 */
void Digital_Init(void)
{
    segment_buff[0] = SEGMENT_TABLE[0];
    segment_buff[1] = SEGMENT_TABLE[0] & SEG_DP_MASK; /* Sáng dấu chấm mặc định */
    segment_buff[2] = SEGMENT_TABLE[0];
    segment_buff[3] = SEGMENT_TABLE[0];
    s_current_digit = 0;
}

/**
 * @brief Cập nhật giờ và phút vào bộ đệm segment_buff theo chuẩn HH.MM.
 *
 * @param[in] hour Giờ (0 - 23).
 * @param[in] minute Phút (0 - 59).
 * @param[in] dot_on Trạng thái dấu chấm ngăn cách giữa giờ và phút (1 = Bật, 0 = Tắt).
 *
 * YÊU CẦU:
 * 1. Tách các chữ số hàng chục và hàng đơn vị của `hour` và `minute`.
 * 2. Tra bảng SEGMENT_TABLE để lấy mã hiển thị và nạp vào 4 vị trí trong segment_buff.
 * 3. Nếu dot_on = 1, bật thêm đoạn DP ở vị trí Digit 2 (đơn vị giờ).
 */
void Digital_Display_Clock(uint8_t hour, uint8_t minute, uint8_t dot_on)
{
    segment_buff[0] = SEGMENT_TABLE[hour / 10];
    segment_buff[1] = SEGMENT_TABLE[hour % 10];
    if (dot_on) {
        segment_buff[1] &= SEG_DP_MASK;
    }
    segment_buff[2] = SEGMENT_TABLE[minute / 10];
    segment_buff[3] = SEGMENT_TABLE[minute % 10];
}

/**
 * @brief Hiển thị số nguyên thập phân (0 - 9999) lên 4 LED 7 đoạn.
 *
 * @param[in] dec Giá trị số nguyên cần hiển thị (0 - 9999).
 *
 * YÊU CẦU:
 * - Tách giá trị dec thành 4 chữ số (nghìn, trăm, chục, đơn vị) và nạp mã tương ứng vào segment_buff.
 */
void Digital_DisplayDEC(uint16_t dec)
{
    if (dec > 9999) dec = 9999;
    segment_buff[0] = SEGMENT_TABLE[dec / 1000];
    segment_buff[1] = SEGMENT_TABLE[(dec / 100) % 10];
    segment_buff[2] = SEGMENT_TABLE[(dec / 10) % 10];
    segment_buff[3] = SEGMENT_TABLE[dec % 10];
}

/**
 * @brief Xóa trắng 1 vị trí LED (tắt hoàn toàn) phục vụ hiệu ứng nhấp nháy.
 *
 * @param[in] digit_idx Vị trí LED cần tắt (0..3).
 *
 * YÊU CẦU:
 * - Gán mã SEG_BLANK vào vị trí digit_idx hợp lệ trong segment_buff.
 */
void Digital_Set_Blank(uint8_t digit_idx)
{
    if (digit_idx < 4) {
        segment_buff[digit_idx] = SEG_BLANK;
    }
}

/**
 * @brief Quét đa công (Time-multiplexing) hiển thị 4 LED 7 đoạn.
 *
 * @details
 * Hàm được gọi định kỳ mỗi 1ms, mỗi lần quét 1 vị trí LED.
 *
 * YÊU CẦU:
 * - Áp dụng nguyên lý quét đa công theo chu kỳ 4 bước (Digit 1 -> Digit 2 -> Digit 3 -> Digit 4).
 * - Chú ý xử lý triệt tiêu hiện tượng bóng mờ (ghosting): cần ngắt nguồn LED trước khi
 *   thay đổi dữ liệu trên bus đoạn Port 1, sau đó mới cấp nguồn cho LED kế tiếp.
 */
void Digital_Scan(void)
{
    /* 1. Blanking: Ngắt nguồn toàn bộ 4 LED trước khi xuất dữ liệu mới để triệt tiêu bóng mờ */
    PIN_DIGIT_1 = DIGIT_DISABLE;
    PIN_DIGIT_2 = DIGIT_DISABLE;
    PIN_DIGIT_3 = DIGIT_DISABLE;
    PIN_DIGIT_4 = DIGIT_DISABLE;
    
    /* 2. Xuất dữ liệu lên Bus (Port 1) */
    SEGMENT_DATA_PORT = segment_buff[s_current_digit];
    
    /* 3. Cấp nguồn cho LED tương ứng */
    switch (s_current_digit) {
        case 0: PIN_DIGIT_1 = DIGIT_ENABLE; break;
        case 1: PIN_DIGIT_2 = DIGIT_ENABLE; break;
        case 2: PIN_DIGIT_3 = DIGIT_ENABLE; break;
        case 3: PIN_DIGIT_4 = DIGIT_ENABLE; break;
        default: break;
    }
    
    /* 4. Tăng chỉ số quét cho lần tiếp theo */
    s_current_digit++;
    if (s_current_digit >= 4) {
        s_current_digit = 0;
    }
}
