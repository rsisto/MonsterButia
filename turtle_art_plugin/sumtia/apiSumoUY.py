#VERSION 1.0
import socket
    
class apiSumoUY:
    cliente = None
    
    #default comunication parameters
    port_cliente = 7001
    ip_server = "127.0.0.1"
    port_server = 8001
    LARGO_MAX_MSG = 100

    #atributos utilizados para intercambiar mensajes UDP con el servidor
    #private DatagramSocket socket;
    #private InetAddress serverIP;
    #private int serverPort;

    #posibles tipos de mensajes entre el servidor y el cliente
    OPE_UPDATE = "update"
    OPE_REPOSICIONAR = "position"
    OPE_START = "start"
    OPE_STOP = "stop"
    OPE_ACK = "ok"

    #estados del juego
    REPOSICIONAR = 0
    START = 1
    STOP = 2

    #estado actual del juego
    estado = STOP

    #posicion y puntaje de mi luchador
    coorX = -1
    coorY = -1
    rot = -1
    yukoP = 0

    #posicion y puntaje del contrincante
    coorXOp = -1
    coorYOp = -1
    rotOp = -1
    yukoPOp = 0

    #posicion en donde debe posicionarse mi luchador
    coorXR = -1
    coorYR = -1
    rotR = -1
       
    def __init__(self):
        pass
    
    def setPuertos(self, port_Cliente = 7001, ip_Server = "127.0.0.1", port_Server = 8001):
        
        self.port_cliente = port_Cliente
        self.ip_server = ip_Server
        self.port_server = port_Server
        
        print "SumoAPI: Puertos Seleccionados:"
        print self.port_cliente
        print self.ip_server
        print self.port_server    
    
    def conectarse(self):
        try:
            self.cliente = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.cliente.bind((self.ip_server, self.port_cliente)) #puerto por donde voy a mandar mis comandos
        except:
            print "SumoAPI: Error trying to connect..."    
            #return -1        
        
    def liberarRecursos(self):
        #print "cerrando comunicacion..."    
        try:
            self.cliente.close() 
        except:
            print "SumoAPI: Closing connection error..."    
        
    def enviarAck(self):
        msg = self.OPE_ACK + '*'
        try:
            err = self.cliente.sendto(msg.encode("ascii"),(self.ip_server, self.port_server))
        except:
            print "SumoAPI: Sending Ack error..."    
        
    #listar modulos: devuelve la lista de los modulos disponibles en el firmware de la placa
    def enviarVelocidades(self, vel_izq = 0, vel_der = 0):
        
        #print "enviar velocidades\n"
        msg = "speed*" + str(int(vel_izq)) + "*" + str(int(vel_der)) + "*" 
        print msg
        try:
            err = self.cliente.sendto(msg,(self.ip_server, self.port_server)) #verificar que es lo q devuelve la funcion
            return "ok"    
        except:    
            print "SumoAPI: Sending speed error..."
        #print err
        #print msg
        
        
    def getInformacion(self):
        print "SumoAPI: get Information\n"
        try:
            mensaje, addr = self.cliente.recvfrom(1024)
            print mensaje
            #print addr        
            data = mensaje.split('*')
            opcode = data[0]
            if opcode == self.OPE_REPOSICIONAR:
                if (len(data) >= 4): 
                    #print "me llego un reposicionar"
                    self.coorXR = int(data[1])
                    self.coorYR = int(data[2])
                    self.rotR   = int(data[3])
                    self.estado = self.REPOSICIONAR
                    self.enviarAck()
            elif opcode == self.OPE_START:
                #print "me llego un start"
                self.estado = self.START
                self.enviarAck()            
            elif opcode == self.OPE_STOP:
                #print "me llego un stop"
                self.estado = self.STOP
                self.enviarAck()
            elif opcode == self.OPE_UPDATE:
                if (len(data) >= 9): 
                    #print "me llego un update"
                    self.coorX = int(data[1])
                    self.coorY = int(data[2])
                    self.rot   = int(data[3])
                    self.yukoP = int(data[4])
                    self.coorXOp = int(data[5])
                    self.coorYOp = int(data[6])
                    self.rotOp   = int(data[7])
                    self.yukoPOp = int(data[8])
            
            mensaje = None
            return 0
        except:
            print "SumoAPI: Getting information error..."
            return -1        
        
    #@return coordenada X de luchador
    def getCoorX(self):
        return self.coorX

    #@return coordenada X de luchador oponente
    def getCoorXOp(self):
        return self.coorXOp  

    #@return coordenada X donde debe posicionarse el luchador
    def getCoorXR(self):
        return self.coorXR
   
    #@return coordenada Y de luchador
    def getCoorY(self):
        return self.coorY
    
    #@return coordenada Y de luchador oponente
    def getCoorYOp(self):
        return self.coorYOp
    
    #@return coordenada Y donde debe posicionarse el luchador
    def getCoorYR(self):
        return self.coorYR

    #@return estado actual del juego
    def getEstado(self):
        return self.estado
      
    #@return rotacion del luchador
    def getRot(self):
        return self.rot

    #@return rotacion del luchador oponente
    def getRotOp(self):
        return self.rotOp
    
    #@return rotacion para reposicionar el luchador
    def getRotR(self):
        return self.rotR
    
    #@return puntos yuko del luchador
    def getYukoP(self):
        return self.yukoP
  
    #@return puntos yuko del luchador oponente
    def getYukoPOp(self):
        return self.yukoPOp     
    
