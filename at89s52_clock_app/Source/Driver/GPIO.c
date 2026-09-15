/**
 * @file GPIO.c
 * @brief Khởi tạo và điều khiển GPIO trên Kit AT89S52 V2.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "GPIO.h"

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Thiết lập trạng thái ban đầu an toàn cho toàn bộ các chân GPIO khi khởi động.
 *
 * @param None
 * @return None
 *
 * YÊU CẦU THỰC HIỆN:
 * - Tra cứu sơ đồ nguyên lý Kit AT89S52 V2 để xác định mức logic tích cực của các ngoại vi:
 *   1. Tắt toàn bộ 8 đoạn của LED 7 đoạn (Port 1 - Anode chung).
 *   2. Khóa 4 transistor PNP chọn vị trí Digit (P2.0 - P2.3) để ngắt nguồn hiển thị.
 *   3. Tắt còi Buzzer (P3.6) và tắt LED chỉ báo hẹn giờ (P2.4).
 * 
 *   4. Thiết lập các chân nút nhấn (P3.2 - P3.5) về trạng thái sẵn sàng đọc ngõ vào.
 */
void GPIO_Init(void)
{
    /* 1. Tắt toàn bộ 8 đoạn của LED 7 đoạn (Port 1 - Anode chung nên tắt bằng mức 1) */
    SEGMENT_DATA_PORT = 0xFF;

    /* 2. Khóa 4 transistor PNP (P2.0 - P2.3) để ngắt nguồn hiển thị (tắt bằng mức 1) */
    PIN_DIGIT_1 = DIGIT_DISABLE;
    PIN_DIGIT_2 = DIGIT_DISABLE;
    PIN_DIGIT_3 = DIGIT_DISABLE;
    PIN_DIGIT_4 = DIGIT_DISABLE;

    /* 3. Tắt còi Buzzer (P3.6) và tắt LED chỉ báo hẹn giờ (P2.4) */
    PIN_BUZZER = BUZZER_OFF;
    PIN_LED_ALARM = LED_ALARM_OFF;

    /* 4. Thiết lập các chân nút nhấn (P3.2 - P3.5) về mức 1 để đọc ngõ vào */
    PIN_BTN_SETUP = 1;
    PIN_BTN_ALARM = 1;
    PIN_BTN_UP = 1;
    PIN_BTN_DOWN = 1;
}
