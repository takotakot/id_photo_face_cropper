CXX = i686-w64-mingw32-g++-posix

CXXFLAGS = -pipe -std=gnu++14 -fopenmp -O2 -g

#INCPATH = -I/home/tako/progs/include
#LIBPATH = -L/home/tako/progs/lib
#INCPATH = -I ~/dlib_install/include
#LIBPATH = -L ~/dlib_install/lib
#INCPATH = -I ~/lprogs/dlib_install/include -I ~/lprogs/opencv-2.4.12.2/include
#LIBPATH = -L ~/lprogs/dlib_install/lib -L ~/lprogs/opencv-2.4.12.2/lib
#LIBRARY = -lopencv_calib3d -lopencv_features2d -lopencv_imgproc -lopencv_core -lopencv_highgui -ldlib

# INCPATH = -I ~/lprogs/dlib_install/include -I ~/lprogs/opencv-3.4.5_install/include
# LIBPATH = -L ~/lprogs/dlib_install/lib -L ~/lprogs/opencv-3.4.5_install/lib
INCPATH = -I ~/progs/dlib_mingw32_install/include -I ~/progs/opencv-3.4.5_install_mingw32/include
LIBPATH = -L ~/progs/dlib_mingw32_install/lib -L ~/progs/opencv-3.4.5_install_mingw32/lib
# LIBRARY = -lopencv_calib3d -lopencv_features2d -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_imgcodecs -ldlib
LIBRARY = -lopencv_calib3d345 -lopencv_features2d345 -lopencv_imgproc345 -lopencv_core345 -lopencv_highgui345 -lopencv_imgcodecs345 -ldlib

TARGET = hpe_4 multi_face_cropper single_face_cropper face_cropper_dnd # thread_test 
OBJS = multi_face_cropper.o face_cropper.o hpe_3.o read_srcs.o functions.o

.PHONY = test

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $^

all: $(OBJS) $(TARGET) # affine_forever

$(TARGET): % : %.o face_cropper.o read_srcs.o functions.o
	$(CXX) $(CXXFLAGS) $(LIBPATH) $^ $(LIBRARY) -o $*

affine_forever: %: %.o
	$(CXX) $(CXXFLAGS) $(LIBPATH) $^ $(LIBRARY) -o $*

test: all
	# time ./face_cropper_dnd to_recognize
	bash -c "time ./affine_forever"
