/**
 * @file Led.h
 * @brief Module điều khiển đèn LED chỉ báo chế độ hẹn giờ (Alarm LED).
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @details Đáp ứng yêu cầu mục 7 của đề bài:
 * - Ở chế độ thay đổi giờ hẹn hoặc phút hẹn: LED nhấp nháy chu kỳ 1s (0.5s ON - 0.5s OFF).
 * - Ở các chế độ khác: LED ở trạng thái OFF.
 */

#ifndef __LED_H
#define __LED_H

#include <stdint.h>

/**
 * @brief Khởi tạo module LED chỉ báo.
 *
 * @param None
 * @return None
 */
void Led_Init(void);

/**
 * @brief Bật hoặc tắt LED chỉ báo.
 *
 * @param[in] state 1 = Bật, 0 = Tắt.
 * @return None
 */
void Led_Set(uint8_t state);

/**
 * @brief Đảo trạng thái bật/tắt của LED.
 *
 * @param None
 * @return None
 */
void Led_Toggle(void);

#endif /* __LED_H */
