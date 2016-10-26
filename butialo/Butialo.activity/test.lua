for nombre, d in pairs(devices) do
	print ("Dispositivo:", nombre)
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

