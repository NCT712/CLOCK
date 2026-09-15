/**
 * @file Timer0.h
 * @brief Driver định thời phần cứng Timer 0 trên AT89S52 (1ms System Tick).
 * @author Team Clock AT89S52
 * @date 2026
 *
 * @note Sử dụng thạch anh 11.0592 MHz (hoặc 12.000 MHz).
 *       Với thạch anh 11.0592 MHz:
 *       - Tần số chu kỳ máy = 11.0592 MHz / 12 = 921,600 Hz.
 *       - 1 chu kỳ máy = 1.08507 us.
 *       - Số xung cần đếm cho 1ms (1000 us) = 1000 / 1.08507 = 921.6 ~ 922 xung.
 *       - Giá trị nạp lại (Reload) cho 16-bit Timer: 65536 - 922 = 64614 = 0xFC66.
 */

#ifndef __TIMER0_H
#define __TIMER0_H

#include <stdint.h>

/* Tần số thạch anh (Hz) */
#define FOSC_HZ             11059200UL

/* Giá trị nạp lại Timer 0 cho chu kỳ ngắt 1ms (1000 Hz) */
#define TIMER0_RELOAD_1MS   64614U       /* 0xFC66 */
#define TIMER0_TH0_1MS      0xFC
#define TIMER0_TL0_1MS      0x66

/* Cờ báo thời gian được set bởi ISR, xóa bởi Main loop */
extern volatile uint8_t timer_1ms_flag;
extern volatile uint8_t timer_1s_flag;

/**
 * @brief Khởi tạo Timer 0 ở Chế độ 1 (16-bit Timer), kích hoạt ngắt Timer 0.
 *
 * @details
 * - Cấu hình TMOD: Timer 0 Mode 1 (16-bit timer).
 * - Nạp giá trị khởi tạo TH0, TL0 tương ứng với chu kỳ 1ms.
 * - Cho phép ngắt Timer 0 (ET0 = 1) và ngắt toàn cục (EA = 1).
 * - Khởi động bộ đếm Timer 0 (TR0 = 1).
 *
 * @param None
 * @return None
 */
void Timer0_Init(void);

#endif /* __TIMER0_H */
