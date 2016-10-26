print ("Starting")
local function p1(v)
 print (">",v)
end 
local function p2(v)
 print ("<",v)
end 
local function p3(v)
 print ("!",v)
 events.stop()
end 
print(p1,p2)
events.add( Pote.get_pote, ">", 200,  p1, 50, 'eventitor1' )
events.add( Pote.get_pote, "<", 200,  p2)
events.add( Pote.get_pote, "<", 2,  p3 )
events.go()
print("Fin!")
