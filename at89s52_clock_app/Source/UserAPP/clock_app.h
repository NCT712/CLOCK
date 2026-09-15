/**
 * @file clock_app.h
 * @brief Tầng ứng dụng chính (Application Layer) - Quản lý FSM và Logic Đồng hồ số.
 * @author Team Clock AT89S52
 * @date 2026
 */

#ifndef __CLOCK_APP_H
#define __CLOCK_APP_H

#include <stdint.h>

/**
 * @brief Định nghĩa các trạng thái (State) của đồng hồ số.
 */
typedef enum {
    MODE_NORMAL       = 0,  /**< Chế độ hoạt động bình thường, hiển thị HH.MM */
    MODE_SET_HOUR     = 1,  /**< Chế độ cài đặt Giờ thực (HH nhấp nháy) */
    MODE_SET_MINUTE   = 2,  /**< Chế độ cài đặt Phút thực (MM nhấp nháy) */
    MODE_ALARM_HOUR   = 3,  /**< Chế độ cài đặt Giờ hẹn báo thức (HH nhấp nháy, LED nhấp nháy) */
    MODE_ALARM_MINUTE = 4,  /**< Chế độ cài đặt Phút hẹn báo thức (MM nhấp nháy, LED nhấp nháy) */
    MODE_ALARM_RINGING= 5   /**< Đang trong thời gian đổ chuông báo thức (5s) */
} ClockMode_t;

/*============================================================================
 * PUBLIC APPLICATION API
 *===========================================================================*/

/**
 * @brief Khởi tạo trạng thái ban đầu của ứng dụng đồng hồ.
 *
 * @details
 * - Thiết lập giờ, phút, giây ban đầu về 00:00:00.
 * - Thiết lập giờ hẹn mặc định về 00:00:00.
 * - Đặt chế độ mặc định MODE_NORMAL.
 * - Khởi tạo các module ngoại vi (Segment, Button, Buzzer, Led).
 *
 * @param None
 * @return None
 */
void ClockApp_Init(void);

/**
 * @brief Task định kỳ 1ms chạy trong vòng lặp chính (Super Loop).
 *
 * @details
 * Được gọi khi timer_1ms_flag = 1:
 * 1. Quét nút nhấn và lọc chống rung (Button_Task_1ms).
 * 2. Xử lý sự kiện nhấn phím (FSM Event Dispatcher).
 * 3. Quản lý trạng thái nhấp nháy 0.5s ON / 0.5s OFF của LED 7 đoạn và LED báo.
 * 4. Cập nhật máy trạng thái còi Buzzer (Buzzer_Task_1ms).
 * 5. Cập nhật dữ liệu hiển thị và quét 1 vị trí LED 7 đoạn (Digital_Scan).
 *
 * @param None
 * @return None
 */
void ClockApp_Task_1ms(void);

/**
 * @brief Task định kỳ 1 giây chạy trong vòng lặp chính (Super Loop).
 *
 * @details
 * Được gọi khi timer_1s_flag = 1:
 * 1. Tăng thời gian thực (Giây -> Phút -> Giờ).
 * 2. So sánh thời gian thực với giờ hẹn để kích hoạt chuông báo thức.
 * 3. Đếm ngược thời gian Timeout 30s khi đang ở các chế độ cài đặt (SETUP / ALARM).
 *    Nếu hết 30s không thao tác, tự động thoát về MODE_NORMAL và phát tiếng pip 300ms.
 *
 * @param None
 * @return None
 */
void ClockApp_Task_1s(void);

#endif /* __CLOCK_APP_H */
