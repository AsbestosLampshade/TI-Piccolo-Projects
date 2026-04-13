#ifndef SCI_COMM_H
#define SCI_COMM_H

#include "DSP28x_Project.h"

#define MAX_RINGBUFF_SIZE 512
#define RINGBUFF_EMPTY_INDEX (MAX_RINGBUFF_SIZE+1)

typedef struct
{
	uint8_t data[MAX_RINGBUFF_SIZE];
	uint8_t rd;//Read from here
	uint8_t wr;//Write to here
	bool is_empty;
}ringbuff_t;

ringbuff_t* init_buff();

bool read_buff(ringbuff_t* buff,uint8_t* data);

bool write_buff(ringbuff_t* buff,uint8_t data, bool consume);

bool print(ringbuff_t* buff,uint8_t* string);

bool hex_print(ringbuff_t* buff,uint8_t byte);

#endif
