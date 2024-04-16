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
    char axis;
    int val;
} adc_t;

void write_package(adc_t data) {
    int val = data.val;
    int msb = val >> 8;
    int lsb = val & 0xFF;

    uart_putc_raw(HC06_UART_ID, data.axis);
    uart_putc_raw(HC06_UART_ID, msb);
    uart_putc_raw(HC06_UART_ID, lsb);
    uart_putc_raw(HC06_UART_ID, -1);
}

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

void task_send_button_states(void *pvParameters) {
    adc_t message;
    while (1) {
        if (uxQueueMessagesWaiting(xQueue) > 0) {
            if (xQueueReceive(xQueue, &message, portMAX_DELAY)) {
                write_package(message);
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