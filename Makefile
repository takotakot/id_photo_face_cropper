
#CC = i686-pc-mingw32-gcc
#CXX = i686-pc-mingw32-g++
#CC = x86_64-w64-mingw32-gcc
#CXX = x86_64-w64-mingw32-g++ 
CC = gcc
CXX = g++

# TARGET = face_lena face_lena_2 clip face_lena_2_clip multi_face_recognition_1
TARGET = face_lena_2_clip face_lena_2_writeout multi_face_recognition_1 multi_face_recognition_2 multi_face_recognition_3 multi_face_recognition_2_lbp multi_face_recognition_2_fast multi_face_recognition_2_fast_out_text
# TARGET = multi_face_recognition_1

# /cygdrive/c/Users/nakano/progs/opencv


# INCLUDE = -I/cygdrive/c/Users/nakano/progs/opencv2.4.10/include -I/cygdrive/c/Users/nakano/progs/opencv2.4.10/include/opencv
INCLUDE = -I/usr/local/include/opencv

# INCLUDE = -I/home/tako/progs/local/include/ -I/home/tako/progs/local/include/opencv/
# INCLUDE = -I/cygdrive/c/Users/nakano/progs/opencv/build/include -I/cygdrive/c/Users/nakano/progs/opencv/build/include/opencv
# LIBPATH = -static-libgcc -static -L/home/tako/facial_recognition/vc_static/ /home/tako/facial_recognition/vc_static/opencv_core243.lib /home/tako/facial_recognition/vc_static/opencv_highgui243.lib /home/tako/facial_recognition/vc_static/opencv_objdetect243.lib /home/tako/facial_recognition/vc_static/opencv_imgproc243.lib
# http://mingw.5.n7.nabble.com/using-VC-lib-with-mingw-td19148.html
# That distinction aside, the library itself should be named either libxml2.a or xml2.lib, but *not* libxml2.lib.  If it is appropriately 


# LIBPATH = -Wl,--export-all-symbols -Wl,--enable-auto-image-base,--enable-auto-import -L/cygdrive/c/Users/nakano/progs/opencv2.4.10/lib -lopencv_core.dll -lopencv_highgui.dll -lopencv_objdetect.dll -lopencv_imgproc.dll -lopencv_calib3d.dll -lopencv_contrib.dll -lopencv_legacy.dll -lopencv_ml.dll -lopencv_stitching.dll -lopencv_ts -lopencv_video.dll -lopencv_features2d.dll -lopencv_flann.dll -lopencv_gpu.dll -lopencv_nonfree.dll -lopencv_ocl.dll -lopencv_photo.dll -lopencv_stitching.dll -lopencv_videostab.dll -lopencv_superres.dll
# LIBPATH = -Wl,--export-all-symbols -Wl,--enable-auto-image-base,--enable-auto-import -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc -lopencv_calib3d -lopencv_contrib -lopencv_legacy -lopencv_ml -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_nonfree -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_videostab -lopencv_superres
LIBPATH = -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_objdetect -lopencv_imgproc -lopencv_calib3d -lopencv_contrib -lopencv_legacy -lopencv_ml -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_nonfree -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_videostab -lopencv_superres


# LIBPATH = -Wl,--export-all-symbols -Wl,--enable-auto-image-base,--enable-auto-import -L/cygdrive/c/Users/nakano/progs/cv-build-2.4.9-cygwin/lib -lcv2.dll -lopencv_core.dll -lopencv_highgui.dll -lopencv_objdetect.dll -lopencv_imgproc.dll -lopencv_calib3d.dll -lopencv_contrib.dll -lopencv_legacy.dll -lopencv_ml.dll -lopencv_stitching.dll -lopencv_ts -lopencv_video.dll -lopencv_features2d.dll -lopencv_flann.dll -lopencv_gpu.dll -lopencv_haartraining_engine -lopencv_nonfree.dll -lopencv_ocl.dll -lopencv_photo.dll -lopencv_stitching.dll -lopencv_videostab.dll -lopencv_superres.dll

#LIBPATH = -L/cygdrive/c/Users/nakano/progs/cv-build-2.4.9-mingw64/lib -lopencv_core_pch_dephelp -lopencv_highgui_pch_dephelp -lopencv_objdetect_pch_dephelp -lopencv_imgproc_pch_dephelp -lopencv_legacy_pch_dephelp -lopencv_ml_pch_dephelp -lopencv_nonfree_pch_dephelp -lopencv_haartraining_engine

# LIBPATH = -static-libgcc -static -L/home/tako/facial_recognition/mingw/lib/ -lopencv_core243.dll -lopencv_highgui243.dll -lopencv_objdetect243.dll -lopencv_imgproc243.dll
# http://takumakei.blogspot.jp/2010/03/mingwg-440.html
# -static-libgcc

# INCLUDE = -I/home/tako/progs/local/include/ -I/home/tako/progs/local/include/opencv/
# LIBPATH = -L/home/tako/progs/local/lib/ -lopencv_core.dll -lopencv_highgui.dll -lopencv_objdetect.dll -lopencv_imgproc.dll

% : %.cc
	$(CXX) -pipe -O2 -Wall $(INCLUDE) $< $(LIBPATH) -o $@

% : %.c
	$(CC) -pipe -O2 $(INCLUDE) $*.o $(LIBPATH) -o $@

all : $(TARGET)

