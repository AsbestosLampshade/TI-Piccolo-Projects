#include "DSP28x_Project.h"
#include "sci_comm.h"

void sci_init(void);
__interrupt void scia_tx_isr(void);
__interrupt void scia_rx_isr(void);

//Enable SPI buff
ringbuff_t* log;
ringbuff_t* input;
uint16_t counter;

volatile uint32_t delay_count;

int main(void)
{
    //Initialisation
    InitSysCtrl();
    //Gpio Pin select
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // SCITXDA
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // SCIRXDA
    EDIS;

    DINT;
    InitPieCtrl();
    InitPieVectTable();
    EALLOW;
    PieVectTable.SCITXINTA = &scia_tx_isr;
    PieVectTable.SCIRXINTA = &scia_rx_isr;
    EDIS;
    EINT;
    ERTM;
    sci_init();
    log= init_buff();
    input= init_buff();
    print(log,"Alfred");
    print(log,"Alfred2");
    print(log,"Alfred4");
    counter=0;
    while(true){
//        uint32_t i;
//        print(log,"Alfred Catherine Resmi\r");
//
//        for(i=0;i<100000;i++){
//            delay_count++;
//        }
        uint8_t data;
        if(read_buff(input,&data))
            write_buff(log,data);
    }
}

void sci_init(void)
{
    //Scia enable
    SciaRegs.SCICTL1.bit.SWRESET = 0;//Reset bit

    SciaRegs.SCICCR.bit.STOPBITS = 0;
    SciaRegs.SCICCR.bit.PARITYENA = 0;
    SciaRegs.SCICCR.bit.SCICHAR = 7;//8 chars
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCIHBAUD = 0x0000;
    SciaRegs.SCILBAUD = 0x00C2;//9600

    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA =1;
    SciaRegs.SCICTL1.bit.SWRESET = 1;//Set bit
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1; // TX
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1; // RX
    IER |= M_INT9;
}

__interrupt void scia_tx_isr(void)
{
    uint8_t data;
    counter++;
    if(read_buff(log,&data))
        SciaRegs.SCITXBUF = data;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

__interrupt void scia_rx_isr(void)
{
    uint8_t data=SciaRegs.SCIRXBUF.all;
    write_buff(input,data);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
