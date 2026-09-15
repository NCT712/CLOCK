/**
 * @file Buzzer.h
 * @brief Module điều khiển còi chip (Buzzer) phi phong tỏa (Non-blocking State Machine).
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @details Theo yêu cầu đề bài:
 * - Kêu "pip" 300ms (0.3s) khi bấm phím hoặc khi timeout tự thoát ra chế độ thường.
 * - Kêu "pip-pip" liên tục trong 5s với chu kỳ 1s (0.5s ON - 0.5s OFF) khi báo thức.
 * - Toàn bộ hoạt động non-blocking (không dùng hàm delay làm chậm hệ thống).
 */

#ifndef __BUZZER_H
#define __BUZZER_H

#include <stdint.h>

/* Thời gian kêu pip ngắn khi nhấn phím (ms) */
#define BUZZER_BEEP_SHORT_MS    300

/* Tổng thời gian báo thức (ms) */
#define BUZZER_ALARM_TOTAL_MS   5000

/* Chu kỳ nhịp báo thức: 500ms ON, 500ms OFF */
#define BUZZER_ALARM_CYCLE_MS   1000
#define BUZZER_ALARM_HALF_MS    500

/**
 * @brief Khởi tạo module Buzzer.
 *
 * @param None
 * @return None
 */
void Buzzer_Init(void);

/**
 * @brief Kích hoạt còi kêu pip 1 lần với thời gian chỉ định (Non-blocking).
 *
 * @param[in] duration_ms Thời gian còi kêu (miligiây). Mặc định là 300ms.
 * @return None
 */
void Buzzer_Beep(uint16_t duration_ms);

/**
 * @brief Kích hoạt chế độ báo thức (Kêu liên tục 5s với nhịp 0.5s ON - 0.5s OFF).
 *
 * @param None
 * @return None
 */
void Buzzer_Alarm_Start(void);

/**
 * @brief Dừng ngay lập tức hoạt động của còi.
 *
 * @param None
 * @return None
 */
void Buzzer_Alarm_Stop(void);

/**
 * @brief Cập nhật trạng thái còi mỗi 1ms (gọi trong ClockApp_Task_1ms).
 *
 * @param None
 * @return None
 */
void Buzzer_Task_1ms(void);

#endif /* __BUZZER_H */
