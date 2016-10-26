Para poner el robot en la arena:
Correr bobot:
  sudo lua bobot-server.lua DEBUG
Cada vez que se conecta la cámara, configurarla (hasta que se automatice el proceso ):
  guvcview -d /dev/video1
    Deshabilitar balance de blancos
    Compensación de Retroiluminacion en manual
