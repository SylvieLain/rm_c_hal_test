#include "app_main.h"
#include "app_qd4310_can.h"
#include "main.h"

void user_main(void)
{
  if (qd4310_can1_init() != HAL_OK)
  {
    Error_Handler();
  }

  while (1)
  {

  }
}
