#include "DSP28x_Project.h"

__interrupt void cpu_timer0_isr(void);
__interrupt void scia_tx_isr(void);
__interrupt void spia_rx_isr(void);
__interrupt void spia_tx_isr(void);


//Globals
volatile uint16_t data_sci = 0x01;
volatile uint16_t data_spi_out = 0x00;
volatile uint16_t data_spi_in = 0x00;

int main(void)
{
    InitSysCtrl();

    //Gpio Pin select
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // SCITXDA
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // SCIRXDA
    //SPI specific GPIO
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;//MOSI
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;//MISO
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;//CLK
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1;//STEA
    //GpioCtrlRegs.GPADIR.bit.GPIO19 = 1; //STEA output
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1; //MOSI output
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 0; //MISO input
    GpioCtrlRegs.GPADIR.bit.GPIO18 = 1; //CLK output
    EDIS;

    DINT;
    InitPieCtrl();
    //IER = 0x00;
    //IFR = 0x00;
    InitPieVectTable();

    //Bind ISR to Vect Table
    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;
    PieVectTable.SCITXINTA = &scia_tx_isr;
    PieVectTable.SPIRXINTA = &spia_rx_isr;
    PieVectTable.SPITXINTA = &spia_tx_isr;
    EDIS;

    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 60, 1000000);
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; //Timer0
    IER |= M_INT1;
    EINT;
    ERTM;
    // Start Timer0
    CpuTimer0Regs.TCR.bit.TSS = 0;

    //Scia enable
    SciaRegs.SCICTL1.bit.SWRESET = 0;//Reset bit
    SciaRegs.SCICCR.bit.STOPBITS = 0;
    SciaRegs.SCICCR.bit.PARITYENA = 0;
    SciaRegs.SCICCR.bit.SCICHAR = 7;//8 chars
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCIHBAUD = 0x0000;
    SciaRegs.SCILBAUD = 0x00C2;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL1.bit.SWRESET = 1;//Set bit
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1; // TX
    IER |= M_INT9;


    //Spi Setup
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;//Disable & start reset
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0; //Output on time
    SpiaRegs.SPICCR.bit.SPICHAR = 7; //8  bit at a time
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;// Normal clocking
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;//Master
    SpiaRegs.SPICTL.bit.TALK = 1;//Enable Transmission
    SpiaRegs.SPICTL.bit.SPIINTENA = 1;//Enable interrupts
    SpiaRegs.SPIBRR =0x007F;
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;//Enable
    PieCtrlRegs.PIEIER6.bit.INTx1 = 1;//RX
    PieCtrlRegs.PIEIER6.bit.INTx2 = 1;//TX
    IER |= M_INT6;

    //Runtime
    SciaRegs.SCITXBUF = 0x47;//Initiate the routine
    SpiaRegs.SPITXBUF = (data_spi_out << 8);//Initiate the routine
	while(1);
}

__interrupt void cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;
    SpiaRegs.SPITXBUF = (data_spi_out << 8);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    data_spi_out++;
}

__interrupt void scia_tx_isr(void)
{
    if(data_sci!=data_spi_in){
        SciaRegs.SCITXBUF = data_spi_in;
        data_sci=data_spi_in;
    }
    else
        SciaRegs.SCITXBUF = 0x00;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

__interrupt void spia_tx_isr(void)
{
    SpiaRegs.SPITXBUF = (data_spi_out << 8);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
}


__interrupt void spia_rx_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;
    data_spi_in = (SpiaRegs.SPIRXBUF) & 0xFF;
}
