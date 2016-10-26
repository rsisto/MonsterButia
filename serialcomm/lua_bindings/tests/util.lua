module(..., package.seeall);

local DEBUG = true

function debug(a)
    if (DEBUG) then
        print(a)
    end
end

-- Convierte un array de bytes (no imprimible) a uua string (inprimible)
-- Cada byte se convierte en dos caracteres hexadecimales (se duplica el tamaño) 
function bytesToString(data)
    local data_hex = ""
    for i=1, string.len(data) do
            data_hex = data_hex .. string.format('%02X', (string.byte(data, i)))
    end
    return data_hex
end

-- Convierte un caracter hexadecimal a su valor numérico.
function charToByte(c_valua)

    if (type(c_valua) == "string") then
        c_valua = string.byte(c_valua)
    end

    local value = 0
    if (string.byte("0") <= c_valua and c_valua <= string.byte("9")) then
        value = c_valua - string.byte("0")
    else
        if (string.byte("A") <= c_valua and c_valua <= string.byte("F")) then
            value = c_valua - string.byte("A") + 10
        else
            debug("charToByte("..c_valua..") ERROR: me pasaste verdura")
        end
    end
    return value
end

-- Convierte un string (inprimible) en un arreglo de bytes (no imprimible)
-- Cada dos caracteres se interpreta un valor hexadecimal entre 0 y FF.
function stringToBytes(data)
    local data_hex = ""
    for i=1, (string.len(data) / 2) do
        local value_h = string.byte(data,i*2-1)
        local value_l = string.byte(data,i*2)
        local value = charToByte(value_h) * 0x10 + charToByte(value_l)
        data_hex = data_hex .. string.char(value)
    end
    return data_hex
end


function shiftL(data, count)
    if (count == nil) then
        count = 1
    end

    for i=1, count do
        data = string.sub(data, 2, string.len(data)) .. string.sub(data, 1, 1)
    end
    return data
end




-- Convierte un array de bytes en un número entero.
function bytesToInteger(data)
    local data_num = 0
    for i=1, string.len(data) do
        data_num = data_num * 256
        data_num = data_num + string.byte(data, i)
    end
    return data_num
end


-- hay que hacer la inversa de la función anterior
function integerToBytes(data)
    return nil
end


