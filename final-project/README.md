g++ -o bin/detect src/detect.cpp -O2 -lm -lpthread -I /usr/X11R6/include -L /usr/X11R6/lib -lm -lpthread -lX11

g++ -g -o bin/detect src/detect.cpp -O2 -lm -lpthread -I /usr/X11R6/include -L /usr/X11R6/lib -lX11
g++ -o bin/detect src/detect.cpp -O2 -lm -lpthread -I /usr/X11R6/include -L /usr/X11R6/lib -lX11

1. 原图 /data/origin/*.jpg

1. 统一输入图像的大小
2. 连写不能分开的解决方法