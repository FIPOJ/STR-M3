#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/touch_pad.h"

#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_THRESH_PERCENT (80)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)
#define NUM_AMOSTRAS 200

static const char *TAG = "TrabalhoM3";

static bool s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];

portMUX_TYPE mutual_exclusion = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t mutual_exclusion_mutex;

// Variáveis globais
int ACELERADOR = 0;
int FREIO = 0;
int TEMPERATURA = 50;
int QUENTE = 0;
int BATIDA = 0; // airbag
int CINTO = 0; 
int VIDRO_MOTORISTA = 0;
int VIDRO_PASSAGEIRO = 0;
int TRAVA_MOTORISTA = 0;
int TRAVA_PASSAGEIRO = 0;
int FAROIS = 0;
int VELOCIDADE = 0;
int CONSUMO = 99;
float consumo[NUM_AMOSTRAS] = {0};
float velocidade[NUM_AMOSTRAS] = {0};
int indiceAtual = 0;
int totalAmostras = 0; // Variável para contar o número de amostras coletadas

int ACELERADOR_ANTERIOR = 0;
int FREIO_ANTERIOR = 0;
int BATIDA_ANTERIOR = 0;
int CINTO_ANTERIOR = 0;
int VIDRO_MOTORISTA_ANTERIOR = 0;
int VIDRO_PASSAGEIRO_ANTERIOR = 0;
int TRAVA_MOTORISTA_ANTERIOR = 0;
int TRAVA_PASSAGEIRO_ANTERIOR = 0;
int FAROIS_ANTERIOR = 0;
int TEMPERATURA_ANTERIOR = 50;
int QUENTE_ANTERIOR = 0;

uint64_t clock_accelerate_start, ACCELERATE_CLOCK_END, clock_temperature_start, ENGINE_TEMPERATURE_CLOCK_END,
    ABS_CLOCK_END, ABS_CLOCK_START,         // ABS
    AIRBAG_CLOCK_END, AIRBAG_CLOCK_START,   // airbag
    CINTO_CLOCK_END, CINTO_CLOCK_START,     // cinto
    LVT_CLOCK_START,                        // tranca, luz, vidro
    TRANCA_CLOCK_END_M,TRANCA_CLOCK_END_P,                  // tranca do carro
    LUZ_CLOCK_END,                          // Luz dianteira
    VIDRO_CLOCK_END_M,VIDRO_CLOCK_END_P; 

float calcularMedia(float arr[], int numAmostras) {
    float soma = 0;
    for (int i = 0; i < numAmostras; i++) {
        soma += arr[i];
    }
    return soma / numAmostras;
}

void abs_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        
        if (FREIO == 1) {
            ABS_CLOCK_START = esp_timer_get_time();
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            ABS_CLOCK_END = esp_timer_get_time();
        }

        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void airbag_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);

        if (BATIDA == 1) {

            // Obter o tempo inicial
            AIRBAG_CLOCK_START = esp_timer_get_time();

            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar

            // Obter o tempo final
            AIRBAG_CLOCK_END = esp_timer_get_time();
        }
        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void cinto_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);

        if (CINTO == 1) {

            // Obter o tempo inicial
            CINTO_CLOCK_START = esp_timer_get_time();

            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar

            // Obter o tempo final
            CINTO_CLOCK_END = esp_timer_get_time();
        }

        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void ltv_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);

        LVT_CLOCK_START = esp_timer_get_time();

        if (FAROIS == 1) {
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            LUZ_CLOCK_END = esp_timer_get_time();
        }

        if (VIDRO_MOTORISTA == 1) {
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            VIDRO_CLOCK_END_M = esp_timer_get_time();
        }

        if (VIDRO_PASSAGEIRO == 1) {
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            VIDRO_CLOCK_END_P = esp_timer_get_time();
        }

        if (TRAVA_MOTORISTA == 1) {
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            TRANCA_CLOCK_END_M = esp_timer_get_time();
        }

        if (TRAVA_PASSAGEIRO == 1) {
            esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
            esp_rom_delay_us(15); // atraso para agir+viajar
            TRANCA_CLOCK_END_P = esp_timer_get_time();
        }

        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void motor_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
        clock_accelerate_start = esp_timer_get_time();
        esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
        esp_rom_delay_us(15); // atraso para agir+viajar
        ACCELERATE_CLOCK_END = esp_timer_get_time();

        if (ACELERADOR == 1 && FREIO == 0)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            if ( VELOCIDADE < 205)
            VELOCIDADE = VELOCIDADE + 1;
        }
        else if (ACELERADOR == 0 && FREIO == 1){
            vTaskDelay(30 / portTICK_PERIOD_MS);
            if (VELOCIDADE > 0)
            {
                VELOCIDADE = VELOCIDADE - 1;
            }
        }
        else if (ACELERADOR == 0 && FREIO == 0){
            vTaskDelay(200 / portTICK_PERIOD_MS);
            if (VELOCIDADE > 0)
            {
                VELOCIDADE = VELOCIDADE - 1;
            }
        }

        if (VELOCIDADE > 1 && VELOCIDADE < 30)
        {
            CONSUMO = 16;
        } else if (VELOCIDADE > 31 && VELOCIDADE < 60)
        {
            CONSUMO = 14;
        } else if (VELOCIDADE > 61 && VELOCIDADE < 90)
        {
            CONSUMO = 12;
        } else if (VELOCIDADE > 91 && VELOCIDADE < 120)
        {
            CONSUMO = 10;
        } else if (VELOCIDADE > 121 && VELOCIDADE < 150)
        {
            CONSUMO = 8;
        } else if (VELOCIDADE > 151 && VELOCIDADE < 180)
        {
            CONSUMO = 6;
        } else if (VELOCIDADE > 181 && VELOCIDADE < 205)
        {
            CONSUMO = 4;
        }
            
        
        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void temperature_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);    

        if (ACELERADOR == 1)
        {
            vTaskDelay(300 / portTICK_PERIOD_MS);

            if(TEMPERATURA < 140)
            TEMPERATURA = TEMPERATURA + 1;

            if (TEMPERATURA > 100) {
                clock_temperature_start = esp_timer_get_time();
                esp_rom_delay_us(11); // 10us para viajar e 1us para pegar
                esp_rom_delay_us(15); // atraso para agir+viajar
                QUENTE = 1;
                ENGINE_TEMPERATURE_CLOCK_END = esp_timer_get_time();            
            }
        }
        else {
            vTaskDelay(400 / portTICK_PERIOD_MS);
            if (TEMPERATURA > 0)
            {
                TEMPERATURA = TEMPERATURA - 1;
                if (TEMPERATURA < 100) {
                    QUENTE = 0;
                }
            }
        }

        
        xSemaphoreGive(mutual_exclusion_mutex);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void display_task(void *pvParameter) {
    uint64_t soma_inj = 0, soma_temp = 0, soma_abs = 0, soma_airbag = 0, soma_cinto = 0, soma_luz = 0, soma_vidro_m = 0, soma_tranca_m = 0, soma_vidro_p = 0, soma_tranca_p = 0;

    while (1) {
        vTaskDelay(100);  // Equivalente a 100 em milissegundos

        // Calcula o tempo de resposta em microssegundos para ACCELERATE e TEMPERATURE
        if (TRAVA_MOTORISTA && TRAVA_MOTORISTA != TRAVA_MOTORISTA_ANTERIOR) {
            soma_tranca_m = TRANCA_CLOCK_END_M - LVT_CLOCK_START;
        }

        if (TRAVA_PASSAGEIRO && TRAVA_PASSAGEIRO != TRAVA_PASSAGEIRO_ANTERIOR) {
            soma_tranca_p = TRANCA_CLOCK_END_P - LVT_CLOCK_START;
        }

        // Vidro
        if (VIDRO_MOTORISTA && VIDRO_MOTORISTA != VIDRO_MOTORISTA_ANTERIOR) {
            soma_vidro_m = VIDRO_CLOCK_END_M - LVT_CLOCK_START;
        }

        if (VIDRO_PASSAGEIRO && VIDRO_PASSAGEIRO != VIDRO_PASSAGEIRO_ANTERIOR) {
            soma_vidro_p = VIDRO_CLOCK_END_P - LVT_CLOCK_START;
        }

        // Luz frontal
        if (FAROIS && FAROIS != FAROIS_ANTERIOR) {
            soma_luz = LUZ_CLOCK_END - LVT_CLOCK_START;
        }

        // Cinto de segurança
        if (CINTO != CINTO_ANTERIOR){
            soma_cinto = CINTO_CLOCK_END - CINTO_CLOCK_START;
        }
        
        // Airbag
        if (BATIDA != BATIDA_ANTERIOR){
            soma_airbag = AIRBAG_CLOCK_END - AIRBAG_CLOCK_START;
        }

        // ABS - Freio
        if (FREIO != FREIO_ANTERIOR){
            soma_abs = ABS_CLOCK_END - ABS_CLOCK_START;
        }
        
        // Injeção motor
        if (ACELERADOR != ACELERADOR_ANTERIOR){
            soma_inj = ACCELERATE_CLOCK_END - clock_accelerate_start;
        }

        // Temperatura motor
        if (QUENTE == 1 && QUENTE != QUENTE_ANTERIOR) {
            soma_temp = ENGINE_TEMPERATURE_CLOCK_END - clock_temperature_start;
        }
        

        // Limpa a tela e imprime os valores

        printf("\033[2J\033[H");
        printf("Acelerador: %d - Tempo de resposta: %llu us                                       \n", ACELERADOR, soma_inj);
        printf("Freio: %d - Tempo de resposta: %llu us                                            \n", FREIO, soma_abs);
        printf("Airbag: %d - Tempo de resposta: %llu us                                            \n", BATIDA, soma_airbag);
        printf("Cinto colocado: %d - Tempo de resposta: %llu us                                   \n", CINTO, soma_cinto);
        printf("Luz: %d - Tempo de resposta: %llu us                                              \n", FAROIS, soma_luz);
        printf("Trava Motorista: %d - Tempo de resposta: %llu us                                  \n", TRAVA_MOTORISTA, soma_tranca_m);
        printf("Trava Passageiro: %d - Tempo de resposta: %llu us                                 \n", TRAVA_PASSAGEIRO, soma_tranca_p);
        printf("Vidro Motorista: %d - Tempo de resposta: %llu us                                  \n", VIDRO_MOTORISTA, soma_vidro_m);
        printf("Vidro Passageiro: %d - Tempo de resposta: %llu us                                 \n", VIDRO_PASSAGEIRO, soma_vidro_p);
        printf("Temperatura: %d                                                                   \n", TEMPERATURA);
        printf("Superaquecimento: %d - Tempo de resposta: %llu us                                 \n", QUENTE, soma_temp);
        printf("Velocidade: %d                                                                    \n", VELOCIDADE);

        consumo[indiceAtual] = CONSUMO;
        velocidade[indiceAtual] = VELOCIDADE;

        totalAmostras = (totalAmostras < NUM_AMOSTRAS) ? totalAmostras + 1 : NUM_AMOSTRAS;
        if (totalAmostras > 0) {
            float mediaConsumo = calcularMedia(consumo, totalAmostras);
            float mediaVelocidade = calcularMedia(velocidade, totalAmostras);

            printf("Média de Consumo: %f / Média de Velocidade: %f\n                                                                       \n                                                                      \n                                                                      \n", mediaConsumo, mediaVelocidade);
        }
        indiceAtual = (indiceAtual + 1) % NUM_AMOSTRAS;
        int ACELERADOR_ANTERIOR = ACELERADOR;
        int FREIO_ANTERIOR = FREIO;
        int BATIDA_ANTERIOR = BATIDA;
        int CINTO_ANTERIOR = CINTO;
        int VIDRO_MOTORISTA_ANTERIOR = VIDRO_MOTORISTA;
        int VIDRO_PASSAGEIRO_ANTERIOR = VIDRO_PASSAGEIRO;
        int TRAVA_MOTORISTA_ANTERIOR = TRAVA_MOTORISTA;
        int TRAVA_PASSAGEIRO_ANTERIOR = TRAVA_PASSAGEIRO;
        int FAROIS_ANTERIOR = FAROIS;
        int QUENTE_ANTERIOR = QUENTE;
    }
    // return NULL;
}

static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        //read filtered value
        touch_pad_read_filtered(i, &touch_value);
        s_pad_init_val[i] = touch_value;
        ESP_LOGI(TAG, "test init: touch pad [%d] val is %d", i, touch_value);
        //set interrupt threshold.
        ESP_ERROR_CHECK(touch_pad_set_thresh(i, touch_value * 2 / 3));

    }
}

void touch_pad_read_task(void *pvParameter) {
    while (1) {
        touch_pad_intr_enable();
        for (int i = 0; i < TOUCH_PAD_MAX; i++) {
            if (s_pad_activated[i] == true) {

                if (i == 0)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        ACELERADOR = (ACELERADOR == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 9)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        FREIO = (FREIO == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 3)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        BATIDA = (BATIDA == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 4)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        CINTO = (CINTO == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 5)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        VIDRO_MOTORISTA = (VIDRO_MOTORISTA == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 6)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        VIDRO_PASSAGEIRO = (VIDRO_PASSAGEIRO == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 7)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        TRAVA_MOTORISTA = (TRAVA_MOTORISTA == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 8)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        TRAVA_PASSAGEIRO = (TRAVA_PASSAGEIRO == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                else if (i == 3)
                {
                        xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                        FAROIS = (FAROIS == 0) ? 1 : 0;
                        xSemaphoreGive(mutual_exclusion_mutex);
                }
                xSemaphoreTake(mutual_exclusion_mutex, portMAX_DELAY);
                s_pad_activated[i] = false;
                xSemaphoreGive(mutual_exclusion_mutex);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void tp_example_rtc_intr(void *arg)
{
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((pad_intr >> i) & 0x01) {
            s_pad_activated[i] = true;
        }
    }
}

int app_main(int argc, char *argv[])
{

    ESP_LOGI(TAG, "Initializing touch pad");
    ESP_ERROR_CHECK(touch_pad_init());

    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);

    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);

    //inicializa o touch pad
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        touch_pad_config(i, TOUCH_THRESH_NO_USE);
    }

    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);

    tp_example_set_thresholds();

    touch_pad_isr_register(tp_example_rtc_intr, NULL);

    mutual_exclusion_mutex = xSemaphoreCreateCounting(3, 3);

    // Criação de tarefas
    xTaskCreate(&touch_pad_read_task, "touch_pad_read_task", 4096, NULL, 25, NULL);
    xTaskCreate(&motor_task, "Motor", 1024, NULL, configMAX_PRIORITIES, NULL); 
    xTaskCreate(&temperature_task, "Temperature", 1024, NULL, 8, NULL);
    xTaskCreate(&abs_task, "ABS", 1024, NULL, 4, NULL);
    xTaskCreate(&airbag_task, "Airbag", 1024, NULL, 4, NULL);
    xTaskCreate(&display_task, "Display", 2048, NULL, 0, NULL);
    xTaskCreate(&cinto_task, "Cinto", 1024, NULL, 0, NULL);
    xTaskCreate(&ltv_task, "LTV", 1024, NULL, 0, NULL);

    return 0;
}