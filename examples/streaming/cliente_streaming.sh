gst-launch tcpclientsrc host=192.168.2.2 port=5000 ! multipartdemux ! jpegdec ! autovideosink
