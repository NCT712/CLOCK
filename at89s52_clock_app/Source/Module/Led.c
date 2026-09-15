/**
 * @file Led.c
 * @brief Điều khiển đèn LED chỉ báo trạng thái trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "Led.h"
#include "../Driver/GPIO.h"

/*_____ F U N C T I O N S __________________________________________________*/

/**
 * @brief Khởi tạo đèn LED ở trạng thái tắt ban đầu.
 */
void Led_Init(void)
{
    PIN_LED_ALARM = LED_ALARM_OFF;
}

/**
 * @brief Bật hoặc tắt đèn LED chỉ báo.
 *
 * @param[in] state 1 = Bật, 0 = Tắt.
 *
 * YÊU CẦU:
 * - Điều khiển mức logic chân PIN_LED_ALARM (P2.4) theo trạng thái mong muốn
 *   (chú ý mức tích cực của LED theo thiết kế phần cứng).
 */
void Led_Set(uint8_t state)
{
    if (state) {
        PIN_LED_ALARM = LED_ALARM_ON;
    } else {
        PIN_LED_ALARM = LED_ALARM_OFF;
    }
}

/**
 * @brief Đảo trạng thái hiện tại của đèn LED.
 */
void Led_Toggle(void)
{
    PIN_LED_ALARM = !PIN_LED_ALARM;
}
