for nombre, d in pairs(devices) do
	print ("Dispositivo:", nombre)
end

for k, v in pairs(Button_3) do
	print ("Button_3:", k, v)
end

while true do
	print("Leyendo:", Button_3.getValue())
	util.wait(0.5)
end


if devices.Lback then 
	local mensaje="oioioi!"
	print("Enviando:", mensaje)
	Lback.send(mensaje)
	print("Leyendo:", Lback.read())
	util.wait(1)
else
	print ("No hay lback, no podemos probar comunicaciones")
end

