gst-launch v4l2src ! video/x-raw-yuv,width=320,height=240,framerate=\(fraction\)5/1 ! ffmpegcolorspace ! jpegenc ! multipartmux ! tcpserversink host=192.168.2.249 port=5000
