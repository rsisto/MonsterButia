class Behavior:

  priority = 0
  behaviorName = "Not set"
  #Variable para determinar si debe correr hilo en comportamientos que lo tengan.
  shouldRun = True
  
  def __init__(self):
    pass

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
    pass

  def getName(self):
    return self.behaviorName
  
  def setShouldRun(self,var):
    self.shouldRun = var
  
