/**
 * @file Timer0.c
 * @brief Cấu hình và lập trình ngắt định thời phần cứng Timer 0 trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include <at89x52.h>
#include "Timer0.h"

/*_____ V A R I A B L E S __________________________________________________*/
volatile uint8_t timer_1ms_flag = 0;  /**< Cờ báo nhịp 1ms, set bởi ISR, xóa bởi Main loop */
volatile uint8_t timer_1s_flag  = 0;  /**< Cờ báo nhịp 1s, set bởi ISR mỗi 1000ms */

static uint16_t s_ms_counter = 0;     /**< Bộ đếm tích lũy miligiây */

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Khởi tạo Timer 0 phát sinh ngắt định thời chu kỳ 1ms.
 *
 * @param None
 * @return None
 *
 * YÊU CẦU THỰC HIỆN:
 * 1. Cấu hình thanh ghi TMOD cho Timer 0 chạy ở Mode 1 (16-bit Timer định thời nội).
 * 2. Tính toán giá trị nạp ban đầu (Reload) cho TH0 và TL0 dựa trên tần số thạch anh 11.0592 MHz
 *    sao cho chu kỳ tràn ngắt đúng 1ms.
 * 3. Cho phép ngắt Timer 0, cho phép ngắt toàn cục và kích hoạt bộ đếm Timer 0 hoạt động.
 */
void Timer0_Init(void)
{
    /* Cấu hình Timer 0 Mode 1 (16-bit timer) */
    TMOD = (TMOD & 0xF0) | 0x01;
    
    /* Nạp giá trị đếm cho ngắt 1ms (thạch anh 11.0592 MHz) */
    TH0 = TIMER0_TH0_1MS;
    TL0 = TIMER0_TL0_1MS;
    
    /* Cho phép ngắt và kích hoạt Timer */
    ET0 = 1;    /* Bật ngắt Timer 0 */
    EA = 1;     /* Cho phép ngắt toàn cục */
    TR0 = 1;    /* Khởi động đếm Timer 0 */
}

/**
 * @brief Trình phục vụ ngắt Timer 0 (Timer 0 ISR) - Vector ngắt số 1.
 *
 * @param None
 * @return None
 *
 * YÊU CẦU THỰC HIỆN:
 * 1. Nạp lại giá trị đếm cho TH0 và TL0 để duy trì chu kỳ ngắt 1ms cho lần kế tiếp.
 * 2. Bật cờ timer_1ms_flag = 1.
 * 3. Đếm tích lũy đủ 1000 lần (1000ms = 1s) thì bật cờ timer_1s_flag = 1 và reset bộ đếm tích lũy.
 *
 * Lưu ý: Giữ ISR ngắn gọn, không thực thi các tác vụ tốn thời gian trong ngắt.
 */
void Timer0_ISR(void) __interrupt(1)
{
    /* Nạp lại giá trị đếm cho 1ms tiếp theo */
    TH0 = TIMER0_TH0_1MS;
    TL0 = TIMER0_TL0_1MS;
    
    /* Set cờ 1ms để Task ở Super Loop xử lý */
    timer_1ms_flag = 1;
    
    /* Đếm tích lũy 1000ms để sinh cờ 1s */
    s_ms_counter++;
    if (s_ms_counter >= 1000) {
        timer_1s_flag = 1;
        s_ms_counter = 0;
    }
}
