/*
 * J3_IBUS_FLYSKY.c
 *
 *  Created on: Sep 23, 2021
 *      Author: Nelson Lima
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "usr/J3_IBUS_FLYSKY.h"


//typedef struct TRxIBus TRxIBus;

TRxIBus* J3_IBUS_new(UART_HandleTypeDef* _uart, uint8_t _qtdCanal){
  TRxIBus* auxRxIBus;
  uint8_t tamBuffer;

  auxRxIBus = malloc(sizeof(TRxIBus));
  auxRxIBus->uart = _uart;
  auxRxIBus->qdtCanal = _qtdCanal;

  tamBuffer = ((auxRxIBus->qdtCanal * 2) + 4) * 2; /* Armazena 2 buffers para garantir ler o pacote inteiro */

  auxRxIBus->buffer = malloc(tamBuffer * sizeof(uint8_t));
  memset(auxRxIBus->buffer, 0x00, tamBuffer * sizeof(uint8_t));

  auxRxIBus->canais = malloc(auxRxIBus->qdtCanal * sizeof(uint16_t));
  memset(auxRxIBus->canais, 0x00, auxRxIBus->qdtCanal * sizeof(uint16_t));

  return auxRxIBus;
}

void J3_IBUS_ProcessBuffer(TRxIBus* _RxIBus){
  uint8_t auxI;
  uint8_t offset_buffer;

  for(uint8_t i=0; i<32; i++){
    if (_RxIBus->buffer[i] == 0x20){
      offset_buffer = i;
      break;
    }
  }

  for(uint8_t i=0; i<_RxIBus->qdtCanal; i++){
    auxI = (i*2 + 2 + offset_buffer);
    _RxIBus->canais[i] = (_RxIBus->buffer[auxI + 1] << 8) | _RxIBus->buffer[auxI];
  }
}

uint16_t J3_IBUS_ChkSum(TRxIBus* _RxIBus){
  uint8_t tamBuffer;
  tamBuffer = (_RxIBus->qdtCanal * 2) + 4;
  return (_RxIBus->buffer[tamBuffer - 2] << 8) | _RxIBus->buffer[tamBuffer - 1];
}

bool J3_IBUS_ValidBuffer(TRxIBus* _RxIBus){
  return true;
}


uint16_t J3_IBUS_GetCh(TRxIBus* _RxIBus, uint8_t _canal){
  if (_canal == 0)
    return 0;
  if (_canal > _RxIBus->qdtCanal)
    return 0;

  return _RxIBus->canais[_canal-1];
}
