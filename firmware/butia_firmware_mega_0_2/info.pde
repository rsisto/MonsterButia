/* PROBLEMAS:

1) lo mueve desparejo
2) no recibe algunos mensajes
3) uno de los motores pira
4) gestionar el ACK y el NACK
5) ya está claro cómo van a ser las respuestas: el handler 0 es el sistema. cuando contesta, el handler es el id propio y no el destino
   * en caso de error en el mensaje, etc., el que contesta es el sistema, y lo hace con un mensaje de 1 byte de data
   * si está todo bien, el que contesta es el handler, con un mensaje de 1 byte si es sólo para decir "OK" o más si tiene algo más para decir
6) hay que ir gestionando lo del hardware
7) implementar una manera de que los módulos se "registren" en la loop, para que ejecuten tareas periódicamente (polling). Por ejemplo un led que parpadea, o un moving average filter

*****************************************************************************************************************************************************************************

  decodificación de error (AX-12):
    
    error interno:
      Bit 0 = Error de Cabecera (falta byte de cabecera = 255)
      Bit 1 = Error de Coherencia (el largo del mensaje difiere de lo esperado)
      Bit 2 = Error de Checksum (el checksum es incorrecto)

    status_error:
      Bit 6 = Instruction Error (undefined instruction is sent or an action instruction is sent without a Reg_Write instruction)
      Bit 5 = Overload Error (the specified maximum torque can't control the applied load)
      Bit 4 = Checksum Error (the checksum of the instruction packet is incorrect)
      Bit 3 = Range Error (the instruction sent is out of the defined range)
      Bit 2 = Overheating Error (the internal temperature of the Dynamixel unit is above the operating temperature range as defined in the control table)
      Bit 1 = Angle Limit Error (the Goal Position is set outside of the range between CW Angle Limit and CCW Angle Limit)
      Bit 0 = Input Voltage Error (the voltage is out of the operating voltage range as defined in the control table)

  
  nombres de las variables del AX12

  EEPROM:  
       MODEL_NUMBER
       VERSION 
       ID
       BAUD_RATE
       RETURN_DELAY_TIME
       CW_ANGLE_LIMIT
       CCW_ANGLE_LIMIT
       LIMIT_TEMPERATURE
       DOWN_LIMIT_VOLTAGE
       UP_LIMIT_VOLTAGE
       MAX_TORQUE
       RETURN_LEVEL
       ALARM_LED
       ALARM_SHUTDOWN
       DOWN_CALIBRATION
       UP_CALIBRATION

  RAM:
       TORQUE_ENABLE
       AX_LED
       CW_COMPLIANCE_MARGIN
       CCW_COMPLIANCE_MARGIN
       CW_COMPLIANCE_SLOPE
       CCW_COMPLIANCE_SLOPE
       GOAL_POSITION
       MOVING_SPEED 
       TORQUE_LIMIT 
       PRESENT_POSITION
       PRESENT_SPEED   
       PRESENT_LOAD    
       PRESENT_VOLTAGE 
       PRESENT_TEMPERATURE 
       REGISTERED_INSTRUCTION
       MOVING
       LOCK  
       PUNCH 


PINES I/O 
 
	Desc.           Cn.0	Cn.1	Cn.2	Cn.3	Cn.4	Cn.5	Cn.6	Cn.7
Pin 1 – GND	        gnd	gnd	gnd	gnd	gnd	gnd	gnd	gnd
Pin 2 – ID0	        d42	d44	d46	d48	d50	d52	d22	d24
Pin 3 – ID1	        d43	d45	d47	d49	d51	d53	d23	d25
Pin 4 – D0 (pwm)	        d04	d05	d06	d07	d08	d09	d10	d11
Pin 5 – D1	        d26	d27	d28	d29	d30	d31	d32	d33
Pin 6 – +5V	        +5V	+5V	+5V	+5V	+5V	+5V	+5V	+5V
Pin 7 – Analog	        A0	A1	A2	A3	A4	A5	A6	A7
Pin 8 – I2C SDA(20)	SDA	SDA	SDA	SDA	SDA	SDA	SDA	SDA
Pin 9 – I2C SCL(21)	SCL	SCL	SCL	SCL	SCL	SCL	SCL	SCL
								
L293D
							
IN 1	    d34							
IN 2	    d35							
IN 3	    d36							
IN 4	    d37							
Enable 1    d03 (pwm)							
Enable 2    d02 (pwm)							

*/


