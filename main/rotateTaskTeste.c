#include <FreeRTOS.h>

#include <queue.h>

#include <semphr.h>

#include <task.h>

#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>

// #include "hc06.h"

#define ENCA_PIN 16
#define ENCB_PIN 17

void rotate_task(void *p) {
    static const int8_t state_table[] = {
        0, -1,  1,  0,
        1,  0,  0, -1,
        -1,  0,  0,  1,
        0,  1, -1,  0
    };
    uint8_t enc_state = 0; // Current state of the encoder
    int8_t last_encoded = 0; // Last encoded state
    int8_t encoded;
    int sum;
    int last_sum = 0; // Last non-zero sum to filter out noise
    int debounce_counter = 0; // Debounce counter

    // Initialize GPIO pins for the encoder
    gpio_init(ENCA_PIN);
    gpio_init(ENCB_PIN);

    gpio_set_dir(ENCA_PIN, GPIO_IN);
    gpio_set_dir(ENCB_PIN, GPIO_IN);

    gpio_pull_up(ENCA_PIN);  // Enable internal pull-up
    gpio_pull_up(ENCB_PIN);  // Enable internal pull-up

    last_encoded = (gpio_get(ENCA_PIN) << 1) | gpio_get(ENCB_PIN);

    printf("Encoder initialized\n");

    while (1) {
        encoded = (gpio_get(ENCA_PIN) << 1) | gpio_get(ENCB_PIN);
        enc_state = (enc_state << 2) | encoded;
        sum = state_table[enc_state & 0x0f];

        if (sum != 0) {
            if (sum == last_sum) {
                if (++debounce_counter > 1) {  // Check if the same movement is read consecutively
                    if (sum == 1) {
                        printf("RIGHT\n");
                        // uart_putc_raw(uart0, 3);
                        // uart_putc_raw(uart0, 2);
                        // uart_putc_raw(uart0, 0);
                        // uart_putc_raw(uart0, -1);
                    } else if (sum == -1) {
                        printf("LEFT\n");
                        // uart_putc_raw(uart0, 3);
                        // uart_putc_raw(uart0, 3);
                        // uart_putc_raw(uart0, 0);
                        // uart_putc_raw(uart0, -1);
                    }
                    debounce_counter = 0;  // Reset the counter after confirming the direction
                }
            } else {
                debounce_counter = 0;  // Reset the counter if the direction changes
            }
            last_sum = sum;  // Update last_sum to the current sum
        }

        vTaskDelay(pdMS_TO_TICKS(1)); // Poll every 1 ms to improve responsiveness
    }
}

int main() {
    stdio_init_all();
    // uart_init(uart0, 9600);
    // uart_puts(uart0, "UART initialized\n");

    xTaskCreate(rotate_task, "rotate_task", 2048, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
