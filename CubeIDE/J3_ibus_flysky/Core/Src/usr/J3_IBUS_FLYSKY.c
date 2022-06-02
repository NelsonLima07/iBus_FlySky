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

  auxRxIBus = malloc(sizeof(TRxIBus));
  auxRxIBus->uart = _uart;
  auxRxIBus->qdtCanal = _qtdCanal;

  auxRxIBus->buffer_size = ((auxRxIBus->qdtCanal * 2) + 4) * 2; /* Armazena 2 buffers para garantir ler o pacote inteiro */
  /* Esse + 4 �  1 byte de inicio, 2 bytes de checksum, 1 byte de finalizador  */

  auxRxIBus->buffer = malloc(auxRxIBus->buffer_size * sizeof(uint8_t));
  memset(auxRxIBus->buffer, 0x00, auxRxIBus->buffer_size * sizeof(uint8_t));

  auxRxIBus->canais = malloc(auxRxIBus->qdtCanal * sizeof(uint16_t));
  memset(auxRxIBus->canais, 0x00, auxRxIBus->qdtCanal * sizeof(uint16_t));

  /* Armazena os valores enquanto processa */
  auxRxIBus->canais_tmp = malloc(auxRxIBus->qdtCanal * sizeof(uint16_t));
  memset(auxRxIBus->canais_tmp, 0x00, auxRxIBus->qdtCanal * sizeof(uint16_t));


  return auxRxIBus;
}

void J3_IBUS_ProcessBuffer(TRxIBus* _RxIBus){
/*
 * Pacote �
 * 1 byte cabecalho 0x40
 * 2 bytes por canal
 * 2 bytes chksum
 * 1 byte finalizado 0x20
 */

  uint8_t auxI;
  uint8_t offset_buffer;
  uint16_t auxChkSum = 0xffff;
  uint16_t chkSum = 0;

  for(uint8_t i=0; i<32; i++){
    if (_RxIBus->buffer[i] == 0x20){
      offset_buffer = i;
      break;
    }
  }

  /* Subtrai logo os bytes de cabecalho e finalizacao */
  auxChkSum -= _RxIBus->buffer[offset_buffer + 1]; /* subtrair todos os bytes do pacote de 0xffff */
  auxChkSum -= 0x20;
  /*------------------------- */

  for(uint8_t i=0; i<_RxIBus->qdtCanal; i++){
    auxI = (i*2 + 2 + offset_buffer);
    _RxIBus->canais_tmp[i] = (_RxIBus->buffer[auxI + 1] << 8) | _RxIBus->buffer[auxI];
    auxChkSum -= _RxIBus->buffer[auxI];
    auxChkSum -= _RxIBus->buffer[auxI + 1];
  }

  /* Os bytes final do ChkSum */

  auxI = ((_RxIBus->qdtCanal * 2) + 2 + offset_buffer);
  chkSum = (_RxIBus->buffer[auxI + 1] << 8) | _RxIBus->buffer[auxI];

  /* Verifcar se o chksum bateu */
  if(chkSum == auxChkSum)
  {
    for(uint8_t i=0; i<_RxIBus->qdtCanal; i++)
    {
	  _RxIBus->canais[i] = _RxIBus->canais_tmp[i];
	}
  }
}

uint16_t J3_IBUS_GetCh(TRxIBus* _RxIBus, uint8_t _canal){
  if (_canal == 0)
    return 0;
  if (_canal > _RxIBus->qdtCanal)
    return 0;

  return _RxIBus->canais[_canal-1];
}
