/**
 * @file main.c
 * @brief Điểm khởi đầu chương trình (Entry Point) - Kiến trúc Super Loop trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @note HƯỚNG DẪN DÀNH CHO NGƯỜI THỰC HIỆN:
 *       Tìm hiểu mô hình bất đồng bộ Flag-based Super Loop (vòng lặp cờ ngắt).
 *       Hoàn thiện hàm main theo hướng dẫn chi tiết bên dưới.
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include <at89x52.h>
#include "../Driver/GPIO.h"
#include "../Driver/Timer0.h"
#include "clock_app.h"

/*_____ M A I N   F U N C T I O N __________________________________________*/

/**
 * @brief Hàm chính của vi điều khiển AT89S52.
 *
 * GỢI Ý CHO NGƯỜI THỰC HIỆN:
 * 1. Khởi tạo phần cứng (Driver layer):
 *    - GPIO_Init();   -> Đưa toàn bộ các chân I/O về mức an toàn.
 *    - Timer0_Init(); -> Cấu hình ngắt định thời Timer 0 chu kỳ 1ms.
 *
 * 2. Khởi tạo ứng dụng (UserAPP layer):
 *    - ClockApp_Init(); -> Nạp trạng thái ban đầu của đồng hồ (00:00).
 *
 * 3. Vòng lặp chính (Super Loop):
 *    while (1)
 *    {
 *        // Task 1ms: Xử lý nút bấm, nhấp nháy, còi, quét hiển thị LED
 *        if (timer_1ms_flag)
 *        {
 *            timer_1ms_flag = 0;   // BẮT BUỘC: Xóa cờ TRƯỚC khi gọi task
 *            ClockApp_Task_1ms();
 *        }
 *
 *        // Task 1s: Tăng giờ/phút/giây, đếm ngược timeout 30s
 *        if (timer_1s_flag)
 *        {
 *            timer_1s_flag = 0;    // BẮT BUỘC: Xóa cờ TRƯỚC khi gọi task
 *            ClockApp_Task_1s();
 *        }
 *    }
 */
void main(void)
{
    /*======================================================================
     * 1. KHỞI TẠO PHẦN CỨNG & ỨNG DỤNG
     *=====================================================================*/
    GPIO_Init();
    Timer0_Init();
    ClockApp_Init();

    /*======================================================================
     * 2. VÒNG LẶP CHÍNH (FLAG-BASED SUPER LOOP)
     *=====================================================================*/
    while (1)
    {
        /* Kiểm tra cờ ngắt 1ms (set bởi Timer 0 ISR) */
        if (timer_1ms_flag)
        {
            timer_1ms_flag = 0;
            ClockApp_Task_1ms();
        }

        /* Kiểm tra cờ ngắt 1s (set bởi Timer 0 ISR) */
        if (timer_1s_flag)
        {
            timer_1s_flag = 0;
            ClockApp_Task_1s();
        }
    }
}
