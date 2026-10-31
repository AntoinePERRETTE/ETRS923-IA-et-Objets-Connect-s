#include "sensor_task.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

stmdev_ctx_t lps22df_ctx;
stmdev_ctx_t stts22h_ctx;
stmdev_ctx_t sht40ad1b_ctx;

static int size;
static char msg[64];

static struct {
	float lps22df_data[2];	// [0]: pressure; [1]: temperature
	float stts22h_data;
	float sht40ad1b_data[2];	// [0]: humidity; [1]: temperature
} SENSORS_Data;

void SENSORS_startMeasure(void) {
	uint8_t flag;
	uint32_t pressure;
	int16_t temperature;

	if (lps22df_pressure_raw_get(&lps22df_ctx, &pressure) == 0 &&
			lps22df_temperature_raw_get(&lps22df_ctx, &temperature) == 0)
	{
		SENSORS_Data.lps22df_data[0] = lps22df_from_lsb_to_hPa(pressure);
		SENSORS_Data.lps22df_data[1] = lps22df_from_lsb_to_celsius(temperature);

		size = sprintf(msg, "LPS22DF -> P: %.2f hPa, T: %.2f °C\r\n", SENSORS_Data.lps22df_data[0], SENSORS_Data.lps22df_data[1]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	}
	else
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading LPS22DF data\r\n", 35, 100);
	}

	tx_thread_sleep(5);
	stts22h_temp_flag_data_ready_get(&stts22h_ctx, &flag);

	if (!flag) {
		HAL_UART_Transmit(&huart1, (uint8_t*)"STTS22H data was not ready for reading\r\n", 42, 100);
	} else if (stts22h_temperature_raw_get(&stts22h_ctx, &temperature) == 0){
		SENSORS_Data.stts22h_data = stts22h_from_lsb_to_celsius(temperature);

		size = sprintf(msg, "STTS22H -> T: %.2f °C\r\n", SENSORS_Data.stts22h_data);

		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	} else {
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading STTS22H data\r\n", 35, 100);
	}

	tx_thread_sleep(5);

	if (sht40ad1b_data_get(&sht40ad1b_ctx, SENSORS_Data.sht40ad1b_data) == 0) {
		size = sprintf(msg, "SHT40AD1B -> H: %.3f, T: %.3f °C %%\r\n", SENSORS_Data.sht40ad1b_data[0], SENSORS_Data.sht40ad1b_data[1]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	} else {
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading SHT40AD1B\r\n", 31, 100);
	}
}

void SENSORS_Init(void){
	SENSORS_LPS22DF_Init();
	SENSORS_STTS22H_Init();
	SENSORS_SHT40AD1B_Init();
}

static void SENSORS_LPS22DF_Init(void) {
	// Configuration du contexte LPS22DF
	lps22df_ctx.write_reg = platform_lps22df_write;
	lps22df_ctx.read_reg = platform_lps22df_read;
	lps22df_ctx.handle = &hi2c1;
	lps22df_ctx.mdelay = platform_delay;

	// Vérification de la présence du capteur
	lps22df_id_t id;
	if (lps22df_id_get(&lps22df_ctx, &id) != 0 || id.whoami != LPS22DF_ID) {
		HAL_UART_Transmit(&huart1, (uint8_t*)"ERROR: LPS22DF not detected\r\n", 28, 100);
		while (1) { HAL_Delay(100); }
	}

	// Configuration du capteur LPS22DF
	lps22df_md_t md;
	md.odr = LPS22DF_4Hz;
	md.avg = LPS22DF_16_AVG;
	md.lpf = LPS22DF_LPF_DISABLE;
	lps22df_mode_set(&lps22df_ctx, &md);

	size = sprintf(msg, "LPS22DF initialized (WHO_AM_I: 0x%02X)\r\n", id.whoami);
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
}

static void SENSORS_STTS22H_Init(void) {
	// Configuration du contexte STTS22H
	stts22h_ctx.write_reg = platform_stts22h_write;
	stts22h_ctx.read_reg = platform_stts22h_read;
	stts22h_ctx.mdelay = platform_delay;
	stts22h_ctx.handle = &hi2c1;
	stts22h_priv_t auto_inc_info;
	stts22h_ctx.priv_data = &auto_inc_info;

	uint8_t whoamI;
	if (stts22h_dev_id_get(&stts22h_ctx, &whoamI) != 0 || whoamI != STTS22H_ID) {
		HAL_UART_Transmit(&huart1, (uint8_t*)"ERROR: STTS22H not detected\r\n", 28, 100);
	}

	// Configuration du capteur STTS22H
	stts22h_auto_increment_set(&stts22h_ctx, 1);

	/*
	 * Set Output Data Rate
	 * WARNING: this function can reset the device configuration.
	 */
	stts22h_temp_data_rate_set(&stts22h_ctx, STTS22H_1Hz);

	size = sprintf(msg, "STTS22H initialized (WHO_AM_I: 0x%02X)\r\n", whoamI);
	HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
}

static void SENSORS_SHT40AD1B_Init(void) {
	// Configuration du contexte SHT40AD1B
	sht40ad1b_ctx.write_reg = platform_sht40ad1b_write;
	sht40ad1b_ctx.read_reg = platform_sht40ad1b_read;
	sht40ad1b_ctx.mdelay = platform_delay;
	sht40ad1b_ctx.handle = &hi2c1;
}

void SENSORS_LPS22DF_dataGet(void) {
	uint32_t pressure;
	int16_t temperature;

	if (lps22df_pressure_raw_get(&lps22df_ctx, &pressure) == 0 &&
			lps22df_temperature_raw_get(&lps22df_ctx, &temperature) == 0)
	{
		SENSORS_Data.lps22df_data[0] = lps22df_from_lsb_to_hPa(pressure);
		SENSORS_Data.lps22df_data[1] = lps22df_from_lsb_to_celsius(temperature);

		size = sprintf(msg, "LPS22DF -> P: %.2f hPa, T: %.2f °C\r\n", SENSORS_Data.lps22df_data[0], SENSORS_Data.lps22df_data[1]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	}
	else
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading LPS22DF data\r\n", 35, 100);
	}
}

void SENSORS_STTS22H_dataGet(void) {
	uint8_t flag;
	int16_t temperature;
	stts22h_temp_flag_data_ready_get(&stts22h_ctx, &flag);

	if (!flag) {
		HAL_UART_Transmit(&huart1, (uint8_t*)"STTS22H data was not ready for reading\r\n", 42, 100);
	} else if (stts22h_temperature_raw_get(&stts22h_ctx, &temperature) == 0){
		SENSORS_Data.stts22h_data = stts22h_from_lsb_to_celsius(temperature);

		size = sprintf(msg, "STTS22H -> T: %.2f °C\r\n", SENSORS_Data.stts22h_data);

		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	} else {
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading STTS22H data\r\n", 35, 100);
	}
}

void SENSORS_SHT40AD1B_dataGet(void) {
	if (sht40ad1b_data_get(&sht40ad1b_ctx, SENSORS_Data.sht40ad1b_data) == 0) {
		size = sprintf(msg, "SHT40AD1B -> H: %.3f, T: %.3f °C %%\r\n", SENSORS_Data.sht40ad1b_data[0], SENSORS_Data.sht40ad1b_data[1]);
		HAL_UART_Transmit(&huart1, (uint8_t*)msg, size, 100);
	} else {
		HAL_UART_Transmit(&huart1, (uint8_t*)"Error while reading SHT40AD1B\r\n", 31, 100);
	}
}
