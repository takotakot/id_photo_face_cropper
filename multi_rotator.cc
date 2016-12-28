#include<iostream>
#include<string>
#include<vector>
#include<iterator>
#include<fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

std::string in_dir_name = "to_recognize/";
std::string out_dir_name = "rotated/";
std::string file_list_name = "to_recognize_list_rotate.txt";
std::string error_file_name = "face_recognition_error.txt";
std::string dat_file_name = "recog.dat";

struct image_rotator{
	std::string default_cascade_name, upper_body_cascade_name;
	cv::CascadeClassifier default_cascade, upper_body_cascade;
	bool status, debug;
	cv::Mat img_gray;
	std::vector<cv::Rect> faces, upper_bodies;
	double scale;
    std::ofstream ofs;

	image_rotator() {
		ofs.open(dat_file_name.c_str());


		debug = false;
		status = true;
		return;
	}
	void set_debug(bool d) {
		debug = d;
		return;
	}
	void writeout_rotate(std::string &write_img_name, cv::Mat &img_color, double angle) {
		// int len = std::max(img_color.cols, img_color.rows);
		int len_max = std::max(img_color.cols, img_color.rows);
		int len_min = std::min(img_color.cols, img_color.rows);
		cv::Point2f pt(img_color.cols/2., img_color.rows/2.);
		cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);

		// for face rotate only
		r.at<double>(1, 2) += len_max/8.;
		// r.at<double>(0, 2) += len/24.;
		cv::Mat rotated_img;
		// cv::Mat rotated_img = img_color.clone();

		// http://stackoverflow.com/questions/34114741/how-to-maintain-white-background-when-using-opencv-warpaffine
		// http://stackoverflow.com/questions/32230252/opencv-keep-background-transparent-during-warpaffine
		// border
		// http://geekn-nerd.blogspot.jp/2012/02/2-opencv-for-android.html
		// http://blog.umentu.work/?p=369
		// cv::warpAffine(img_color, rotated_img, r, cv::Size(len, len), cv::INTER_CUBIC, cv::BORDER_CONSTANT, cv::Scalar(200, 200, 180));
		// cv::warpAffine(img_color, rotated_img, r, cv::Size(len_min, len_max));
		cv::warpAffine(img_color, rotated_img, r, cv::Size(len_min, len_max), cv::INTER_CUBIC, cv::BORDER_REFLECT);

		cv::imwrite(write_img_name, rotated_img);
		return;
	}
};

int main(int argc, char *argv[]) {
	// std::cerr << "begin" << std::endl;
	std::string buf;
	int w, h;
	double angle;
	// read file list
	std::vector<std::string> file_list;
	std::vector<double> angles;
	std::ifstream ifs;
	ifs.open(file_list_name.c_str());
	while(!ifs.eof()) {
		// std::getline(ifs, buf);
		ifs >> buf >> w >> h >> angle;
		if(buf != "") {
			file_list.push_back(buf);
			angles.push_back(angle);
		}
		// std::cerr << buf << std::endl;
	}
	ifs.close();

	std::ofstream ofs_error;
	ofs_error.open(error_file_name.c_str());

	cv::Mat img_color;
	std::string read_img_name, write_img_name;
	bool recog_ok;
	// face_recognizer face_recognizer_obj;
	image_rotator rotator_obj;
	// face_recognizer_obj.set_debug(true);
	std::vector<double>::iterator dit = angles.begin();
	for(std::vector<std::string>::iterator it = file_list.begin(); it != file_list.end(); it++) {
		std::cerr << *it << std::endl;
		recog_ok = true;
		// name
		read_img_name = in_dir_name + *it;
		write_img_name = out_dir_name + *it;

		// read
		img_color = cv::imread(read_img_name);

		// recognize
		// recog_ok = face_recognizer_obj.recognize(img_color, read_img_name);
		// recog_ok = rotator_obj.rotate(img_color, read_img_name, *dit);

		// write
		if(!recog_ok) {
			std::cerr << "error recognize " << read_img_name << std::endl;
			ofs_error << "error recognize " << read_img_name << std::endl;
		}else{
			std::cerr << write_img_name << " " << *dit << std::endl;
			rotator_obj.writeout_rotate(write_img_name, img_color, *dit);
			// cv::imwrite(write_img_name, img_color);
		}
		++dit;
	}

	return 0;
}
