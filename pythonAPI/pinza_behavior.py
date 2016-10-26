import butiaAPI, behavior, time


PINZA_IZQ_CHASIS = "23"
PINZA_IZQ_PZA = "24"
PINZA_DER_CHASIS = "21"
PINZA_DER_PZA = "22"
OPEN_DER = 340
CLOSE_DER = 666
ALT_MAX_DER = 375
#ALT_MIN_DER = 823
ALT_MIN_DER = 805
OPEN_IZQ = 871
CLOSE_IZQ = 511
ALT_MAX_IZQ = 73
#ALT_MIN_IZQ = 488
ALT_MIN_IZQ = 463
INC = 10
PAUSA = 0.1

class PickLata(behavior.Behavior):

	priority = 0
	behaviorName = "PickLata"
	posDerChasis = ALT_MAX_DER
	posDerPinza = OPEN_DER
	posIzqChasis = ALT_MAX_IZQ
	posIzqPinza = OPEN_IZQ
	butiabot = None
	
	def __init__(self):
		self.butiabot = butiaAPI.robot()
		self.butiabot.joint_mode(PINZA_IZQ_CHASIS, "0", "1023") #1023 is 300 degrees
		self.butiabot.joint_mode(PINZA_IZQ_PZA, "0", "1023") #1023 is 300 degrees
		self.butiabot.joint_mode(PINZA_DER_CHASIS, "0", "1023") #1023 is 300 degrees
		self.butiabot.joint_mode(PINZA_DER_PZA, "0", "1023") #1023 is 300 degrees

		self.butiabot.set_position(PINZA_DER_CHASIS, str(ALT_MAX_DER))
		self.butiabot.set_position(PINZA_DER_PZA, str(OPEN_DER))
		self.butiabot.set_position(PINZA_IZQ_CHASIS, str(ALT_MAX_IZQ))
		self.butiabot.set_position(PINZA_IZQ_PZA, str(OPEN_IZQ))

		self.posDerChasis = ALT_MAX_DER
		self.posDerPinza = OPEN_DER
		self.posIzqChasis = ALT_MAX_IZQ
		self.posIzqPinza = OPEN_IZQ

				
	def reset(self):
		pass
		
	def getPriority(self):
		return self.priority

	def setPriority(self, priority):
		self.priority = priority

	def canActivate(self):
		return True
	
	def finish(self):
		pass
	
	def behave(self):
		self.perceptualSchema()
		self.motorSchema()
		
	def perceptualSchema(self):
		pass
		
	def motorSchema(self):	
		#bajo pinza	
		while self.posDerChasis<ALT_MIN_DER and self.posIzqChasis<ALT_MIN_IZQ:	
			self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))
			self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
			if self.posDerChasis<ALT_MIN_DER:
				self.posDerChasis+=INC;
			if self.posIzqChasis<ALT_MIN_IZQ:
				self.posIzqChasis+=INC;
			time.sleep(PAUSA)
		#cierro
		while self.posDerPinza <CLOSE_DER and self.posIzqPinza > CLOSE_IZQ:	
			self.butiabot.set_position(PINZA_DER_PZA, str(self.posDerPinza))
			self.butiabot.set_position(PINZA_IZQ_PZA, str(self.posIzqPinza))
			if self.posDerPinza <CLOSE_DER:
				self.posDerPinza+=INC;
			if self.posIzqPinza > CLOSE_IZQ:
				self.posIzqPinza-=INC;
			time.sleep(PAUSA)
		#subo pinza	
		while self.posDerChasis>ALT_MAX_DER and self.posIzqChasis>ALT_MAX_IZQ:	
			self.butiabot.set_position(PINZA_DER_CHASIS, str(self.posDerChasis))
			self.butiabot.set_position(PINZA_IZQ_CHASIS, str(self.posIzqChasis))
			if self.posDerChasis>ALT_MAX_DER:
				self.posDerChasis-=INC;
			if self.posIzqChasis>ALT_MAX_IZQ:
				self.posIzqChasis-=INC;
			time.sleep(PAUSA)
		time.sleep(PAUSA)
		#abro
		while self.posDerPinza >OPEN_DER and self.posIzqPinza < OPEN_IZQ :	
			self.butiabot.set_position(PINZA_DER_PZA, str(self.posDerPinza))
			self.butiabot.set_position(PINZA_IZQ_PZA, str(self.posIzqPinza))
			if self.posDerPinza >OPEN_DER:
				self.posDerPinza-=INC
			if self.posIzqPinza < OPEN_IZQ:
				 self.posIzqPinza+=INC
			time.sleep(PAUSA)
					

	def getName(self):
		return self.behaviorName
