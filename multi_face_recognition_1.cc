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

struct face_recognizer{
	std::string default_cascade_name, upper_body_cascade_name;
	cv::CascadeClassifier default_cascade, upper_body_cascade;
	bool status, debug;
	cv::Mat img_gray;
	std::vector<cv::Rect> faces, upper_bodies;
	double scale;
	
	face_recognizer() {
		status = false;
		scale = 2.0;  // 4.0
		
		default_cascade_name = "haarcascade_frontalface_default.xml";
		upper_body_cascade_name = "haarcascade_upperbody.xml";
		
		if(!default_cascade.load(default_cascade_name)) {
			std::cerr << "error load " << default_cascade_name << std::endl;
			return;
		}
		if(!upper_body_cascade.load(upper_body_cascade_name)) {
			std::cerr << "error load " << upper_body_cascade_name << std::endl;
			return;
		}
		
		debug = false;
		status = true;
		return;
	}
	void set_debug(bool d) {
		debug = d;
		return;
	}
	bool recognize(cv::Mat &img_color) {
		// initialize
		faces.clear();
		upper_bodies.clear();
		
		// cv::Mat gray, img_gray_small(cv::saturate_cast<int>(img.rows/scale), cv::saturate_cast<int>(img.cols/scale), CV_8UC1);
		// グレースケール画像に変換
		cv::Mat img_gray_small(cv::saturate_cast<int>(img_color.rows/scale), cv::saturate_cast<int>(img_color.cols/scale), CV_8UC1);
		cv::cvtColor(img_color, img_gray, CV_BGR2GRAY);
		
		// 処理時間短縮のために画像を縮小
		cv::resize(img_gray, img_gray_small, img_gray_small.size(), 0, 0, cv::INTER_LINEAR);
		cv::equalizeHist( img_gray_small, img_gray_small);
		//  cv::equalizeHist(img_gray, img_gray);
		//  cv::Mat img_gray_small = img_gray;
		
		/// マルチスケール（顔）探索xo
		// 画像，出力矩形，縮小スケール，最低矩形数，（フラグ），最小矩形
		default_cascade.detectMultiScale(img_gray_small, faces, 1.1, 2,
			CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		
		// 結果の描画
		// size fractions is 0.75
		std::vector<cv::Rect>::const_iterator r;  // r = faces.begin();
		cv::Point center, tl, br, tl_cut, br_cut;
		int radius;
		// for(; r != faces.end(); ++r)
		if(0 == faces.size()) {
			return false;
		}
		
		r = select_max_size(faces);
		center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
		center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
		radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);
		std::cerr << r->width*scale << " " << r->height*scale << std::endl;
		if(debug) cv::circle( img_color, center, radius, cv::Scalar(80,80,255), 3, 8, 0 );
		tl.x = r->x*scale;
		tl.y = r->y*scale;
		br.x = (r->x + r->width)*scale;
		br.y = (r->y + r->height)*scale;
		if(debug) cv::rectangle( img_color, tl, br, cv::Scalar(128,128,255), 3, 8, 0 );
		
		// 1.8/2.4 = 0.75
		// 1.65/2.2 = 0.75
		tl_cut.x = (r->x - r->width * 0.325) * scale;
		tl_cut.y = (r->y - r->height * 0.6) * scale;
		br_cut.x = (r->x + r->width * 1.325) * scale;
		br_cut.y = (r->y + r->height * 1.6) * scale;
		
		if(debug) cv::rectangle( img_color, tl_cut, br_cut, cv::Scalar(255,128,128), 3, 8, 0 );
		
		/*
		/// マルチスケール（上半身）探索
		// 画像，出力矩形，縮小スケール，最低矩形数，（フラグ），最小矩形
		upper_body_cascade.detectMultiScale(img_gray_small, upper_bodies, 1.1, 2,
			CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
		
		// 結果の描画
		r = upper_bodies.begin();
		for(; r != upper_bodies.end(); ++r) {
			int radius;
			center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
			center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
			radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);
			cv::circle( img_color, center, radius, cv::Scalar(80,255,80), 3, 8, 0 );
			tl.x = r->x*scale;
			tl.y = r->y*scale;
			br.x = (r->x + r->width)*scale;
			br.y = (r->y + r->height)*scale;
			cv::rectangle( img_color, tl, br, cv::Scalar(128,255,128), 3, 8, 0 );
		}
		*/
		
		return true;
	}
	std::vector<cv::Rect>::const_iterator select_max_size(const std::vector<cv::Rect>& rects) {
		int size_max = -1, size;
		std::vector<cv::Rect>::const_iterator r_max, r = rects.begin();
		for(; r != rects.end(); ++r) {
			size = cv::saturate_cast<int>((r->width * r->height));
			if(size_max < size) {
				size_max = size;
				r_max = r;
			}
		}
		
		return r_max;
	}
	void writeout(std::string &write_img_name, cv::Mat &img_color) {
		std::vector<cv::Rect>::const_iterator r;
		r = select_max_size(faces);
		
		cv::Point tl_cut, br_cut;
		tl_cut.x = (r->x - r->width * 0.325) * scale;
		tl_cut.y = (r->y - r->height * 0.6) * scale;
		br_cut.x = (r->x + r->width * 1.325) * scale;
		br_cut.y = (r->y + r->height * 1.6) * scale;
		
		tl_cut.x = std::max(tl_cut.x, 0);
		tl_cut.y = std::max(tl_cut.y, 0);
		br_cut.x = std::min(br_cut.x, img_color.cols);
		br_cut.y = std::min(br_cut.y, img_color.rows);
		std::cerr << tl_cut.x << " " << tl_cut.y << " " << br_cut.x << " " << br_cut.y << std::endl;
		
		cv::Mat roi_img(img_color, cv::Rect(tl_cut, br_cut));
		
		// cv::rectangle( img_color, tl_cut, br_cut, cv::Scalar(255,128,128), 3, 8, 0 );
		cv::imwrite(write_img_name, roi_img);
		return;
	}
};

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
	face_recognizer face_recognizer_obj;
	for(std::vector<std::string>::iterator it = file_list.begin(); it != file_list.end(); it++) {
		std::cerr << *it << std::endl;
		recog_ok = false;
		// name
		read_img_name = in_dir_name + *it;
		write_img_name = out_dir_name + *it;
		
		// read
		img_color = cv::imread(read_img_name);
		
		// recognize
		recog_ok = face_recognizer_obj.recognize(img_color);
		
		// write
		if(!recog_ok) {
			std::cerr << "error recognize " << read_img_name << std::endl;
			ofs_error << "error recognize " << read_img_name << std::endl;
		}else{
			face_recognizer_obj.writeout(write_img_name, img_color);
			// cv::imwrite(write_img_name, img_color);
		}
	}
	
	return 0;
}
