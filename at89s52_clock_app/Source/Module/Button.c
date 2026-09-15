/**
 * @file Button.c
 * @brief Lọc chống rung (Debounce) và bắt sự kiện nút nhấn trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "Button.h"
#include "../Driver/GPIO.h"

/*_____ D E F I N I T I O N S ______________________________________________*/
#define NUM_BUTTONS             4

/* Cấu trúc quản lý trạng thái chống rung của từng nút */
typedef struct {
    uint8_t current_state;      /**< Trạng thái ổn định đã qua lọc */
    uint8_t last_raw_state;     /**< Trạng thái thô ở chu kỳ trước */
    uint8_t debounce_cnt;       /**< Bộ đếm miligiây chống rung */
} ButtonDebounce_t;

/*_____ V A R I A B L E S __________________________________________________*/
static ButtonDebounce_t s_buttons[NUM_BUTTONS];
static KeyCode_t s_pressed_key = KEY_NONE;

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Khởi tạo module nút bấm.
 *
 * YÊU CẦU:
 * - Khởi tạo giá trị mặc định ban đầu cho mảng trạng thái s_buttons (trạng thái nhả, bộ đếm 0).
 * - Đặt s_pressed_key = KEY_NONE.
 */
void Button_Init(void)
{
    uint8_t i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        s_buttons[i].current_state = BTN_RELEASED;
        s_buttons[i].last_raw_state = BTN_RELEASED;
        s_buttons[i].debounce_cnt = 0;
    }
    s_pressed_key = KEY_NONE;
}

/**
 * @brief Hàm nội bộ: Đọc mức logic chân vật lý theo chỉ số nút.
 *
 * @param[in] index Chỉ số nút: 0: SETUP, 1: ALARM, 2: UP, 3: DOWN.
 * @return uint8_t Mức logic đọc từ chân tương ứng (0: Đang bấm, 1: Đang nhả).
 */
static uint8_t _read_raw_pin(uint8_t index)
{
    switch (index) {
        case 0: return PIN_BTN_SETUP;
        case 1: return PIN_BTN_ALARM;
        case 2: return PIN_BTN_UP;
        case 3: return PIN_BTN_DOWN;
        default: return BTN_RELEASED;
    }
}

/**
 * @brief Quét nút và lọc rung phần mềm, gọi định kỳ mỗi 1ms.
 *
 * YÊU CẦU:
 * 1. Đọc trạng thái thô của từng nút bấm qua hàm _read_raw_pin().
 * 2. Thực hiện giải thuật lọc chống rung phần mềm (Debounce) với thời gian trễ 20ms:
 *    - Tín hiệu phải giữ nguyên trạng thái liên tục trong 20ms mới được công nhận là hợp lệ.
 * 3. Bắt sườn xuống (Falling Edge - thời điểm nút bắt đầu được nhấn):
 *    - Mỗi lần nhấn chỉ sinh 1 sự kiện duy nhất vào biến s_pressed_key, giữ nút không bị lặp phím.
 */
void Button_Task_1ms(void)
{
    uint8_t i;
    uint8_t raw_state;

    for (i = 0; i < NUM_BUTTONS; i++) {
        raw_state = _read_raw_pin(i);
        
        /* Nếu trạng thái thô không đổi so với chu kỳ trước */
        if (raw_state == s_buttons[i].last_raw_state) {
            if (s_buttons[i].debounce_cnt < BUTTON_DEBOUNCE_MS) {
                s_buttons[i].debounce_cnt++;
                
                /* Khi đạt đủ số đếm 20ms liên tục (tín hiệu đã ổn định) */
                if (s_buttons[i].debounce_cnt == BUTTON_DEBOUNCE_MS) {
                    /* Kiểm tra sườn xuống: Trạng thái hiện tại đang Nhả (1), Trạng thái mới là Nhấn (0) */
                    if ((s_buttons[i].current_state == BTN_RELEASED) && (raw_state == BTN_PRESSED)) {
                        /* Đẩy sự kiện vào biến lưu (s_pressed_key) */
                        switch (i) {
                            case 0: s_pressed_key = KEY_SETUP; break;
                            case 1: s_pressed_key = KEY_ALARM; break;
                            case 2: s_pressed_key = KEY_UP; break;
                            case 3: s_pressed_key = KEY_DOWN; break;
                            default: break;
                        }
                    }
                    /* Cập nhật lại trạng thái hiện tại (đã qua lọc) */
                    s_buttons[i].current_state = raw_state;
                }
            }
        } else {
            /* Nếu trạng thái thô thay đổi (có rung phím hoặc nhiễu), reset lại quá trình đếm */
            s_buttons[i].last_raw_state = raw_state;
            s_buttons[i].debounce_cnt = 0;
        }
    }
}

/**
 * @brief Lấy mã sự kiện phím nhấn vừa xảy ra và xóa bộ đệm.
 *
 * @param None
 * @return KeyCode_t Mã phím được nhấn (KEY_NONE nếu không có sự kiện).
 *
 * YÊU CẦU:
 * - Trả về mã phím đã được ghi nhận trong s_pressed_key, sau đó xóa cờ này về KEY_NONE.
 */
KeyCode_t Button_GetKey(void)
{
    KeyCode_t key = s_pressed_key;
    s_pressed_key = KEY_NONE; /* Xóa cờ sự kiện sau khi đọc */
    return key;
}
