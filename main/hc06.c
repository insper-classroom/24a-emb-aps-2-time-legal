#include "hc06.h"

bool hc06_check_connection() {
    char str[32];
    int i = 0;
    uart_puts(HC06_UART_ID, "AT");
    while (uart_is_readable_within_us(HC06_UART_ID, 1000)) {
        str[i++] = uart_getc(HC06_UART_ID);
    }
    str[i] = '\0';

    if (strstr(str, "OK") > 0)
        return true;
    else
        return false;
}

bool hc06_set_name(char name[]) {
    char str[32];
    int i = 0;

    sprintf(str, "AT+NAME%s", name);
    uart_puts(HC06_UART_ID, str);
    while (uart_is_readable_within_us(HC06_UART_ID, 1000)) {
        str[i++] = uart_getc(HC06_UART_ID);
    }
    str[i] = '\0';

    if (strstr(str, "OK") > 0)
        return true;
    else
        return false;
}

bool hc06_set_pin(char pin[]) {
    char str[32];
    int i = 0;

    uart_puts(HC06_UART_ID, str);
    while (uart_is_readable_within_us(HC06_UART_ID, 1000)) {
        str[i++] = uart_getc(HC06_UART_ID);
    }
    str[i] = '\0';

    if (strstr(str, "OK") > 0)
        return true;
    else
        return false;
}

bool hc06_set_at_mode(int on) {
    gpio_put(HC06_PIN, on);
}

bool hc06_init(char name[], char pin[]) {
    hc06_set_at_mode(1);
    while (hc06_check_connection() == false) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    while (hc06_set_name(name) == false) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
    while (hc06_set_pin(pin) == false) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    hc06_set_at_mode(0);
}
