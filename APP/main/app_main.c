#include "app_main.h"
#include "app_qd4310_can.h"
#include "board.h"
#include "main.h"

void user_main(void)
{
  board_init();
  if (qd4310_can1_init() != HAL_OK)
  {
    Error_Handler();
  }

  

  while (1)
  {

  }
}
