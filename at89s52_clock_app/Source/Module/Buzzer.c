/**
 * @file Buzzer.c
 * @brief Điều khiển còi chip phi phong tỏa (Non-blocking State Machine) trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "Buzzer.h"
#include "../Driver/GPIO.h"

/*_____ V A R I A B L E S __________________________________________________*/
static uint16_t s_buzzer_timer = 0;   /**< Đếm ngược miligiây còn lại của còi */
static uint16_t s_alarm_cycle  = 0;   /**< Đếm chu kỳ 1000ms khi đổ chuông báo thức */
static uint8_t  s_is_alarm     = 0;   /**< Cờ phân biệt: 0 = Pip ngắn, 1 = Báo thức */

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Khởi tạo trạng thái ban đầu cho module Buzzer.
 *
 * YÊU CẦU:
 * - Đảm bảo còi tắt và reset toàn bộ các biến đếm thời gian.
 */
void Buzzer_Init(void)
{
    PIN_BUZZER = BUZZER_OFF;
    s_buzzer_timer = 0;
    s_alarm_cycle = 0;
    s_is_alarm = 0;
}

/**
 * @brief Kích hoạt còi kêu pip một khoảng thời gian (Non-blocking).
 *
 * @param[in] duration_ms Thời gian còi kêu tính theo miligiây.
 *
 * YÊU CẦU:
 * - Nạp thời gian kêu vào s_buzzer_timer, đặt chế độ tiếng bíp thường và bật còi.
 */
void Buzzer_Beep(uint16_t duration_ms)
{
    PIN_BUZZER = BUZZER_ON;
    s_buzzer_timer = duration_ms;
    s_is_alarm = 0;
}

/**
 * @brief Bắt đầu chuỗi kêu báo thức 5 giây với chu kỳ ngắt quãng (0.5s ON - 0.5s OFF).
 *
 * YÊU CẦU:
 * - Thiết lập chế độ báo thức, nạp tổng thời gian 5000ms và bắt đầu nhịp kêu.
 */
void Buzzer_Alarm_Start(void)
{
    PIN_BUZZER = BUZZER_ON;
    s_buzzer_timer = BUZZER_ALARM_TOTAL_MS;
    s_alarm_cycle = 0;
    s_is_alarm = 1;
}

/**
 * @brief Dừng còi ngay lập tức.
 *
 * YÊU CẦU:
 * - Tắt còi phần cứng và xóa toàn bộ thời gian đếm còn lại.
 */
void Buzzer_Alarm_Stop(void)
{
    PIN_BUZZER = BUZZER_OFF;
    s_buzzer_timer = 0;
    s_is_alarm = 0;
}

/**
 * @brief Máy trạng thái còi chạy định kỳ mỗi 1ms trong Task 1ms.
 *
 * YÊU CẦU:
 * - Quản lý việc đếm ngược thời gian kêu mà không dùng hàm delay.
 * - Xử lý 2 chế độ âm thanh:
 *   1. Tiếng bíp thường: Giữ còi bật liên tục cho đến khi hết thời gian nạp.
 *   2. Tiếng chuông báo thức: Tạo nhịp ngắt quãng chu kỳ 1s (500ms BẬT, 500ms TẮT) trong suốt 5s.
 * - Khi hết thời gian, tự động tắt còi hoàn toàn.
 */
void Buzzer_Task_1ms(void)
{
    if (s_buzzer_timer > 0) {
        s_buzzer_timer--;
        
        /* Nếu đang ở chế độ báo thức (Chu kỳ 0.5s ON / 0.5s OFF) */
        if (s_is_alarm) {
            s_alarm_cycle++;
            if (s_alarm_cycle >= BUZZER_ALARM_CYCLE_MS) {
                s_alarm_cycle = 0;
            }
            
            if (s_alarm_cycle < BUZZER_ALARM_HALF_MS) {
                PIN_BUZZER = BUZZER_ON;
            } else {
                PIN_BUZZER = BUZZER_OFF;
            }
        }
        
        /* Khi đếm ngược kết thúc, tắt còi hoàn toàn */
        if (s_buzzer_timer == 0) {
            PIN_BUZZER = BUZZER_OFF;
            s_is_alarm = 0;
        }
    }
}
