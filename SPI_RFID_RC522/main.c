#include "DSP28x_Project.h"
#include "sci_comm.h"

#define READ 0x80
#define WRITE 0x00

#define RC522_CS_LOW()   (GpioDataRegs.GPACLEAR.bit.GPIO19 = 1)
#define RC522_CS_HIGH()  (GpioDataRegs.GPASET.bit.GPIO19   = 1)

//Declarations
void gpio_init(void);
void spi_init(void);
void sci_init(void);
uint8_t spi_txrx(uint8_t data);
uint8_t rc522_send_cmd(uint8_t rw, uint8_t reg, uint8_t command);
static void delay_simple(uint32_t count);

//Interrupts
__interrupt void scia_tx_isr(void);
__interrupt void scia_rx_isr(void);

//Globals
ringbuff_t* log;
ringbuff_t* input_buff;

uint8_t temp;
uint8_t count;

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
    input_buff = init_buff ();
    print(log,"Starts \r");

    //Free running while
    uint8_t instr;
    count=0x00;
    while(true){
        //hex_print(log,rc522_send_cmd(READ, 0x37, 0x00)); //version
        if(read_buff(input_buff,&instr)){
            switch(instr){
             case 'R':
                    print(log,"Reset Low \r");
                    GpioDataRegs.GPACLEAR.bit.GPIO0=1;
                    delay_simple(3000000);  // ~50 ms @ 60 MHz
                    print(log,"Reset high \r");
                    GpioDataRegs.GPASET.bit.GPIO0=1;
                    break;
                case 'c':
                    print(log,"Read command reg \r");
                    temp = rc522_send_cmd(READ,0x01,0xFF);
                    //print(log,temp);
                    hex_print(log,temp); //version
                    break;

                case 'v':
                    print(log,"Version Check \r");
                    hex_print(log,rc522_send_cmd(READ, 0x37, 0xFF));
                    break;
                case 't':
                    print(log,"Self Test \r");
                    hex_print(log,rc522_send_cmd(WRITE,0x01,0x0F));//Reset
                    delay_simple(3000000);  // ~50 ms @ 60 MHz
                    hex_print(log,rc522_send_cmd(WRITE, 0x0A, 0x80)); // FIFOLevelReg = flush FIFO
                    delay_simple(10000);              // ~0.15 ms
                    //Configs?
                    hex_print(log,rc522_send_cmd(WRITE, 0x36, 0x09));//Enable auto test
                    hex_print(log,rc522_send_cmd(WRITE, 0x09, 0x00)); // FIFODataReg = 0x00
                    hex_print(log,rc522_send_cmd(WRITE, 0x01, 0x03)); // CommandReg = CalcCRC
                    delay_simple(3000000);  // ~50 ms @ 60 MHz
                    uint8_t fifo_len = rc522_send_cmd(READ, 0x0A, 0x00) & 0x7F;
                    hex_print(log, fifo_len);
                    uint8_t result[64];
                    print(log,"Data     \r");
                    volatile uint8_t i;
                    for (i=0; i < fifo_len; i++)
                    {
                        result[i] = rc522_send_cmd(READ, 0x09, 0x00); // FIFODataReg
                        hex_print(log,result[i]);
                    }

                    hex_print(log,rc522_send_cmd(WRITE, 0x01, 0x0F)); // SoftReset
                    delay_simple(3000000);  // ~50 ms @ 60 MHz
                    break;
                case 's':
                    print(log,"soft reset \r");
                    hex_print(log,rc522_send_cmd(WRITE, 0x01, 0x0F)); // SoftReset
                    break;
                case 'a':
                {
                    uint8_t val;
                    print(log,"Enable Antenna \r");
                    val = rc522_send_cmd(READ,0x14,0x00);
                    rc522_send_cmd(WRITE,0x14, 0x03);
                    hex_print(log, rc522_send_cmd(READ,0x14,0x00));
                    break;
                }
                case 'p':
                    print(log,"Power Down \r");
                    hex_print(log, rc522_send_cmd(WRITE,0x01,0x17));
                    break;
                case 'P':
                    print(log,"Power On \r");
                    hex_print(log, rc522_send_cmd(WRITE,0x01,0x07));
                    break;
                case 'b'://Debug
                {
                    print(log,"Byte \r");
                    hex_print(log,spi_txrx(count));
                    count++;
                    break;
                }
                case 'f':
                {
                rc522_send_cmd(WRITE,0x01,0x00);
                rc522_send_cmd(WRITE,0x0A,0x80);
                rc522_send_cmd(WRITE,0x04,0x7F);
                uint8_t t;
                t=rc522_send_cmd(READ,0x14,0xff);
                hex_print(log,t);
                rc522_send_cmd(WRITE,0x14,t|0x03);
                t=rc522_send_cmd(READ,0x14,0xff);
                hex_print(log,t);
                rc522_send_cmd(WRITE, 0x0A, 0x80); // Flush FIFO
                rc522_send_cmd(WRITE, 0x0D, 0x07); // BitFramingReg = 7 bits TX
                rc522_send_cmd(WRITE, 0x09, 0x26); // FIFODataReg = REQA
                rc522_send_cmd(WRITE, 0x01, 0x0C); // CommandReg = Transceive
                rc522_send_cmd(WRITE, 0x0D, 0x87); // BitFramingReg = StartSend + 7 bits
                uint8_t irq;
                do {
                    irq = rc522_send_cmd(READ, 0x04, 0xFF); // ComIrqReg
                } while ((irq & 0x30) == 0);                // RX done or Idle
                uint8_t fifo_len;
                uint8_t atqa0, atqa1;

                fifo_len = rc522_send_cmd(READ, 0x0A, 0xFF) & 0x7F; // FIFOLevelReg
                // Expected: fifo_len == 2
                hex_print(log,fifo_len);

                atqa0 = rc522_send_cmd(READ, 0x09, 0xFF); // FIFODataReg
                atqa1 = rc522_send_cmd(READ, 0x09, 0xFF);
                hex_print(log,atqa0);
                hex_print(log,atqa1);
                break;
                }
                case 'i':
                    print(log,"Initialisation \r");
                    // Soft reset
                    rc522_send_cmd(WRITE, 0x01, 0x0F);
                    delay_simple(3000000);

                    // Timer configuration
                    rc522_send_cmd(WRITE, 0x2A, 0x8D);
                    rc522_send_cmd(WRITE, 0x2B, 0x3E);
                    rc522_send_cmd(WRITE, 0x2C, 0x00);
                    rc522_send_cmd(WRITE, 0x2D, 0x00);

                    // CRC preset for ISO14443A
                    rc522_send_cmd(WRITE, 0x11, 0x3D);

                    // ASK modulation
                    rc522_send_cmd(WRITE, 0x15, 0x40);

                    // Receiver gain
                    rc522_send_cmd(WRITE, 0x26, 0x70);
                    break;
                case 'q':
                    print(log,"Request sent \r");
                    // Idle + clear FIFO
                    rc522_send_cmd(WRITE, 0x01, 0x00);
                    rc522_send_cmd(WRITE, 0x0A, 0x80);
                    rc522_send_cmd(WRITE, 0x04, 0x7F);

                    // Send REQA
                    rc522_send_cmd(WRITE, 0x0D, 0x07); // 7-bit frame
                    rc522_send_cmd(WRITE, 0x09, 0x26);
                    rc522_send_cmd(WRITE, 0x01, 0x0C);
                    rc522_send_cmd(WRITE, 0x0D, 0x87);

                    // Wait for RX
                    uint8_t irq;
                    do {
                        irq = rc522_send_cmd(READ, 0x04, 0xFF);
                    } while ((irq & 0x30) == 0);
                    break;
                case 'Q':
                {
                    print(log,"Response received \r");
                    uint8_t fifo_len;
                    uint8_t atqa0, atqa1;

                    fifo_len = rc522_send_cmd(READ, 0x0A, 0xFF) & 0x7F;
                    // fifo_len must be 2

                    atqa0 = rc522_send_cmd(READ, 0x09, 0xFF);
                    atqa1 = rc522_send_cmd(READ, 0x09, 0xFF);
                    hex_print(log,atqa0);
                    hex_print(log,atqa1);
                    break;
                }
                case 'A':
                {
                    print(log, "Anticollision\r");

                    uint8_t irq;
                    uint8_t fifo_len;
                    uint8_t uid[5];
                    uint8_t i;

                    // Go idle, clear FIFO and IRQs
                    rc522_send_cmd(WRITE, 0x01, 0x00); // Idle
                    rc522_send_cmd(WRITE, 0x0A, 0x80); // Flush FIFO
                    rc522_send_cmd(WRITE, 0x04, 0x7F); // Clear IRQs

                    // Anticollision command: 0x93 0x20
                    rc522_send_cmd(WRITE, 0x0D, 0x00); // 8-bit frame
                    rc522_send_cmd(WRITE, 0x09, 0x93);
                    rc522_send_cmd(WRITE, 0x09, 0x20);

                    rc522_send_cmd(WRITE, 0x01, 0x0C); // Transceive
                    rc522_send_cmd(WRITE, 0x0D, 0x80); // StartSend

                    // Wait for RX
                    do {
                        irq = rc522_send_cmd(READ, 0x04, 0xFF); // ComIrqReg
                    } while ((irq & 0x30) == 0);

                    // Read FIFO length
                    fifo_len = rc522_send_cmd(READ, 0x0A, 0xFF) & 0x7F;
                    hex_print(log, fifo_len); // should be 5

                    print(log, "UID\r");

                    // Read UID (4 bytes + BCC)
                    for (i = 0; i < fifo_len; i++) {
                        uid[i] = rc522_send_cmd(READ, 0x09, 0xFF);
                        hex_print(log, uid[i]);
                    }

                    break;
                }
                case 'H':
                {
                    print(log, "Halt card\r");

                    rc522_send_cmd(WRITE, 0x01, 0x00); // Idle
                    rc522_send_cmd(WRITE, 0x0A, 0x80); // Flush FIFO

                    rc522_send_cmd(WRITE, 0x09, 0x50); // HALT
                    rc522_send_cmd(WRITE, 0x09, 0x00);

                    rc522_send_cmd(WRITE, 0x01, 0x0C); // Transceive
                    rc522_send_cmd(WRITE, 0x0D, 0x80); // StartSend

                    break;
                }
                default:
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
    SpiaRegs.SPICCR.bit.SPILBK = 0;//Loopback
    SpiaRegs.SPIBRR =0x007F;//100

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
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // SCIRXDA - GPIO 28

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // RST Pin
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1; //GPIO 28 output

    //SPI specific GPIO
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;//MOSI
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;//MISO
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;//CLK
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;//CS
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1; //CS output
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

uint8_t spi_txrx(uint8_t data)
{
    uint8_t garbage;
    if(SpiaRegs.SPISTS.bit.INT_FLAG)
        garbage = (uint8_t)SpiaRegs.SPIRXBUF;
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);
    SpiaRegs.SPITXBUF=(uint16_t)data << 8;//16 bit buffs, we're only using 8 bits
    while(SpiaRegs.SPISTS.bit.INT_FLAG==0);
    return (uint8_t)(SpiaRegs.SPIRXBUF);
}

uint8_t rc522_send_cmd(uint8_t rw, uint8_t reg, uint8_t command)
{
    uint8_t r;
    RC522_CS_LOW();
    //hex_print(log,spi_txrx((rw) | ((reg&0x3F)<<1)));
    spi_txrx((rw) | ((reg&0x3F)<<1));
    r=spi_txrx(command);
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);
    RC522_CS_HIGH();
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
