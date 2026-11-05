#include <Arduino.h>

#define PIN_PV_VOLTAGE       34     
#define PIN_PV_CURRENT       35     
#define PIN_PWM_GATE         18     
#define PWM_CHANNEL          0       
#define PWM_FREQ_HZ          20000   
#define PWM_RESOLUTION_BITS  10      
#define PWM_MAX_DUTY_COUNTS  1023    
#define ADC_MAX_VAL          4095.0f 
#define ADC_VREF             3.3f    
#define V_DIVIDER_RATIO      15.7058f
#define ACS712_ZERO_VOLTAGE  1.650f  
#define ACS712_SENSITIVITY   0.100f  
#define DELTA_D              0.001f  
#define D_MAX                0.85f   
#define D_MIN                0.10f   
#define MPPT_SAMPLE_INTERVAL 5       
static float V_old = 36.0f;
static float P_old = 0.0f;
static float duty_cycle = 0.50f;      
static unsigned long last_mppt_time = 0;
static unsigned long last_telemetry_time = 0;

uint16_t read_analog_filtered(uint8_t pin) {
    uint32_t adc_acc = 0;
    for (int i = 0; i < 16; i++) {
        adc_acc += analogRead(pin);
    }
    return (uint16_t)(adc_acc / 16);
}

float read_panel_voltage() {
    uint16_t raw = read_analog_filtered(PIN_PV_VOLTAGE);
    float pin_volts = ((float)raw / ADC_MAX_VAL) * ADC_VREF;
    return pin_volts * V_DIVIDER_RATIO;
}

float read_panel_current() {
    uint16_t raw = read_analog_filtered(PIN_PV_CURRENT);
    float pin_volts = ((float)raw / ADC_MAX_VAL) * ADC_VREF;
    float current = (pin_volts - ACS712_ZERO_VOLTAGE) / ACS712_SENSITIVITY;
    if (current < 0.05f) current = 0.0f; 
    return current;
}

void apply_pwm_duty(float duty) {
    uint32_t duty_counts = (uint32_t)(duty * (float)PWM_MAX_DUTY_COUNTS);
    ledcWrite(PWM_CHANNEL, duty_counts);
}

void execute_mppt_po(float V_pv, float I_pv) {
    float P_pv = V_pv * I_pv;
    float delta_P = P_pv - P_old;
    float delta_V = V_pv - V_old;

    if (delta_P > 0.0f) {
        if (delta_V > 0.0f) {
            duty_cycle -= DELTA_D;
        } else {
            duty_cycle += DELTA_D;
        }
    } else if (delta_P < 0.0f) {
        if (delta_V > 0.0f) {
            duty_cycle += DELTA_D;
        } else {
            duty_cycle -= DELTA_D;
        }
    }

    if (duty_cycle > D_MAX) duty_cycle = D_MAX;
    if (duty_cycle < D_MIN) duty_cycle = D_MIN;

    V_old = V_pv;
    P_old = P_pv;

    apply_pwm_duty(duty_cycle);
}

void send_serial_telemetry(float V_pv, float I_pv, float duty) {
    float P_pv = V_pv * I_pv;
    Serial.printf("%.2f,%.2f,%.1f,%.3f\r\n", V_pv, I_pv, P_pv, duty);
}

void setup() {
    Serial.begin(115200);

    analogSetAttenuation(ADC_11db); 
    pinMode(PIN_PV_VOLTAGE, INPUT);
    pinMode(PIN_PV_CURRENT, INPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
    ledcAttachPin(PIN_PWM_GATE, PWM_CHANNEL);

    apply_pwm_duty(duty_cycle);
}

void loop() {
    unsigned long current_time = millis();

    float v_now = read_panel_voltage();
    float i_now = read_panel_current();

    if (current_time - last_mppt_time >= MPPT_SAMPLE_INTERVAL) {
        last_mppt_time = current_time;

        if (v_now < 12.0f) {
            apply_pwm_duty(0.0f);
        } else {
            execute_mppt_po(v_now, i_now);
        }
    }

    if (current_time - last_telemetry_time >= 50) {
        last_telemetry_time = current_time;
        send_serial_telemetry(v_now, i_now, duty_cycle);
    }
}