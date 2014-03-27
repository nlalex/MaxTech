const int CONFIG = 1; //0 for enCORE, 1 for James's

//Other constants
const int HUB_NUM = 1;
const XBeeAddress64 HUB_ADDR = XBeeAddress64(0,0);

//XBee addresses
XBeeAddress64 addr2 = XBeeAddress64(0x0013A200,0x40ABB9A8);
XBeeAddress64 addr3 = XBeeAddress64(0x0013A200,0x40ABB9DE);
XBeeAddress64 addr4 = XBeeAddress64(0x0013A200,0x40AD57DA);
XBeeAddress64 addr5 = XBeeAddress64(0x0013A200,0x40ABAE96);
XBeeAddress64 addr6 = XBeeAddress64(0x0013A200,0x40ABBB6C);

//XBee pinout
const int pLDR1 = 0;
const int pHUM = 1;
const int pTEMP = 2;
const int pLDR2 = 3;
const int pPIR = 4;

//Arduino IO pinout
//digital
const int pTEMPh = 1;
const int pHUMh = 0;
const int pLDR1h = 2;
const int pLDR2h = 3;
const int pCAL = 8; //to be used with setEqual() routine
const int pVentHigh[] = {0, 1, 2, 3, 4, 5};
const int pVentLow[] = {0, 1, 2, 3, 4, 5};
const int pHeaters[] = {};
//analog
const int pPIRh = 2;
