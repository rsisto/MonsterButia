--require ("butialo"); setfenv(1,_G)

--wait(1)

for nombre, d in pairs(devices) do
	print (nombre)
end

--print ("global", AAAAA)

print (get_time())
wait(0.1)
print (get_time())
--print (type(asd) )
print (Pote)
--Pote=print
--print (Pote)
--asdasd=1
--print (asdasd)

local a = new_array()
--a[1]=nil
print('==',a.containing(),a.len())

for i=1, 5 do
	a[i]=i
end
for i=1, a.len() do
	print(a[i])
end

print(a.containing())
a.add(60)
print('-',a.remove_last())
print('-',a.remove_last())
for i=1, a.len() do
	print(a[i])
end
--a[2]=nil
--a[6]='aaa'
--a[10]=1
