/*
 * sci_comm.c
 *
 *  Created on: 3 Jan 2026
 *      Author: home
 */

#include "sci_comm.h"
#include <stdlib.h>

ringbuff_t* init_buff(){
    ringbuff_t *txbf;
    txbf=malloc(sizeof(ringbuff_t));
    if(txbf)
    {
        txbf->rd=0;
        txbf->wr=0;
        txbf->is_empty=1;
    }
    return txbf;
};


bool read_buff(ringbuff_t* buff,uint8_t* data){
    if(!buff->is_empty){
        *data=buff->data[buff->rd];
        buff->rd=(++buff->rd)%MAX_RINGBUFF_SIZE;
        if(buff->rd == buff->wr)
            buff->is_empty=1;
        return 1;
    }
    return 0;
}

bool write_buff(ringbuff_t* buff,uint8_t data){
    if((buff->wr == buff->rd)&&(!buff->is_empty))//buffer full
        return 0;
    else
    {
        buff->data[buff->wr]=data;
        buff->wr=(++buff->wr)%MAX_RINGBUFF_SIZE;
        buff->is_empty=0;
        if(SciaRegs.SCICTL2.bit.TXRDY)
        {
                uint8_t data;
                if(read_buff(buff,&data))
                    SciaRegs.SCITXBUF = data;
        }
        return 1;
    }
}

bool print(ringbuff_t* buff,uint8_t* string){
   while(*string!=0)
   {
       if(!write_buff(buff,*string))
           return 0;
       string++;
   }
   return 1;
}

bool hex_print(ringbuff_t* buff,uint8_t byte)
{
        if(!print(buff,"0x"))
            return 0;
        uint8_t LSB,MSB,out;
        MSB =byte & 0xF0;
        MSB=(MSB>>4);
        out = (MSB < 10) ? ('0' + MSB): ('A' + MSB - 10);
        if(!write_buff(buff,out))
            return 0;
        LSB =byte & 0x0F;
        out = (LSB < 10) ? ('0' + LSB): ('A' + LSB - 10);
        if(!write_buff(buff,out))
                    return 0;
        if(!write_buff(buff,0x0D))
                            return 0;
        return 1;
}
