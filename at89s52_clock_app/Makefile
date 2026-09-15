#==============================================================================
# MAKEFILE - DỰ ÁN ĐỒNG HỒ SỐ AT89S52
# Trình biên dịch: SDCC (Small Device C Compiler)
# Trình nạp: avrdude
#==============================================================================

SHELL = cmd.exe

# Tên file xuất xưởng
TARGET = clock_app

# Công cụ
CC      = sdcc
PACKIHX = packihx
AVRDUDE = avrdude

# Cấu hình nạp vi điều khiển AT89S52
PROGRAMMER   = usbasp
MCU          = 89s52
AVRDUDE_CONF = ../avrdude.conf

# Thư mục
BUILD_DIR = build
SRC_DIR   = Source

# Cờ biên dịch SDCC
CFLAGS   = -mmcs51 --model-small --std-c99 --opt-code-speed
INCLUDES = -I$(SRC_DIR)/Driver -I$(SRC_DIR)/Module -I$(SRC_DIR)/UserAPP

# Danh sách file đối tượng .rel
OBJS = \
    $(BUILD_DIR)/Driver/Timer0.rel \
    $(BUILD_DIR)/Driver/GPIO.rel \
    $(BUILD_DIR)/Module/Segment.rel \
    $(BUILD_DIR)/Module/Button.rel \
    $(BUILD_DIR)/Module/Buzzer.rel \
    $(BUILD_DIR)/Module/Led.rel \
    $(BUILD_DIR)/UserAPP/clock_app.rel \
    $(BUILD_DIR)/UserAPP/main.rel

# Mục tiêu mặc định
all: directories $(BUILD_DIR)/$(TARGET).hex

directories:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@if not exist $(BUILD_DIR)\Driver mkdir $(BUILD_DIR)\Driver
	@if not exist $(BUILD_DIR)\Module mkdir $(BUILD_DIR)\Module
	@if not exist $(BUILD_DIR)\UserAPP mkdir $(BUILD_DIR)\UserAPP

# Tạo file HEX từ IHX
$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).ihx
	@echo [PACKIHX] Chuyen doi file IHX sang HEX...
	$(PACKIHX) $< > $@
	@echo ==========================================================
	@echo [SUCCESS] Build thanh cong: $@
	@echo ==========================================================

# Liên kết các file .rel thành file .ihx
$(BUILD_DIR)/$(TARGET).ihx: $(OBJS)
	@echo [LINK] Lien ket cac module thanh file $(TARGET).ihx...
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Biên dịch từng file .c thành .rel
$(BUILD_DIR)/Driver/%.rel: $(SRC_DIR)/Driver/%.c
	@echo [CC] $<
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/Module/%.rel: $(SRC_DIR)/Module/%.c
	@echo [CC] $<
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/UserAPP/%.rel: $(SRC_DIR)/UserAPP/%.c
	@echo [CC] $<
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Lệnh nạp chương trình vào AT89S52 qua mạch nạp USBasp
flash: $(BUILD_DIR)/$(TARGET).hex
	@echo [FLASH] Dang nap code vao AT89S52 qua $(PROGRAMMER)...
	$(AVRDUDE) -C $(AVRDUDE_CONF) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

# Kiểm tra kết nối vi điều khiển
check:
	$(AVRDUDE) -C $(AVRDUDE_CONF) -c $(PROGRAMMER) -p $(MCU)

# Dọn dẹp thư mục build
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo [CLEAN] Da don dep thu muc $(BUILD_DIR).

.PHONY: all directories clean flash check
