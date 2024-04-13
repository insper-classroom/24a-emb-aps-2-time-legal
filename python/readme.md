# Documentação do Código de Emulação do Gamepad

Este código Python é projetado para emular um gamepad usando um microcontrolador conectado via serial (USB ou Bluetooth). Ele usa a biblioteca serial para comunicação serial e a biblioteca uinput para criar um dispositivo virtual do gamepad.

---
## Funcionalidade Principal

O código realiza as seguintes operações principais:
- Configuração do Dispositivo Serial: Configura a comunicação serial com a raspberry pico.
- Configuração do Dispositivo de Emulação: Cria um dispositivo virtual de gamepad usando a biblioteca uinput.
- Recepção e Interpretação de Dados: Recebe dados do dispositivo conectado via serial, interpreta esses dados e os emula como entrada de gamepad.

---
## Adicionando ou Removendo Botões

Para adicionar ou remover botões do gamepad emulado, você precisa ajustar algumas partes específicas do código:
### 1. Lista de Botões

As listas buttons e axes definem os botões e eixos disponíveis e seus respectivos códigos. Você pode adicionar ou remover botões e eixos desta lista, conforme necessário.

Por exemplo, para adicionar um novo botão BTN_C, você pode incluir uinput.BTN_C na lista. Da mesma forma, para remover um botão existente, basta remover o correspondente uinput.BTN_X, por exemplo.
(A lista completa de botões do uinput está [aqui](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h?h=v4.7))
### 2. Quantidade de Botões

A variável button_quantity representa a quantidade total de botões no seu controlador, excluindo os movimentos do joystick. Certifique-se de ajustar essa variável para refletir corretamente o número de botões no seu controlador, após adicionar ou remover botões.
### 3. Função no C: (Considerando que você está usando o modelo do lab)
```
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
}
```

Dentro do seu código em C você deve lidar com cada botão e eixo na ordem que está na lista no python, por exemplo, se tenho 4 botões na lista ```buttons = [uinput.BTN_A, uinput.BTN_B, uinput.BTN_X, uinput.BTN_Y]```, eu devo acessá-la utilizando o axis como 0, 1, 2 e 3 para os botões A, B, X e Y do controle, respectivamente. Portanto se você adicionar um botão, você deve contar sempre a posição do novo botão na lista no python e utilzizar esse valor no axis do envio.
### Exemplo de Adição de Botão

Suponha que você deseje adicionar um novo botão chamado BTN_Z ao seu gamepad. Você precisaria seguir estas etapas:

    Adicione uinput.BTN_Z à lista de botões.
    Ajuste button_quantity para refletir a nova quantidade total de botões no seu controlador.

