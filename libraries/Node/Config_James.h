const int CONFIG = 1; //0 for enCORE, 1 for James's

//Other constants
const int HUB_NUM = 1;
const XBeeAddress64 HUB_ADDR = XBeeAddress64(0,0);
const int threshCT = 100;
const unsigned long tVentWait = 500;
const unsigned long tWaitSend = 50; //SHOULD NOT NEED
const char server[] = "mesh.org.ohio-state.edu";

//XBee addresses
const XBeeAddress64 addr2 = XBeeAddress64(0x0013A200,0x40ABB7F7);
const XBeeAddress64 addr3 = XBeeAddress64(0x0013A200,0x40AEB88F); 
const XBeeAddress64 addr4 = XBeeAddress64(0x0013A200,0x40AEBA2C);
const XBeeAddress64 addr5 = XBeeAddress64(0x0013A200,0x40AEB9AA);
const XBeeAddress64 addr6 = XBeeAddress64(0x0013A200,0x40AEB9C3); 

//XBee pinout
const int pLDR1 = 0;
const int pHUM = 1;
const int pTEMP = 2;
const int pLDR2 = 3;
const int pPIR = 4;

//Arduino IO pinout
//analog
const int pTEMPh = 15;
const int pHUMh = 0;
const int pLDR1h = 2;
const int pLDR2h = 3;
const int pCAL = 8; //to be used with setEqual() routine
const int pCT = 4;

//digital
const int pPIRh = 2;

//1a=main_hallway=blue, 1b=front_bath=brown, 2=front_bed=orange, 3=office=green
const int pVentPos[] = {22, 24, 28, 26, 30, 32}; //solid colors
const int pVentNeg[] = {23, 25, 29, 27, 31, 33}; //striped colors
const int pVentEnable[] = {36, 37, 39, 40, 38, 41}; //needed to power h-bridge to enable switching
const int pHeaters[] = {};

