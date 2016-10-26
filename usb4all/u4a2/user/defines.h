/* Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Andres Aguirre	   27/03/07		Modificaciones Nuevo protocolo y agregados	
 ********************************************************************/

#ifndef DEFINES_H
#define DEFINES_H

#define ERROR 0xFF 

// Funciones para acceder a los punteros a los buffers de los endpoints, sustituye a USBGEN_BD_OUT USBGEN_BD_IN
#define EPBUFFERIN(ep)    (((BDT*)0x400)[2*ep+1].ADR)
#define EPBUFFEROUT(ep)   (((BDT*)0x400)[2*ep].ADR)

// Funciones para ver si un endopoint esta ocupado sustituye a mUSBGenTxIsBusy() y mUSBGenRxIsBusy()
#define EPIN_IS_BUSY(ep)  (((BDT*)0x400)[2*ep+1].Stat.UOWN)
#define EPOUT_IS_BUSY(ep) (((BDT*)0x400)[2*ep].Stat.UOWN)

// Funciones para saber la cantidad de bytes recibidos o setear la cantidad de byes enviados en un endpoint
#define EPIN_SIZE(ep)  (((BDT*)0x400)[2*ep+1].Cnt)
#define EPOUT_SIZE(ep) (((BDT*)0x400)[2*ep].Cnt)

#define EPIN_BDT(ep) ((BDT*)0x400)[2*ep+1]
#define EPOUT_BDT(ep) ((BDT*)0x400)[2*ep]

//defino endpoints usados por el modulo
//TODO o compilar antes de subir, o obtener los endpoints del admin
#define USBGEN_BD_OUT           ep1Bo
#define USBGEN_BD_IN            ep1Bi

//auxiliar para parametrizar el endpoint usado
#define mUSBGenTxIsBusy()           USBGEN_BD_IN.Stat.UOWN
#define mUSBGenRxIsBusy()           USBGEN_BD_OUT.Stat.UOWN

#define mUSBBufferReady2(bdt){                                   \
    bdt.Stat._byte &= _DTSMASK;          /* Save only DTS bit */     \
    bdt.Stat.DTS = !bdt.Stat.DTS;        /* Toggle DTS bit    */     \
    bdt.Stat._byte |= _USIE|_DTSEN;      /* Turn ownership to SIE */ \
}

/*en transferencias isoc no necesito toglear el bit DTS*/
#define mUSBBufferReady3(bdt){                                   \
    bdt.Stat._byte &= _DTSMASK;          /* Save only DTS bit */     \
    bdt.Stat._byte |= _USIE|_DTSEN;      /* Turn ownership to SIE */ \
}

//Usado para las respuestas hacia la PC
typedef enum _respType {
	ACK 	= 0x01,
	NACK	= 0x00
} respType;


#endif //DEFINES_H

