#include "DSP28x_Project.h"
#include "sci_comm.h"

#define SD_CS_HIGH()   (GpioDataRegs.GPASET.bit.GPIO19 = 1)
#define SD_CS_LOW()    (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1)


//Declarations
void gpio_init(void);
void spi_init(void);
void sci_init(void);
void sd_init(void);
uint8_t spi_txrx(uint8_t data);
uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);
static void delay_simple(uint32_t count);

//Interrupts
__interrupt void scia_tx_isr(void);
__interrupt void scia_rx_isr(void);

//Globals
ringbuff_t* log;
ringbuff_t* input_buff;

int main(void)
{
    //Initialisation
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    InitPieVectTable();
    EALLOW;
    PieVectTable.SCITXINTA = &scia_tx_isr;
    PieVectTable.SCIRXINTA = &scia_rx_isr;
    EDIS;
    EINT;
    ERTM;
    gpio_init();
    spi_init();
    sci_init();
    log= init_buff();
    input_buff= init_buff();
    volatile uint32_t i;
    delay_simple(3000000);  // ~50 ms @ 60 MHz
    print(log,"Delay Complete\r");
    //Free running while
    uint8_t instr;
    while(true){
        if(read_buff(input_buff,&instr)){
                    switch(instr){
                    case 'r':
                        print(log,"Moving to SPI \r");
                        sd_init();//set cs high and wait >74 Clock cycles
                        break;
                    case '0':
                        print(log,"Cmd 0 \r");
                        hex_print(log,sd_send_cmd(0, 0, 0x95));
                        break;
                    case '1':
                        print(log,"Cmd 1 \r");
                        hex_print(log,sd_send_cmd(1, 0, 0x95));
                        break;
                    case '8':
                        print(log,"Cmd 8 \r");
                        hex_print(log,sd_send_cmd(8, 0x000001aa, 0x87));
                        break;
                    case 'a':
                        print(log,"Acmd 41 \r");
                        hex_print(log,sd_send_cmd(55, 0, 0x65));
                        hex_print(log,sd_send_cmd(69, 0x40000000, 0x65));
                        break;
                    default:
                        print(log,"Unsupported instr \r");
                        break;
                    }
        }
    }
}

void spi_init(void)
{
    //Spi Setup
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;//Disable & start reset

    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0; //Output on time
    SpiaRegs.SPICCR.bit.SPICHAR = 7; //8  bit at a time
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;// Normal clocking
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;//Master
    SpiaRegs.SPICTL.bit.TALK = 1;//Enable Transmission
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;//Do not enable interrupts
    SpiaRegs.SPIBRR =0x003F;//200kHz

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;//Enable
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

void gpio_init(void)
{
    //Gpio Pin select
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // SCITXDA
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // SCIRXDA
    //SPI specific GPIO
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;//MOSI
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;//MISO
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;//CLK
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;//GPIO19
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1; //GPIO19 output
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1; //MOSI output
    GpioCtrlRegs.GPADIR.bit.GPIO17 = 0; //MISO input
    GpioCtrlRegs.GPADIR.bit.GPIO18 = 1; //CLK output

    // Enable pull-ups (ACTIVE LOW bits!)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; // MISO pull-up ENABLE
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0; // MOSI pull-up ENABLE
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0; // SCLK pull-up ENABLE
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0; // CS pull-up ENABLE

    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;//MISO ASYNC
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;//CLK ASYNC

    EDIS;
}

void sd_init(void)
{
    SD_CS_HIGH();
    int i;
    for (i = 0; i < 100; i++)
    {
        spi_txrx(0xFF);   // 80 clocks
    }
}

uint8_t spi_txrx(uint8_t data)
{
    uint8_t garbage;
    if(SpiaRegs.SPISTS.bit.INT_FLAG)
        garbage = (uint8_t)SpiaRegs.SPIRXBUF;
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);//Waiting for TX to complete
    SpiaRegs.SPITXBUF=data<<8;//16 bit buffs, we're only using 8 bits; TX data is on the MSB
    while(SpiaRegs.SPISTS.bit.INT_FLAG==0);//Waiting till RX is received
    return (uint8_t)(SpiaRegs.SPIRXBUF);//LSB is received
}

uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r;
    uint16_t i;

    SD_CS_LOW();
    delay_simple(600000);  // ~10 ms @ 60 MHz
    spi_txrx((uint8_t)(0x40 | cmd));

    spi_txrx((uint8_t)(arg >> 24));
    spi_txrx((uint8_t)(arg >> 16));
    spi_txrx((uint8_t)(arg >> 8));
    spi_txrx((uint8_t)arg);
    spi_txrx((uint8_t)crc);

    // Wait for response (MSB cleared)
    for (i = 0; i < 8; i++) {
        r = spi_txrx((uint8_t)0xFF);
        hex_print(log,r);
//        if (r != 0xFF)
//            break;
    }
    delay_simple(600000);  // ~10 ms @ 60 MHz
    SD_CS_HIGH();
    //spi_txrx(0xFF);
    return r;
}

__interrupt void scia_tx_isr(void)
{
    uint8_t data;
    if(read_buff(log,&data))
        SciaRegs.SCITXBUF = data;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

__interrupt void scia_rx_isr(void)
{
    uint8_t data=SciaRegs.SCIRXBUF.all;
    write_buff(input_buff,data,true);
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

static void delay_simple(uint32_t count)
{
    while(count--) asm(" NOP");
}
