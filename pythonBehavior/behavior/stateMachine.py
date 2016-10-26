import behavior

class StateMachine(behavior.Behavior):
  
  
  def __init__(self):
    behavior.Behavior.__init__(self)
    self.behaviors = {}
    
  def reset(self):
    for behavior in self.behaviors:
        self.behaviors[behavior].reset()
    self.state = self.INITIAL_STATE

  def behave(self):
    self.doTransition()
    self.behaviors[self.state].behave()

  def setState(self, state):
    self.behaviors[self.state].reset()
    self.state = state

  def doTransition(self):
    pass
    
  def setShouldRun(self,var):
    self.shouldRun = var
    for behavior in self.behaviors:
        self.behaviors[behavior].setShouldRun(var)
