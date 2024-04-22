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
#define RED_BUTTON_PIN 7
#define YELLOW_BUTTON_PIN 8
#define BLUE_BUTTON_PIN 9
#define ORANGE_BUTTON_PIN 10
#define UP_BUTTON_PIN 11
#define DOWN_BUTTON_PIN 12
#define ROTARY_ENCODER_1_PIN_A 16
#define ROTARY_ENCODER_1_PIN_B 17
#define ROTARY_ENCODER_1_CLICK 18
#define ROTARY_ENCODER_2_PIN_A 19
#define ROTARY_ENCODER_2_PIN_B 20
#define ROTARY_ENCODER_2_CLICK 21

#define HC06_UART_ID uart1
#define HC06_BAUD_RATE 9600
#define HC06_PIN 3
#define HC06_TX_PIN 4
#define HC06_RX_PIN 5

QueueHandle_t xQueue;

typedef struct adc {
    int axis;
    int val;
} adc_t;

typedef struct button_state {
    uint32_t last_change_ms;
    int state;
} button_state_t;

button_state_t button_states[13]; // Temos 13 botões, então criamos um array de 13 elementos

int rotate_callback(uint gpio, uint32_t events) {
    static const int8_t state_table[] = {
        0, -1, 1, 0,
        1, 0, 0, -1,
        -1, 0, 0, 1,
        0, 1, -1, 0};
    static uint8_t encoder_state = 0; // Current state of the encoder

    int8_t encoded;
    int sum;
    int retorno = 0;

    encoded = (gpio_get(ROTARY_ENCODER_1_PIN_A) << 1) | gpio_get(ROTARY_ENCODER_1_PIN_B);
    encoder_state = (encoder_state << 2) | encoded;
    sum = state_table[encoder_state & 0x0f];

    if (sum != 0) {
        if (sum == 1) {
            retorno = 1;
        } else if (sum == -1) {
            retorno = 0;
        }
    }
    return retorno;
}

void button_callback(uint gpio, uint32_t events) {
    adc_t message;
    if (events == GPIO_IRQ_EDGE_RISE || events == GPIO_IRQ_EDGE_FALL) {
        switch (gpio) {
        case GREEN_BUTTON_PIN:
            message.axis = 0;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case RED_BUTTON_PIN:
            // message.axis = 3;
            message.axis = 1;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case YELLOW_BUTTON_PIN:
            message.axis = 2;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case BLUE_BUTTON_PIN:
            message.axis = 3;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case ORANGE_BUTTON_PIN:
            message.axis = 4;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case UP_BUTTON_PIN:
            message.axis = 5;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case DOWN_BUTTON_PIN:
            message.axis = 6;
            message.val = (events == GPIO_IRQ_EDGE_FALL) ? 1 : 0; // 1 para pressionado, 0 para liberado
            break;
        case ROTARY_ENCODER_1_PIN_A:
            message.axis = 7;
            message.val = rotate_callback(ROTARY_ENCODER_1_PIN_A, events);
            break;
        case ROTARY_ENCODER_1_PIN_B:
            message.axis = 8;
            message.val = rotate_callback(ROTARY_ENCODER_1_PIN_B, events);
            break;
        case ROTARY_ENCODER_1_CLICK:
            message.axis = 9;
            break;
        case ROTARY_ENCODER_2_PIN_A:
            message.axis = 10;
            message.val = rotate_callback(ROTARY_ENCODER_2_PIN_A, events);
            break;
        case ROTARY_ENCODER_2_PIN_B:
            message.axis = 11;
            message.val = rotate_callback(ROTARY_ENCODER_2_PIN_B, events);
            break;
        case ROTARY_ENCODER_2_CLICK:
            message.axis = 12;
            break;
        }
        uint32_t current_ms = to_ms_since_boot(get_absolute_time());
        if (current_ms - button_states[message.axis].last_change_ms > 50) { // 50 ms de debounce
            button_states[message.axis].state = message.val;
            button_states[message.axis].last_change_ms = current_ms;
            printf("Button %d: %d\n", message.axis, message.val);
            xQueueSendFromISR(xQueue, &message, (TickType_t)0);
        }
    }
}

void setup() { // Inicializa todos os pinos
    stdio_init_all();

    gpio_init(HC06_PIN);
    gpio_set_dir(HC06_PIN, GPIO_IN);

    gpio_init(GREEN_BUTTON_PIN);
    gpio_set_dir(GREEN_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(GREEN_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(GREEN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(BLUE_BUTTON_PIN);
    gpio_set_dir(BLUE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BLUE_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BLUE_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ORANGE_BUTTON_PIN);
    gpio_set_dir(ORANGE_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(ORANGE_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(ORANGE_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(RED_BUTTON_PIN);
    gpio_set_dir(RED_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RED_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(RED_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(YELLOW_BUTTON_PIN);
    gpio_set_dir(YELLOW_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(YELLOW_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(YELLOW_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(UP_BUTTON_PIN);
    gpio_set_dir(UP_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(UP_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(UP_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(DOWN_BUTTON_PIN);
    gpio_set_dir(DOWN_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(DOWN_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(DOWN_BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // GPIOS FOR THE ENCODER 1
    gpio_init(ROTARY_ENCODER_1_PIN_A);
    gpio_set_dir(ROTARY_ENCODER_1_PIN_A, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_PIN_A);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_1_PIN_B);
    gpio_set_dir(ROTARY_ENCODER_1_PIN_B, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_PIN_B);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_1_CLICK);
    gpio_set_dir(ROTARY_ENCODER_1_CLICK, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_1_CLICK);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_1_CLICK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // GPIOS FOR THE ENCODER 2
    gpio_init(ROTARY_ENCODER_2_PIN_A);
    gpio_set_dir(ROTARY_ENCODER_2_PIN_A, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_PIN_A);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_2_PIN_B);
    gpio_set_dir(ROTARY_ENCODER_2_PIN_B, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_PIN_B);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    gpio_init(ROTARY_ENCODER_2_CLICK);
    gpio_set_dir(ROTARY_ENCODER_2_CLICK, GPIO_IN);
    gpio_pull_up(ROTARY_ENCODER_2_CLICK);
    gpio_set_irq_enabled_with_callback(ROTARY_ENCODER_2_CLICK, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_callback);

    for (int i = 0; i < 13; i++) {
        button_states[i].state = 0;
        button_states[i].last_change_ms = to_ms_since_boot(get_absolute_time());
    }
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
    while (1) {
        if (xQueueReceive(xQueue, &message, portMAX_DELAY) == pdTRUE) {
            write_package(message);
        }
    }
}

void hc06_task(void *p) {
    uart_init(HC06_UART_ID, HC06_BAUD_RATE);
    gpio_set_function(HC06_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(HC06_RX_PIN, GPIO_FUNC_UART);
    hc06_init("melhor_guitarra", "1234");

    while (true) {
        ;
    }
}

int main() {
    stdio_init_all();
    setup();

    printf("Start bluetooth task\n");

    xQueue = xQueueCreate(20, sizeof(adc_t));

    xTaskCreate(hc06_task, "UART_Task 1", 4096, NULL, 1, NULL);
    xTaskCreate(task_send_button_states, "Send Button States", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}