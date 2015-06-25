void setup_adc(void);
void turnoff_adc(void);
void turnon_adc(void);
double conversion(uint8_t *calibrate, float *angle, uint8_t *measurement);
void measure(uint8_t *measurement);
void calibration(uint8_t *measurement);
