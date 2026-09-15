/**
 * @file clock_app.c
 * @brief Máy trạng thái (FSM) và Tầng ứng dụng Đồng hồ số trên AT89S52.
 * @author Team Clock AT89S52
 * @date 2026
 */

/*_____ I N C L U D E S ____________________________________________________*/
#include "clock_app.h"
#include "../Module/Segment.h"
#include "../Module/Button.h"
#include "../Module/Buzzer.h"
#include "../Module/Led.h"

/*_____ M A C R O S ________________________________________________________*/
#define TIMEOUT_SECONDS         30          /**< 30 giây không bấm nút -> tự thoát */
#define BLINK_HALF_PERIOD_MS    500         /**< 500ms ON / 500ms OFF (chu kỳ 1s) */

/*_____ P R I V A T E   F U N C T I O N S __________________________________*/
static void _tick_clock(void);
static void _tick_blink(void);
static void _tick_timeout(void);
static void _process_keys(void);
static void _update_display(void);

static void _handle_setup(void);
static void _handle_alarm(void);
static void _handle_up(void);
static void _handle_down(void);

static void _enter_mode(ClockMode_t new_mode);
static void _exit_to_normal(uint8_t save_time);
static void _reset_timeout(void);

/*_____ V A R I A B L E S __________________________________________________*/

/* Biến lưu trữ thời gian thực tế */
static uint8_t s_hour   = 0;
static uint8_t s_minute = 0;
static uint8_t s_second = 0;

/* Biến lưu trữ thời gian hẹn giờ (báo thức) - lưu trong RAM */
static uint8_t s_alarm_hour   = 0;
static uint8_t s_alarm_minute = 0;

/* Biến tạm thời dùng khi đang chỉnh giờ hoặc chỉnh báo thức */
static uint8_t s_edit_hour   = 0;
static uint8_t s_edit_minute = 0;

/* Biến lưu trạng thái hoạt động hiện tại (State của FSM) */
static ClockMode_t s_mode = MODE_NORMAL;

/* Biến đếm ngược thời gian chờ Timeout 30 giây */
static uint8_t s_timeout_cnt = 0;

/* Quản lý chu kỳ nhấp nháy 0.5s ON / 0.5s OFF */
static uint16_t s_blink_ms = 0;
static uint8_t  s_blink_on = 1;

/* Trạng thái dấu chấm ngăn cách HH.MM */
static uint8_t  s_dot_on = 1;

/*_____ P U B L I C   F U N C T I O N S ____________________________________*/

/**
 * @brief Khởi tạo toàn bộ ứng dụng đồng hồ (Gọi 1 lần trong main.c).
 *
 * YÊU CẦU:
 * - Khởi tạo các module ngoại vi cấp dưới (Segment, Button, Buzzer, Led).
 * - Cài đặt giá trị ban đầu cho đồng hồ thời gian thực (00:00:00) và giờ báo thức.
 * - Đặt chế độ mặc định MODE_NORMAL và hiển thị giá trị ban đầu lên LED 7 đoạn.
 */
void ClockApp_Init(void)
{
    Digital_Init();
    Button_Init();
    Buzzer_Init();
    Led_Init();

    s_hour = 0;
    s_minute = 0;
    s_second = 0;
    s_alarm_hour = 0;
    s_alarm_minute = 0;

    _enter_mode(MODE_NORMAL);
}

/**
 * @brief Task định kỳ 1ms chạy trong vòng lặp chính (Super Loop).
 *
 * YÊU CẦU:
 * - Điều phối lần lượt các công việc định kỳ mỗi 1ms:
 *   1. Quét và lọc chống rung nút nhấn.
 *   2. Đọc sự kiện phím và điều phối chuyển trạng thái FSM.
 *   3. Cập nhật nhịp nhấp nháy hiển thị.
 *   4. Cập nhật máy trạng thái còi Buzzer.
 *   5. Cập nhật nội dung hiển thị vào bộ đệm và quét 1 vị trí LED 7 đoạn.
 */
void ClockApp_Task_1ms(void)
{
    Button_Task_1ms();
    _process_keys();
    _tick_blink();
    Buzzer_Task_1ms();
    _update_display();
    Digital_Scan();
}

/**
 * @brief Task định kỳ 1 giây chạy trong vòng lặp chính (Super Loop).
 *
 * YÊU CẦU:
 * - Tăng thời gian thực (giây, phút, giờ) và kiểm tra kích hoạt chuông báo thức.
 * - Đếm ngược thời gian Timeout 30 giây khi đang ở các chế độ cài đặt.
 */
void ClockApp_Task_1s(void)
{
    _tick_clock();
    _tick_timeout();
}

/*_____ P R I V A T E   F U N C T I O N S __________________________________*/

/**
 * @brief Tăng đồng hồ thời gian thực mỗi giây và kiểm tra điều kiện báo thức.
 *
 * YÊU CẦU:
 * 1. Tăng giây -> đủ 60 giây tăng phút -> đủ 60 phút tăng giờ -> đủ 24 giờ reset về 0.
 * 2. Đảo trạng thái dấu chấm giây ở chế độ bình thường.
 * 3. So sánh giờ thực với giờ hẹn: Nếu trùng khớp thì kích hoạt reo chuông báo thức 5 giây.
 * 4. Tự động thoát khỏi trạng thái reo chuông khi hết thời gian báo thức.
 */
static void _tick_clock(void)
{
    s_second++;
    if (s_second >= 60) {
        s_second = 0;
        s_minute++;
        if (s_minute >= 60) {
            s_minute = 0;
            s_hour++;
            if (s_hour >= 24) {
                s_hour = 0;
            }
        }
    }

    if (s_mode == MODE_NORMAL) {
        s_dot_on = !s_dot_on;
    } else {
        s_dot_on = 1;
    }

    if (s_hour == s_alarm_hour && s_minute == s_alarm_minute && s_second == 0) {
        _enter_mode(MODE_ALARM_RINGING);
        Buzzer_Alarm_Start();
    }
}

/**
 * @brief Quản lý nhịp nhấp nháy chu kỳ 1s (500ms ON / 500ms OFF).
 *
 * YÊU CẦU:
 * - Đếm tích lũy thời gian để đảo trạng thái biến s_blink_on mỗi 500ms.
 * - Khi đang ở chế độ chỉnh giờ hẹn hoặc phút hẹn: Nhấp nháy đèn LED chỉ báo theo nhịp.
 * - Khi ở các chế độ khác: Luôn tắt đèn LED chỉ báo.
 */
static void _tick_blink(void)
{
    s_blink_ms++;
    if (s_blink_ms >= BLINK_HALF_PERIOD_MS) {
        s_blink_ms = 0;
        s_blink_on = !s_blink_on;

        if (s_mode == MODE_ALARM_HOUR || s_mode == MODE_ALARM_MINUTE) {
            Led_Set(s_blink_on);
        } else {
            Led_Set(0);
        }
    }
}

/**
 * @brief Đếm ngược Timeout 30 giây khi không có thao tác ấn nút.
 *
 * YÊU CẦU:
 * - Khi đang ở các chế độ cài đặt (SETUP hoặc ALARM), nếu sau 30 giây không có sự kiện bấm nút:
 *   + Tự động hủy các thay đổi chưa xác nhận và thoát về MODE_NORMAL.
 *   + Kích hoạt còi kêu tiếng bíp ngắn 300ms để cảnh báo người dùng.
 */
static void _tick_timeout(void)
{
    if (s_mode == MODE_SET_HOUR || s_mode == MODE_SET_MINUTE || 
        s_mode == MODE_ALARM_HOUR || s_mode == MODE_ALARM_MINUTE) {
        if (s_timeout_cnt > 0) {
            s_timeout_cnt--;
            if (s_timeout_cnt == 0) {
                _exit_to_normal(0);
                Buzzer_Beep(BUZZER_BEEP_SHORT_MS);
            }
        }
    }
}

/**
 * @brief Đọc sự kiện nút nhấn và điều phối xử lý theo State Machine.
 *
 * YÊU CẦU:
 * - Đọc mã phím từ module Button.
 * - Nếu đang đổ chuông báo thức: Bấm nút bất kỳ sẽ tắt chuông ngay lập tức.
 * - Nếu có phím bấm hợp lệ: Phát tiếng bíp ngắn 300ms, reset lại bộ đếm timeout 30s,
 *   và gọi hàm xử lý tương ứng với từng phím (SETUP, ALARM, UP, DOWN).
 */
static void _process_keys(void)
{
    KeyCode_t key = Button_GetKey();
    if (key != KEY_NONE) {
        if (s_mode == MODE_ALARM_RINGING) {
            Buzzer_Alarm_Stop();
            _enter_mode(MODE_NORMAL);
            return;
        }

        Buzzer_Beep(BUZZER_BEEP_SHORT_MS);
        _reset_timeout();

        switch (key) {
            case KEY_SETUP: _handle_setup(); break;
            case KEY_ALARM: _handle_alarm(); break;
            case KEY_UP:    _handle_up(); break;
            case KEY_DOWN:  _handle_down(); break;
            default: break;
        }
    }
}

/**
 * @brief Xử lý nút SETUP (BT1 - P3.5): Cài đặt giờ thực tế.
 *
 * YÊU CẦU:
 * - Chuyển đổi trạng thái theo chu trình:
 *   MODE_NORMAL -> MODE_SET_HOUR -> MODE_SET_MINUTE -> Lưu giờ mới và về MODE_NORMAL.
 * - Sử dụng biến tạm s_edit_hour, s_edit_minute trong lúc điều chỉnh.
 */
static void _handle_setup(void)
{
    if (s_mode == MODE_NORMAL) {
        s_edit_hour = s_hour;
        s_edit_minute = s_minute;
        _enter_mode(MODE_SET_HOUR);
    } else if (s_mode == MODE_SET_HOUR) {
        _enter_mode(MODE_SET_MINUTE);
    } else if (s_mode == MODE_SET_MINUTE) {
        s_hour = s_edit_hour;
        s_minute = s_edit_minute;
        s_second = 0;
        _exit_to_normal(1);
    }
}

/**
 * @brief Xử lý nút ALARM (BT3 - P3.4): Cài đặt giờ báo thức.
 *
 * YÊU CẦU:
 * - Chuyển đổi trạng thái theo chu trình:
 *   MODE_NORMAL -> MODE_ALARM_HOUR -> MODE_ALARM_MINUTE -> Lưu giờ hẹn vào RAM và về MODE_NORMAL.
 * - Bật nháy LED chỉ báo khi đang trong các chế độ chỉnh giờ hẹn.
 */
static void _handle_alarm(void)
{
    if (s_mode == MODE_NORMAL) {
        s_edit_hour = s_alarm_hour;
        s_edit_minute = s_alarm_minute;
        _enter_mode(MODE_ALARM_HOUR);
    } else if (s_mode == MODE_ALARM_HOUR) {
        _enter_mode(MODE_ALARM_MINUTE);
    } else if (s_mode == MODE_ALARM_MINUTE) {
        s_alarm_hour = s_edit_hour;
        s_alarm_minute = s_edit_minute;
        _exit_to_normal(1);
    }
}

/**
 * @brief Xử lý nút UP (+) (BT2 - P3.2): Tăng giờ hoặc phút tương ứng.
 *
 * YÊU CẦU:
 * - Tăng giá trị giờ (0..23) hoặc phút (0..59) tùy theo vị trí đang được chọn điều chỉnh.
 * - Tự động quay vòng về 0 khi vượt ngưỡng tối đa.
 */
static void _handle_up(void)
{
    if (s_mode == MODE_SET_HOUR || s_mode == MODE_ALARM_HOUR) {
        s_edit_hour++;
        if (s_edit_hour >= 24) s_edit_hour = 0;
    } else if (s_mode == MODE_SET_MINUTE || s_mode == MODE_ALARM_MINUTE) {
        s_edit_minute++;
        if (s_edit_minute >= 60) s_edit_minute = 0;
    }
}

/**
 * @brief Xử lý nút DOWN (-) (BT4 - P3.3): Giảm giờ hoặc phút tương ứng.
 *
 * YÊU CẦU:
 * - Giảm giá trị giờ (23..0) hoặc phút (59..0) tùy theo vị trí đang được chọn điều chỉnh.
 * - Tự động quay vòng về giá trị cực đại khi giảm dưới 0.
 */
static void _handle_down(void)
{
    if (s_mode == MODE_SET_HOUR || s_mode == MODE_ALARM_HOUR) {
        if (s_edit_hour == 0) s_edit_hour = 23;
        else s_edit_hour--;
    } else if (s_mode == MODE_SET_MINUTE || s_mode == MODE_ALARM_MINUTE) {
        if (s_edit_minute == 0) s_edit_minute = 59;
        else s_edit_minute--;
    }
}

/**
 * @brief Cập nhật dữ liệu ra bộ đệm hiển thị kèm hiệu ứng nhấp nháy.
 *
 * YÊU CẦU:
 * - Căn cứ vào trạng thái hiện tại (s_mode):
 *   + MODE_NORMAL / ALARM_RINGING: Hiển thị giờ và phút thực tế.
 *   + MODE_SET_HOUR / ALARM_HOUR: Hiển thị giá trị đang chỉnh, nhấp nháy 2 chữ số giờ
 *     (làm ẩn 2 số giờ khi nhịp nhấp nháy ở pha tắt).
 *   + MODE_SET_MINUTE / ALARM_MINUTE: Hiển thị giá trị đang chỉnh, nhấp nháy 2 chữ số phút
 *     (làm ẩn 2 số phút khi nhịp nhấp nháy ở pha tắt).
 */
static void _update_display(void)
{
    uint8_t h, m;

    if (s_mode == MODE_NORMAL || s_mode == MODE_ALARM_RINGING) {
        h = s_hour;
        m = s_minute;
    } else {
        h = s_edit_hour;
        m = s_edit_minute;
    }

    Digital_Display_Clock(h, m, s_dot_on);

    if (!s_blink_on) {
        if (s_mode == MODE_SET_HOUR || s_mode == MODE_ALARM_HOUR) {
            Digital_Set_Blank(0);
            Digital_Set_Blank(1);
        } else if (s_mode == MODE_SET_MINUTE || s_mode == MODE_ALARM_MINUTE) {
            Digital_Set_Blank(2);
            Digital_Set_Blank(3);
        }
    }
}

/**
 * @brief Chuyển sang một chế độ mới.
 *
 * YÊU CẦU:
 * - Cập nhật biến trạng thái s_mode, đồng bộ lại chu kỳ nhấp nháy và reset timeout.
 */
static void _enter_mode(ClockMode_t new_mode)
{
    s_mode = new_mode;
    s_blink_ms = 0;
    s_blink_on = 1;
    _reset_timeout();
    if (new_mode == MODE_NORMAL) {
        Led_Set(0);
    }
}

/**
 * @brief Thoát khỏi chế độ cài đặt, trở về MODE_NORMAL.
 *
 * YÊU CẦU:
 * - Đưa trạng thái về MODE_NORMAL, tắt LED chỉ báo và xóa bộ đếm timeout.
 */
static void _exit_to_normal(uint8_t save_time)
{
    (void)save_time; /* Dữ liệu đã được lưu trước khi gọi _exit_to_normal nếu save_time=1 */
    _enter_mode(MODE_NORMAL);
}

/**
 * @brief Thiết lập lại bộ đếm Timeout về 30 giây.
 */
static void _reset_timeout(void)
{
    s_timeout_cnt = TIMEOUT_SECONDS;
}
