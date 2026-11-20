/**
  ******************************************************************************
  * @file    app_x-cube-ai.c
  * @brief   Initialisation materielle de l'IA (NPU, RAM, RISAF).
  *          L'application de validation est retiree : l'inference est faite
  *          par le thread "Meteo" (Application/User/Meteo/meteo_app.c).
  ******************************************************************************
  */
#ifdef __cplusplus
extern "C" {
#endif

#include "app_x-cube-ai.h"
#include "npu_init.h"

/* USER CODE BEGIN includes */
/* USER CODE END includes */

/* Appele dans main() avant le demarrage de ThreadX :
 * horloges et RAM du NPU, droits d'acces RIF/RISAF. */
void STM32CubeAI_Studio_AI_Init(void)
{
    aiPreInitialize();
    /* USER CODE BEGIN init */
    /* USER CODE END init */
}

/* Plus utilise : avec ThreadX, la boucle while(1) de main() n'est jamais atteinte */
void STM32CubeAI_Studio_AI_Process(void)
{
}

void STM32CubeAI_Studio_AI_Deinit(void)
{
}

#ifdef __cplusplus
}
#endif
