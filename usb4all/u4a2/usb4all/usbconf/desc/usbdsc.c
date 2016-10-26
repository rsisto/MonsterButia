/** I N C L U D E S *************************************************/
#include "system\typedefs.h"
#include "system\usb\usb.h"
/** C O N S T A N T S ************************************************/
#pragma romdata _device_dsc

//	 Device Descriptor 
rom USB_DEV_DSC device_dsc=
{
    sizeof(USB_DEV_DSC),    // Size of this descriptor in bytes
    DSC_DEV,                // DEVICE descriptor type
    0x0200,				// USB Spec Release Number in BCD format
    0x0,				// Class Code
    0x0,				// Subclass code
    0x0,				// Protocol code
    EP0_BUFF_SIZE,				// Max packet size for EP0, see usbcfg.h
    0x4d8,				// Vendor ID
    0xc,				// Product ID: PICDEM FS USB (DEMO Mode)
    0x0,				// Device release number in BCD format
    0x1,				// Manufacturer string index
    0x2,				// Product string index
    0x3,				// Device serial number string index
    0x1,				// Number of possible configurations
};
//		 Configuration 1 Descriptor
CFG01=
{
    // Configuration Descriptor
    sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    DSC_CFG,                // CONFIGURATION descriptor type
    sizeof(cfg01),          // Total length of data for this cfg
1,                      // Number of interfaces in this cfg
1,                      // Index value of this configuration
0,                      // Configuration string index
_DEFAULT,               // Attributes, see usbdefs_std_dsc.h
250,                    // Max power consumption (2X mA)
// Interface Descriptor
sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
DSC_INTF,               // INTERFACE descriptor type
0,                      // Interface Number
0,                      // Alternate Setting Number
6,                      // Number of endpoints in this intf
0x0,                   // Class code
0x0,                   // Subclass code
0x0,                   // Protocol code
0,                      // Interface string index

//				 Endpoint Descriptors
sizeof(USB_EP_DSC),DSC_EP,_EP01_OUT,_BULK,64,255,
sizeof(USB_EP_DSC),DSC_EP,_EP01_IN,_BULK,64,255,
sizeof(USB_EP_DSC),DSC_EP,_EP02_OUT,_BULK,64,255,
sizeof(USB_EP_DSC),DSC_EP,_EP02_IN,_BULK,64,255,
sizeof(USB_EP_DSC),DSC_EP,_EP03_OUT,_ISO,64,6,
sizeof(USB_EP_DSC),DSC_EP,_EP03_IN,_ISO,64,6};
rom struct{byte bLength;byte bDscType;word string[1];}sd000={
sizeof(sd000),DSC_STR,0x409};
rom struct{byte bLength;byte bDscType;word string[24];}sd001={
sizeof(sd001),DSC_STR,'A','g','u','i','r','r','e',' ','F','e','r','n','a','n','d','e','z',' ','G','r','o','s','s','y'};
rom struct{byte bLength;byte bDscType;word string[20];}sd002={
sizeof(sd002),DSC_STR,'U','S','B','4','a','l','l',' ','(','C',')','2','0','0','7',' ','F','I','N','G'};
rom struct{byte bLength;byte bDscType;word string[8];}sd003={
sizeof(sd003),DSC_STR,'0','0','0','0','0','0','0','2'};
//dejo estos punteros en una posicion fija de memoria
#pragma romdata _usb_cd_ptr
rom const unsigned char *rom USB_CD_Ptr[]={(rom char*)&cfg01,(rom char*)&cfg01};
#pragma romdata _usb_sd_ptr
rom const unsigned char *rom USB_SD_Ptr[]={(rom char*)&sd000,(rom char*)&sd001,(rom char*)&sd002,(rom char*)&sd003};
#pragma code sys
