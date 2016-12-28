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
std::string out_dir_name = "results/";
std::string file_list_name = "to_recognize_list.txt";
std::string error_file_name = "face_recognition_error.txt";

int main(int argc, char *argv[]) {
	// std::cerr << "begin" << std::endl;
	std::string buf;
	// read file list
	std::vector<std::string> file_list;
	std::ifstream ifs;
	ifs.open(file_list_name.c_str());
	while(!ifs.eof()) {
		std::getline(ifs, buf);
		if(buf != "") {
			file_list.push_back(buf);
		}
		// std::cerr << buf << std::endl;
	}
	ifs.close();

	std::ofstream ofs_error;
	ofs_error.open(error_file_name.c_str());

	cv::Mat img_color;
	std::string read_img_name, write_img_name;
	bool recog_ok;
	for(std::vector<std::string>::iterator it = file_list.begin(); it != file_list.end(); it++) {
		recog_ok = true;
		std::cerr << *it << std::endl;
		// name
		read_img_name = in_dir_name + *it;
		// read
		img_color = cv::imread(read_img_name);
		// write
		if(!recog_ok) {
			std::cerr << "error recognize " << read_img_name << std::endl;
			ofs_error << "error recognize " << read_img_name << std::endl;
			std::cout << *it << "\t" << 0 << "\t" << 0 << std::endl;
		}else{
			std::cout << *it << "\t" << img_color.cols << "\t" << img_color.rows << std::endl;
		}
	}

	return 0;
}
