/**
 * @file Button.h
 * @brief Module đọc và chống rung nút bấm (Debounce & Single-click detector).
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @details Xử lý chống rung cho 4 nút nhấn trên kit AT89S52:
 * - BT1 (P3.5): Nút SETUP
 * - BT3 (P3.4): Nút ALARM
 * - BT2 (P3.2): Nút UP (+)
 * - BT4 (P3.3): Nút DOWN (-)
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>

/* Thời gian lọc chống rung (ms) */
#define BUTTON_DEBOUNCE_MS      20

/* Định nghĩa mã sự kiện nút bấm */
typedef enum {
    KEY_NONE  = 0,      /**< Không có sự kiện bấm */
    KEY_SETUP = 1,      /**< Nhấn nút SETUP (BT1 - P3.5) */
    KEY_ALARM = 2,      /**< Nhấn nút ALARM (BT3 - P3.4) */
    KEY_UP    = 3,      /**< Nhấn nút UP (+) (BT2 - P3.2) */
    KEY_DOWN  = 4       /**< Nhấn nút DOWN (-) (BT4 - P3.3) */
} KeyCode_t;

/**
 * @brief Khởi tạo  đọc nút nhấn.
 *
 * @param None
 * @return None
 */
void Button_Init(void);

/**
 * @brief Quét và xử lý chống rung các nút nhấn, gọi mỗi 1ms.
 *
 * @details
 * - Đọc trạng thái ngõ vào từ các chân GPIO (P3.2 - P3.5).
 * - Sử dụng thuật toán lọc rung phần mềm tích lũy thời gian (Debounce filter).
 * - Bắt sườn xuống (Falling edge) để phát hiện sự kiện nhấn nút 1 lần duy nhất.
 *
 * @param None
 * @return None
 */
void Button_Task_1ms(void);

/**
 * @brief Lấy mã sự kiện phím nhấn vừa xảy ra (FIFO đơn / Single-event queue).
 *
 * @details
 * - Trả về mã sự kiện phím đã được lọc và xác nhận nhấn.
 * - Sau khi đọc, bộ đệm phím tự động được xóa về KEY_NONE.
 *
 * @param None
 * @return KeyCode_t Mã phím được nhấn (KEY_NONE nếu không có nút nào).
 */
KeyCode_t Button_GetKey(void);

#endif /* __BUTTON_H */
