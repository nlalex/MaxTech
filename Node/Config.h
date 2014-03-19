//Other constants
const unsigned long SEND_TIME = 5100; //wait time between sending, in milliseconds
const int HUB_NUM = 1;
const XBeeAddress64 HUB_ADDR = XBeeAddress64(0,0);

//XBee pinout
const int pLDR1 = 0;
const int pHUM = 1;
const int pTEMP = 2;
const int pLDR2 = 3;
const int pPIR = 4;

//Arduino IO pinout
const int pTEMPh = 0;
const int pHUMh = 1;
const int pLDR1h = 2;
const int pLDR2h = 3;
const int pCAL = 4; //to be used with setEqual() routine

const int pPIRh = 0;
