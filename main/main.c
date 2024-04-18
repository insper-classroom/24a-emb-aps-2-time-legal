/*
 * LED blink with FreeRTOS
 */
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include <string.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hc06.h"

#define GREEN_BUTTON_PIN 6
#define BLUE_BUTTON_PIN 7
#define ORANGE_BUTTON_PIN 8
#define RED_BUTTON_PIN 9
#define YELLOW_BUTTON_PIN 10
#define UP_BUTTON_PIN 11
#define DOWN_BUTTON_PIN 12
#define ROTARY_ENCODER_1_PIN_A 16
#define ROTARY_ENCODER_1_PIN_B 17
#define ROTARY_ENCODER_1_CLICK 18
#define ROTARY_ENCODER_2_PIN_A 19
#define ROTARY_ENCODER_2_PIN_B 20
#define ROTARY_ENCODER_2_CLICK 21

#define HC06_UART_ID uart0
#define HC06_BAUD_RATE 9600
#define HC06_CONNECTED_PIN 3
#define HC06_TX_PIN 4
#define HC06_RX_PIN 5

QueueHandle_t xQueue;

typedef struct adc {
    int axis;
    int val;
} adc_t;

void button_callback(uint gpio, uint32_t events) {
    adc_t message;
    if (events == GPIO_IRQ_EDGE_RISE || events == GPIO_IRQ_EDGE_FALL) {
        switch (gpio) {
        case GREEN_BUTTON_PIN:
            message.axis = 0;
            break;
        case BLUE_BUTTON_PIN:
            message.axis = 1;
            break;
        case ORANGE_BUTTON_PIN:
            message.axis = 2;
            break;
        case RED_BUTTON_PIN:
            message.axis = 3;
            break;
        case YELLOW_BUTTON_PIN:
            message.axis = 4;
            break;
        case UP_BUTTON_PIN:
            message.axis = 5;
            break;
        case DOWN_BUTTON_PIN:
            message.axis = 6;
            break;
        case ROTARY_ENCODER_1_PIN_A:
            message.axis = 7;
            break;
        case ROTARY_ENCODER_1_PIN_B:
            message.axis = 8;
            break;
        case ROTARY_ENCODER_1_CLICK:
            message.axis = 9;
            break;
        case ROTARY_ENCODER_2_PIN_A:
            message.axis = 10;
            break;
        case ROTARY_ENCODER_2_PIN_B:
            message.axis = 11;
            break;
        case ROTARY_ENCODER_2_CLICK:
            message.axis = 12;
            break;
        }
        message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
        xQueueSendFromISR(xQueue, &message, (TickType_t)0);
    }
}

void setup() { // Inicializa todos os pinos
    stdio_init_all();

    // gpio_init(HC06_CONNECTED_PIN);
    // gpio_set_dir(HC06_CONNECTED_PIN, GPIO_IN);

    gpio_init(GREEN_BUTTON_PIN);
    gpio_set_dir(GREEN_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(GREEN_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(GREEN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(BLUE_BUTTON_PIN);
    gpio_set_dir(BLUE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BLUE_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(BLUE_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ORANGE_BUTTON_PIN);
    gpio_set_dir(ORANGE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(ORANGE_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(ORANGE_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(RED_BUTTON_PIN);
    gpio_set_dir(RED_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RED_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(RED_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(YELLOW_BUTTON_PIN);
    gpio_set_dir(YELLOW_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(YELLOW_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(YELLOW_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(UP_BUTTON_PIN);
    gpio_set_dir(UP_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(UP_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(UP_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(DOWN_BUTTON_PIN);
    gpio_set_dir(DOWN_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DOWN_BUTTON_PIN);
    // gpio_set_irq_enabled_with_callback(DOWN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_1_PIN_A);
    gpio_set_dir(ROTARY_ENCODER_1_PIN_A, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_PIN_A);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_1_PIN_B);
    gpio_set_dir(ROTARY_ENCODER_1_PIN_B, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_PIN_B);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_1_CLICK);
    gpio_set_dir(ROTARY_ENCODER_1_CLICK, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_CLICK);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_CLICK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_2_PIN_A);
    gpio_set_dir(ROTARY_ENCODER_2_PIN_A, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_PIN_A);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_2_PIN_B);
    gpio_set_dir(ROTARY_ENCODER_2_PIN_B, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_PIN_B);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_2_CLICK);
    gpio_set_dir(ROTARY_ENCODER_2_CLICK, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_CLICK);
    // gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_CLICK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);
}

void write_package(adc_t data) {
    int val = data.val;
    int msb = val >> 8;
    int lsb = val & 0xFF;

    uart_putc_raw(HC06_UART_ID, data.axis);
    uart_putc_raw(HC06_UART_ID, msb);
    uart_putc_raw(HC06_UART_ID, lsb);
    uart_putc_raw(HC06_UART_ID, -1);
}

void task_send_button_states(void *p) {
    adc_t message;
    uint32_t start_ms = to_ms_since_boot(get_absolute_time());
    while (1) {
        if (uxQueueMessagesWaiting(xQueue) > 0) {
            if (xQueueReceive(xQueue, &message, portMAX_DELAY)) {
                uint32_t pressed_ms = to_ms_since_boot(get_absolute_time());
                if (pressed_ms - start_ms > 80) {
                    write_package(message);
                    start_ms = pressed_ms;
                }
            }
        }
    }
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("melhor_guitarra", "1234");

    while (true) {
        if (gpio_get(HC06_CONNECTED_PIN) == 0) {
            // Se a conexão falhar, tente inicializar novamente
            hc06_init("melhor_guitarra", "1234");
        }
    }
}

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
    gpio_init(ROTARY_ENCODER_1_PIN_A);
    gpio_init(ROTARY_ENCODER_1_PIN_B);

    gpio_set_dir(ROTARY_ENCODER_1_PIN_A, GPIO_IN);
    gpio_set_dir(ROTARY_ENCODER_1_PIN_B, GPIO_IN);

    gpio_pull_up(ROTARY_ENCODER_1_PIN_A);  // Enable internal pull-up
    gpio_pull_up(ROTARY_ENCODER_1_PIN_B);  // Enable internal pull-up

    last_encoded = (gpio_get(ROTARY_ENCODER_1_PIN_A) << 1) | gpio_get(ROTARY_ENCODER_1_PIN_B);

    printf("Encoder initialized\n");

    while (1) {
        encoded = (gpio_get(ROTARY_ENCODER_1_PIN_A) << 1) | gpio_get(ROTARY_ENCODER_1_PIN_B);
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
    setup();

    // printf("Start bluetooth task\n");

    xQueue = xQueueCreate(20, sizeof(adc_t));

    // xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(task_send_button_states, "Send Button States", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}