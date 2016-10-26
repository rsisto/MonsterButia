class VirtualSensor:

	def __init__(self, initValue):
		self.valor = initValue
		
	def getValue(self):
		return self.valor
		
	def setValue(self, valor):
		self.valor = valor
