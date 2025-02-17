// Definição das biliotecas a serem utilizadas na execução
#include <stdio.h> 
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h" 
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/time.h" 
#include "inc/ssd1306.h"
#include "inc/font.h" 

#define I2C_PORT i2c1
#define I2C_SDA 14          //Definição da porta GPIO da comunicação I2C (DADOS)
#define I2C_SCL 15          //Definição da porta GPIO da comunicação I2C (CLOCK)
#define endereco 0x3c       //Definição do endereço do display ssd1306
#define LED_PIN_GREEN 11    //Definição da porta GPIO do led verde do conjunto RGB 
#define LED_PIN_BLUE 12     //Definição da porta GPIO do led azul do conjunto RGB 
#define LED_PIN_RED 13      //Definição da porta GPIO do led vermelho do conjunto RGB
#define BUTTON_A 5          //Definição da porta GPIO do botão A
#define BUTTON_JOY 22       //Definição da porta GPIO do botão do Joystick
#define VRX_PIN 27          //Definição da porta GPIO do potenciômetro do eixo X do Joystick
#define VRY_PIN 26          //Definição da porta GPIO do potenciômetro do eixo Y do Joystick

// Definição de todos os contadores, flags, variáveis e estruturas que serão utilizadas de forma global 
static volatile bool ledg = false;
static volatile bool leds = true;
static volatile bool rec2 = false;
static volatile uint32_t ultimo_tempo = 0;
static ssd1306_t ssd; 
static volatile bool cor = true;

// Definição do divisor de clock e do wrap a serem usados no PWM
const float CLKDIV = 6.1;
const uint16_t WRAP = 2048;

// Função que administrará a interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

// Função que inicializará todas as entradas, saídas e interfaces necessárias à execução do código
void setup(){
    // Inicialização do LED verde do conjunto RGB e definição como saída
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    // Inicialização do botão A e do botão do joystick, definição como entrada e acionamento do pull-up interno
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_JOY);
    gpio_set_dir(BUTTON_JOY, GPIO_IN);
    gpio_pull_up(BUTTON_JOY);

    // Inicialização e configuração da I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); 
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C_SDA); 
    gpio_pull_up(I2C_SCL);

    // Inicialização e configuração do display ssd1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 
    ssd1306_config(&ssd); 
    ssd1306_send_data(&ssd); 
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicialização e configuração do PWM para o LED vermelho
    gpio_set_function(LED_PIN_RED, GPIO_FUNC_PWM);
    uint slice_num1 = pwm_gpio_to_slice_num(LED_PIN_RED);
    pwm_set_clkdiv(slice_num1, CLKDIV);
    pwm_set_wrap(slice_num1, WRAP);
    pwm_set_enabled(slice_num1, true);

    // Inicialização e configuração do PWM para o LED azul
    gpio_set_function(LED_PIN_BLUE, GPIO_FUNC_PWM);
    uint slice_num2 = pwm_gpio_to_slice_num(LED_PIN_BLUE);
    pwm_set_clkdiv(slice_num2, CLKDIV);
    pwm_set_wrap(slice_num2, WRAP);
    pwm_set_enabled(slice_num2, true);

    // Inicialização do ADC e configuração para os GPIOs 26 e 27
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
}

// Função principal
int main(){

    // Inicialização da biblioteca para comunicação serial (não utilizada, mas pode ser implementada facilmente)
    stdio_init_all();

    // Inicialização dos periféricos necessários
    setup();

    // Interrupção com callback para cada um dos botões 
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOY, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    // Variáveis para auxiliar no controle de luminosidade dos LEDs
    int ledr_value = 0;
    int ledb_value = 0; 

    // Loop infinito
    while (true){

        // Leitura do ADC para a entrada 1 (GPIO 27)
        adc_select_input(1);
        uint16_t vrx_value = adc_read();
        // Se a flag leds estiver ativa, o valor convertido no ADC será avaliado 
        if(leds){
            // Valores maiores que 2100 e menores que 2000 afetam a intensidade da luz
            if(vrx_value > 2100){
                ledr_value = vrx_value-2048;
            } else if(vrx_value < 2000){
                ledr_value = 2048-vrx_value;
            } 
            // Valores entre 2000 e 2100 não afetam a luminosidade
            else if(vrx_value>2000 && vrx_value<2100){
                ledr_value = 0;
            }
        } // Se a flag estiver inativa, o LED fica apagado
        else{
            ledr_value = 0;
        }
        // O valor atual de ledr_value é passado para o PWM
        pwm_set_gpio_level(LED_PIN_RED, ledr_value);

        // Leitura do ADC para a entrada 0 (GPIO 26)
        adc_select_input(0);
        uint16_t vry_value = adc_read();
        // Se a flag leds estiver ativa, o valor convertido no ADC será avaliado 
        if(leds){
            // Valores maiores que 2100 e menores que 2000 afetam a intensidade da luz
            if(vry_value > 2200){
                ledb_value = vry_value-2048;
            } else if(vry_value < 2000){
                ledb_value = 2048-vry_value;
            }
            // Valores entre 2000 e 2100 não afetam a luminosidade 
            else if(vry_value<2100 && vry_value>2000){
                ledb_value = 0;
            }
        } 
        // Se a flag estiver inativa, o LED fica apagado
        else {
            ledb_value = 0;
        }
        // O valor atual de ledb_value é passado para o PWM
        pwm_set_gpio_level(LED_PIN_BLUE, ledb_value);

        // O display é atualizado de acordo com a flag rec2 e com a posição do quadrado 8x8
        ssd1306_fill(&ssd, !cor); 
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
        // Se rec2 estiver ativa, um retângulo a mais é desenhado
        if(rec2){
            ssd1306_rect(&ssd, 4, 4, 120, 56, cor, !cor);
        }
        // Os valores do ADC para cada coordenada são convertidos para um valor que pode ser passado para o display 
        // O fator de cada conversão foi calculado manualmente, a depender dos retângulos que se desejou desenhar
        int coord_x = ceil(vrx_value/38.0);
        int coord_y = ceil(vry_value/88.0);
        // Os loops percorrem o display, colorindo os pixels determinados de acordo com o valor lido do ADC
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                // O fator 5 foi usado para limitar o quadrado ao espaço interior das bordas dos retângulos
                // O fator 48 foi usado para corrigir a direção de movimento no eixo Y
                ssd1306_pixel(&ssd, coord_x+5+j, 48-coord_y+5+i, cor);
            }
        }
        ssd1306_send_data(&ssd);
        // Pequeno delay entre duas iterações consecutivas
        sleep_ms(10);
    }
    return 0;
}


// Função de interrupção com debouncing
static void gpio_irq_handler(uint gpio, uint32_t events){
    // Criação de booleanos para obter o estado de cada botão durante a interrupção
    bool turn_on_adc = gpio_get(BUTTON_A);
    bool turn_on_border = gpio_get(BUTTON_JOY);
    // Obtenção do tempo em que ocorre a interrupção desde a inicialização
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    // Verificação de alteração em um intervalo maior que 300ms (debouncing)
    if(tempo_atual - ultimo_tempo > 300000){
        if(!turn_on_adc){
            //Se o botão A foi pressionado, o tempo é atualizado 
            ultimo_tempo = tempo_atual;
            //E o estado da flag leds é alterado, permitindo ou não que os LEDs de PWM seja ligados
            leds = !leds; 
        } else if(!turn_on_border){
            //Se o botão do joystick foi pressionado, o tempo é atualizado 
            ultimo_tempo = tempo_atual;
            // A flag rec2 é alterada, permitindo ou não a criação de um novo retângulo no display
            rec2 = !rec2;
            // O estado do LED verde é alterado
            ledg = !ledg;
            // A função agora passa o valor de ledg para a GPIO correspondente
            gpio_put(LED_PIN_GREEN, ledg);
        }
    }
}
