==============================================Serial Comm library ========================================

Ejecutar make en el directorio serialcomm, esto va a generar la biblioteca de linkeo dinámico (.so) serialcomm.so
Existen bindings para diferentes para lua y python que permiten usar esta biblioteca en estos lenguajes sin nece-
sidad de dearrollar en C.

Binding LUA
-----------

En el subdirectorio lua_bindings se encuentra el binding necesario para ser utilizado en lua.
Al ejecutar make se genera la biblioteca lua_serialcomm.so.
En el directorio tests se incluyen tests que muestran como incluirla en un proyecto lua.

Para ejecutar los tests, se ingresa al directorio tests y en una shell ejecutar lua nombre_test.lua

Binding Python
--------------
En el subdirectorio python_bindings se encuentra el binding necesario para ser utilizado en python.
En construcción actualmente
