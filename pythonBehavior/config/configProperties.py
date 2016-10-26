import ConfigParser,os

class ConfigProperties:
	propertiesFileName=os.path.abspath(os.path.join(os.path.dirname(__file__), 'config.properties'))
	config = ConfigParser.ConfigParser()
	config.readfp(open(propertiesFileName))
	
	def getPropertie(self, section, name):
		return self.config.get(section, name)
