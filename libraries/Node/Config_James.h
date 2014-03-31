const int CONFIG = 1; //0 for enCORE, 1 for James's

//Other constants
const int HUB_NUM = 1;
const XBeeAddress64 HUB_ADDR = XBeeAddress64(0,0);
const int heatThreshold = 300;
const char server[] = "mesh.org.ohio-state.edu";

//XBee addresses
const XBeeAddress64 nAddr2 = XBeeAddress64(0x0013A200,0x40ABB7F7); //
const XBeeAddress64 nAddr3 = XBeeAddress64(0x0013A200,0x40AEB88F); 
const XBeeAddress64 nAddr4 = XBeeAddress64(0x0013A200,0x40AEBA2C);
const XBeeAddress64 nAddr5 = XBeeAddress64(0x0013A200,0x40AEB9AA);
const XBeeAddress64 nAddr6 = XBeeAddress64(0x0013A200,0x40AEB9C3); 

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
const int pVentPos[] = {22, 24, 26, 28, 30, 32};
const int pVentNeg[] = {23, 25, 27, 29, 31, 33};
const int pHeaters[] = {};
//analog
const int pPIRh = 2;
const int pHEAT = 5;
