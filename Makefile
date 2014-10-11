
CC = i686-pc-mingw32-gcc
CXX = i686-pc-mingw32-g++
# CC = gcc
# CXX = g++

TARGET = face_lena face_lena_2 clip face_lena_2_clip multi_face_recognition_1

INCLUDE = -I/home/tako/progs/local/include/ -I/home/tako/progs/local/include/opencv/
# LIBPATH = -static-libgcc -static -L/home/tako/facial_recognition/vc_static/ /home/tako/facial_recognition/vc_static/opencv_core243.lib /home/tako/facial_recognition/vc_static/opencv_highgui243.lib /home/tako/facial_recognition/vc_static/opencv_objdetect243.lib /home/tako/facial_recognition/vc_static/opencv_imgproc243.lib
# http://mingw.5.n7.nabble.com/using-VC-lib-with-mingw-td19148.html
# That distinction aside, the library itself should be named either libxml2.a or xml2.lib, but *not* libxml2.lib.  If it is appropriately 
LIBPATH = -static-libgcc -static -L/home/tako/facial_recognition/mingw/lib/ -lopencv_core243.dll -lopencv_highgui243.dll -lopencv_objdetect243.dll -lopencv_imgproc243.dll
# http://takumakei.blogspot.jp/2010/03/mingwg-440.html
# -static-libgcc

# INCLUDE = -I/home/tako/progs/local/include/ -I/home/tako/progs/local/include/opencv/
# LIBPATH = -L/home/tako/progs/local/lib/ -lopencv_core.dll -lopencv_highgui.dll -lopencv_objdetect.dll -lopencv_imgproc.dll

% : %.cc
	$(CXX) -pipe -O2 $(INCLUDE) $< $(LIBPATH) -o $@

% : %.c
	$(CC) -pipe -O2 $(INCLUDE) $< $(LIBPATH) -o $@

all : $(TARGET)

