#include <epaper_tasks.h>

DisplayData display_data_arr[10];
uint8_t display_data_arr_count = 0;
uint8_t text_line = 0;
uint16_t char_pos = 0;
uint16_t scale_line = 0;
uint16_t char_line = 0;
uint16_t char_byte = 0;
char buff[MAX_BUFF_LEN];
volatile uint8_t buff_len = 0;

volatile task_ptr_t next_task = NULL;

static void PrepareChar(void) {
    uint16_t x, y, fwidth, fheight;
    uint8_t scale, fvis;

    spiSendDataStr((uint8_t*)buff, buff_len);
    memset(buff, 0, MAX_BUFF_LEN);
    buff_len = 0;

    x = display_data_arr[text_line].x;
    y = display_data_arr[text_line].y;
    scale = display_data_arr[text_line].scale;

    fwidth = 2;
    fheight = 20;

    fvis = (scale == 1) ? 16 : 12;

    x += (fvis * scale) * char_pos;
    x -= x % 8;
    SetMemoryArea(x, y, x + (fwidth * scale * 8) - 1, y + (fheight * scale) - 1);
    SetMemoryPointer(x, y);
    spiSendCmdChar(WRITE_RAM);
}

static void scaleByte(uint8_t byte, uint8_t scale) {
    uint8_t idx = buff_len;
    for (uint8_t i = 0; i < scale; i++) {
        buff[idx + i] = 0xFF;
        buff_len++;
    }
    for (uint8_t get_bit = 0; get_bit < 8; get_bit++) {
        uint8_t set_bit = get_bit * scale;
        if ((byte & (1 << get_bit)) == 0) {
            for (uint8_t s  = 0; s < scale; s++) {
                uint8_t pos_byte = (scale - (set_bit + s) / 8);
                uint8_t pos_bit = (set_bit + s) % 8;
                buff[idx + pos_byte - 1] &= ~(1 << pos_bit);
            }
        }
    }
}

static void procCharLineByte(void) {
    char gc_chr = display_data_arr[text_line].text[char_pos];
    uint8_t idx = cgGetIdx((uint8_t)gc_chr);
    const char* cg_char = cgTab[idx];
    char cg_char_byte = cg_char[(char_line * 2) + char_byte];
    uint8_t scale = display_data_arr[text_line].scale;
    scaleByte(~cg_char_byte, scale);
    if (buff_len > 32) {
        spiSendDataStr((uint8_t*)buff, buff_len);
        memset(buff, 0, MAX_BUFF_LEN);
        buff_len = 0;
    }
}

void tasks_init(void) {
    setupTasks(app,
            ep_resetL, ep_resetH, ep_initFull,
            ep_initPartial, ep_writeText,
            ep_applyPartial, ep_clearMem, ep_sleep
            );

    setupTasks(vtimer, do_next_task, update_time);
    initSchedulerVTTimer();
}

uint32_t counter = 0;

void update_time(uint8_t unused) {
    counter += 16; // on 16 seconds timer (only works in multiples of 16)

    char temp_str[7] = "";

    char temp_c_int_str[3] = "", temp_c_float_str[3] = "";
    itoa(temp_c_int, temp_c_int_str, 10);
    itoa(temp_c_float, temp_c_float_str, 10);

    if (temp_c_int < 10) {
        temp_c_int_str[1] = temp_c_int_str[0];
        temp_c_int_str[0] = '0';
    }
    if (temp_c_float < 10) {
        temp_c_float_str[1] = temp_c_float_str[0];
        temp_c_float_str[0] = '0';
    }

    strcat(temp_str, temp_c_int_str);
    strcat(temp_str, ",");
    strcat(temp_str, temp_c_float_str);
    strcat(temp_str, "c");

    DisplayData display_data[2];
    display_data[0].x = 32;
    display_data[0].y = 24;
    strcpy(display_data[0].text, temp_str);
    display_data[0].scale = 2;
    display_data[0].scale_line = 0;

    uint8_t minutes = counter / 60;
    const uint8_t hours = minutes / 60;
    minutes = minutes % 60;
    const uint8_t seconds = counter % 60;

    char hours_str[3] = "", min_str[3] = "", sec_str[3] = "";
    itoa(hours, hours_str, 10);
    itoa(minutes, min_str, 10);
    itoa(seconds, sec_str, 10);

    if (hours < 10) {
        hours_str[1] = hours_str[0];
        hours_str[0] = '0';
    }
    if (minutes < 10) {
        min_str[1] = min_str[0];
        min_str[0] = '0';
    }
    if (seconds < 10) {
        sec_str[1] = sec_str[0];
        sec_str[0] = '0';
    }

    char time[9] = "";
    strcat(time, hours_str);
    strcat(time, ":");
    strcat(time, min_str);
    strcat(time, ":");
    strcat(time, sec_str);

    display_data[1].x = 8;
    display_data[1].y = 160;
    strcpy(display_data[1].text, time);
    display_data[1].scale = 2;
    display_data[1].scale_line = 0;

    set_display_data_arr(display_data, 2);
    start_sampling_adc();

    postTask(app, ep_resetL, UNUSED);
    startVTimer(update_time, 16000, UNUSED);
}


void do_next_task(uint8_t unused) {
    postTask(app, next_task, UNUSED);
}

void ep_resetL(uint8_t unused) {
    P6OUT &= ~BIT3;
    next_task = ep_resetH;
    startVTimer(do_next_task, 80, UNUSED);
}

void ep_resetH(uint8_t unused) {
    P6OUT |= BIT3;
    next_task = ep_initFull;
    startVTimer(do_next_task, 80, UNUSED);
}

void ep_initFull(uint8_t unused) {
    cfgDisp();
    lutFull();
    ClearPatternMemory(0xFF);
    DisplayPattern();
    ClearPatternMemory(0xFF);
    DisplayPattern();
    next_task = ep_initPartial;
    startVTimer(do_next_task, 80, UNUSED);
}

void ep_initPartial(uint8_t unused) {
    cfgDisp();
    lutPartial();
    ClearPatternMemory(0xFF);
    next_task = ep_writeText;
    startVTimer(do_next_task, 80, UNUSED);
}

void ep_writeText(uint8_t unused) {
    if (text_line < 10 && display_data_arr[text_line].scale != 0) {
        if (char_pos < strlen(display_data_arr[text_line].text)) {
            if (char_line == 0 && char_byte == 0 && scale_line == 0) {
                PrepareChar();
            }
            procCharLineByte();
            char_byte++;
            if (char_byte == 2) {
                char_byte = 0;
                scale_line++;
            }
            if (scale_line == display_data_arr[text_line].scale) {
                scale_line = 0;
                char_line++;
            }
            if (char_line == 20) {
                char_line = 0;
                char_pos++;
            }
            if (char_pos >= strlen(display_data_arr[text_line].text)) {
                text_line++;
                char_pos = 0;
            }
        } else {
            text_line++;
            display_data_arr_count--;
        }
        next_task = ep_writeText;
    } else { // no more text
        DisplayPattern();
        next_task = ep_sleep;
    }
    postTask(app, next_task, UNUSED);
}

void ep_clearMem(uint8_t unused) {
    ClearPatternMemory(0xFF);
    DisplayPattern();
    next_task = ep_sleep;
    startVTimer(do_next_task, 80, UNUSED);
}

void ep_applyPartial(uint8_t unused) {
}

void ep_sleep(uint8_t unused) {
    dispSleep();
    __bis_SR_register(LPM0_bits + GIE);
}

void set_display_data_arr(DisplayData* data_arr, size_t num_elements) {
    // copies display data to global variable
    memset(display_data_arr, 0, 10 * sizeof(DisplayData));
    memcpy(display_data_arr, data_arr, num_elements * sizeof(DisplayData));
    display_data_arr_count = num_elements;
    text_line = char_pos = scale_line = char_line = char_byte = 0;
}

