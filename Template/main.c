#include "DSP28x_Project.h"

__interrupt void cpu_timer0_isr(void);
__interrupt void scia_tx_isr(void);

//Globals
uint16_t data = 0x01;

int main(void)
{
    InitSysCtrl();

    //Gpio Pin select
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // SCITXDA
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // SCIRXDA
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
    EDIS;

    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 60, 1000000);
    //CpuTimer0Regs.TCR.bit.TIE = 1; - not reqd
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

    //SCI FIFO enable - Do I really need the FIFO?
//    SciaRegs.SCIFFTX.all = 0xE040;
//    SciaRegs.SCIFFRX.all = 0x2044;
//    SciaRegs.SCIFFCT.all = 0x0;

    //Sci Setup
    SciaRegs.SCIHBAUD = 0x0000;
    SciaRegs.SCILBAUD = 0x00C2;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL1.bit.SWRESET = 1;//Set bit
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1; // TX
    IER |= M_INT9;

    SciaRegs.SCITXBUF = 0x47;
	while(1);
}

__interrupt void cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void scia_tx_isr(void)
{
    SciaRegs.SCITXBUF = data;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}
