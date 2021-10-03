/*
 * J3_IBUS_FLYSKY.h
 *
 *  Created on: Sep 23, 2021
 *      Author: Nelson Lima
 */

#ifndef INC_USR_J3_IBUS_FLYSKY_H_
#define INC_USR_J3_IBUS_FLYSKY_H_

#include "usart.h"

struct TRxIBus{
  UART_HandleTypeDef* uart;
  uint8_t qdtCanal;
  uint8_t* buffer;
  uint16_t* canais;
};


typedef struct TRxIBus TRxIBus;

TRxIBus* J3_IBUS_new(UART_HandleTypeDef* _uart, uint8_t _qtdCanal); /* Retorna um ponteiro para um um RxIBus */
void J3_IBUS_ProcessBuffer(TRxIBus* _RxIBus);
uint16_t J3_IBUS_GetCh(TRxIBus* _RxIBus, uint8_t _canal); /* Retorna o valor do canal */

#endif /* INC_USR_J3_IBUS_FLYSKY_H_ */
