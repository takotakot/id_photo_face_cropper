#include<iostream>
#include<string>
#include<vector>
#include<iterator>
#include<fstream>
#include<omp.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

// std::string in_dir_name = "to_recognize/";
std::string in_dir_name = "rotated/";
std::string out_dir_name = "results/";
std::string file_list_name = "to_recognize_list_prep.txt";
std::string error_file_name = "face_recognition_error.txt";
// std::string dat_file_name = "recog.dat";
std::string result_file_name = "result_detection.txt";

struct face_detect_result{
	// int w, h;
	int rx, ry;
	int rw, rh;
	int detected;  // 0|1
};

struct face_detector{
	std::string default_cascade_name, upper_body_cascade_name, alt_cascade_name, alt2_cascade_name, alt_tree_cascade_name, profile_cascade_name;
	// cv::CascadeClassifier default_cascade, upper_body_cascade, alt_cascade, alt_tree_cascade, alt2_cascade;
	std::vector<cv::CascadeClassifier> cascades;
	bool status, debug, do_equalize;
	cv::Mat img_gray;
	std::vector<cv::Rect> faces, faces_all, upper_bodies;
	double scale;
	// std::ofstream ofs;
	cv::Scalar scalar_circle;
	cv::Size min_search_size;

	face_detector() {
		status = false;
		scale = 1.1;  // 4.0

		// http://symfoware.blog68.fc2.com/blog-entry-1556.html
		// http://ameblo.jp/principia-ca/entry-11200475276.html
		default_cascade_name = "haarcascade_frontalface_default.xml";
		alt_cascade_name = "haarcascade_frontalface_alt.xml";
		alt_tree_cascade_name = "haarcascade_frontalface_alt_tree.xml";
		alt2_cascade_name = "haarcascade_frontalface_alt2.xml";
		profile_cascade_name = "haarcascade_profileface.xml";
		upper_body_cascade_name = "haarcascade_upperbody.xml";

		cv::CascadeClassifier cascade_tmp;

		if(!cascade_tmp.load(default_cascade_name)) {
			std::cerr << "error load " << default_cascade_name << std::endl;
			return;
		}
		cascades.push_back(cascade_tmp);
		if(!cascade_tmp.load(alt_cascade_name)) {
			std::cerr << "error load " << alt_cascade_name << std::endl;
			return;
		}
		cascades.push_back(cascade_tmp);
		if(!cascade_tmp.load(alt_tree_cascade_name)) {
			std::cerr << "error load " << alt_tree_cascade_name << std::endl;
			return;
		}
		cascades.push_back(cascade_tmp);
		if(!cascade_tmp.load(alt2_cascade_name)) {
			std::cerr << "error load " << alt2_cascade_name << std::endl;
			return;
		}
		cascades.push_back(cascade_tmp);
		// ofs.open(result_file_name.c_str());

		debug = false;
		status = true;
		do_equalize = false;

		scalar_circle = cv::Scalar(80,80,255);
		return;
	}
	void set_debug(bool d) {
		debug = d;
		return;
	}
	bool detect(cv::Mat &img_color, std::string file, face_detect_result &result) {
		std::vector<cv::Rect>::const_iterator r;  // r = faces.begin();

		// initialize
		faces_all.clear();

		// cv::Mat gray, img_gray_small(cv::saturate_cast<int>(img.rows/scale), cv::saturate_cast<int>(img.cols/scale), CV_8UC1);
		// グレースケール画像に変換
		// cv::Mat img_gray_small(cv::saturate_cast<int>(img_color.rows/scale), cv::saturate_cast<int>(img_color.cols/scale), CV_8UC1);
		// cv::Mat img_gray_small(cv::saturate_cast<int>(img_color.rows), cv::saturate_cast<int>(img_color.cols), CV_8UC1);
		cv::cvtColor(img_color, img_gray, CV_BGR2GRAY);

		// 処理時間短縮のために画像を縮小
		// cv::resize(img_gray, img_gray_small, img_gray_small.size(), 0, 0, cv::INTER_LINEAR);

		if(do_equalize) {
			cv::equalizeHist( img_gray, img_gray);
		}

		/// マルチスケール（顔）探索xo
		// 画像，出力矩形，縮小スケール，最低矩形数，（フラグ），最小矩形

		// http://docs.opencv.org/2.4/modules/objdetect/doc/cascade_classification.html
		// https://github.com/Itseez/opencv/blob/master/samples/cpp/dbt_face_detection.cpp
		min_search_size = cv::Size(30, 30);
		for(int i = 0; i < cascades.size(); ++i) {
			faces.clear();
			cascades[i].detectMultiScale(img_gray, faces, scale, 2, CV_HAAR_SCALE_IMAGE, min_search_size);

			if(0 != faces.size()) {
				faces_all.insert(faces_all.end(), faces.begin(), faces.end());
				r = select_max_size(faces_all);
				min_search_size = cv::Size(r->width * 0.8, r->height * 0.8);
			}
		}

		// 結果の描画
		// size fractions is 0.75
		cv::Point center, tl, br, tl_cut, br_cut;
		int radius;
		// for(; r != faces.end(); ++r)
		if(0 == faces_all.size()) {
			return false;
		}

		r = select_intelligent(faces_all);
        /*
		center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
		center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
		radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);
		std::cerr << r->width*scale << " " << r->height*scale << std::endl;
        */
		if(debug) {
			std::vector<cv::Rect>::const_iterator r = faces_all.begin();
			for(; r != faces_all.end(); ++r) {
				center.x = r->x + r->width*0.5;
				center.y = r->y + r->height*0.5;
				radius = (r->width + r->height)*0.25;

				cv::circle( img_color, center, radius, scalar_circle, 3, 8, 0 );
			}
		}

		center.x = cv::saturate_cast<int>((r->x + r->width*0.5));
		center.y = cv::saturate_cast<int>((r->y + r->height*0.5));
		radius = cv::saturate_cast<int>((r->width + r->height)*0.25);

		tl.x = r->x;
		tl.y = r->y;
		br.x = (r->x + r->width);
		br.y = (r->y + r->height);
		if(debug) cv::rectangle( img_color, tl, br, cv::Scalar(128,128,255), 3, 8, 0 );

		// 1.8/2.4 = 0.75
		// 1.65/2.2 = 0.75

		tl_cut.x = (r->x - r->width * 0.325);
		tl_cut.y = (r->y - r->height * 0.6);
		br_cut.x = (r->x + r->width * 1.325);
		br_cut.y = (r->y + r->height * 1.6);

		// result.w
		// result.h
		result.rx = r->x;
		result.ry = r->y;
		result.rw = r->width;
		result.rh = r->height;

		if(debug) cv::rectangle( img_color, tl_cut, br_cut, cv::Scalar(255,128,128), 3, 8, 0 );

        // dat file
        // ofs << file << "\t" << 1 << " " << r->x << " " << r->y << " " << r->x+r->width << " " << r->y+r->height << std::endl;

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
	std::vector<cv::Rect>::const_iterator select_intelligent(const std::vector<cv::Rect>& rects) {
		int size_max = -1, size;
		std::vector<cv::Rect>::const_iterator r_max, r = rects.begin();
		for(; r != rects.end(); ++r) {
			size = cv::saturate_cast<int>((r->width * r->height));
			if(size_max < size) {
				size_max = size;
				r_max = r;
			}
		}

		// choose upper rectangle if size > size_max *0.8
		int y_min = 1<<30;
		r = rects.begin();
		for(; r != rects.end(); ++r) {
			size = cv::saturate_cast<int>((r->width * r->height));
			if(size > size_max * 0.8) {
				if(r->y < y_min) {
					y_min = r->y;
					size_max = size;
					r_max = r;
				}
			}
		}

		return r_max;
	}
	void writeout(std::string &write_img_name, cv::Mat &img_color) {
		std::vector<cv::Rect>::const_iterator r;
		r = select_intelligent(faces_all);

		cv::Point tl_cut, br_cut;
		tl_cut.x = (r->x - r->width * 0.325);
		tl_cut.y = (r->y - r->height * 0.6);
		br_cut.x = (r->x + r->width * 1.325);
		br_cut.y = (r->y + r->height * 1.6);

		// TODO
		tl_cut.x = std::max(tl_cut.x, 0);
		tl_cut.y = std::max(tl_cut.y, 0);
		br_cut.x = std::min(br_cut.x, img_color.cols);
		br_cut.y = std::min(br_cut.y, img_color.rows);
		std::cerr << tl_cut.x << " " << tl_cut.y << " " << br_cut.x << " " << br_cut.y << std::endl;

		if(!debug) {
			cv::Mat roi_img(img_color, cv::Rect(tl_cut, br_cut));

			// cv::rectangle( img_color, tl_cut, br_cut, cv::Scalar(255,128,128), 3, 8, 0 );
			cv::imwrite(write_img_name, roi_img);
		}else{
			cv::imwrite(write_img_name, img_color);
		}
		return;
	}
};

int main(int argc, char *argv[]) {
	// std::cerr << "begin" << std::endl;
	std::string buf;
	int w, h;
	face_detect_result prev_result;
	// read file list
	std::vector<std::string> file_list;
	std::vector<int> ws, hs;
	std::vector<face_detect_result> results;
	std::ifstream ifs;
	std::ofstream ofs;
	ifs.open(file_list_name.c_str());
	ofs.open(result_file_name.c_str());
	while(!ifs.eof()) {
		// std::getline(ifs, buf);
		ifs >> buf >> w >> h;
		ifs >> prev_result.rx >> prev_result.ry >> prev_result.rw >> prev_result.rh >> prev_result.detected;
		// if(buf != "") {
		if(ifs) {
			file_list.push_back(buf);
			ws.push_back(w);
			hs.push_back(h);
			results.push_back(prev_result);
		}
		// std::cerr << buf << std::endl;
	}
	ifs.close();

	std::ofstream ofs_error;
	ofs_error.open(error_file_name.c_str());

	/*
	*/
	// #pragma omp parallel for private(read_img_name, write_img_name, img_color, recog_ok, face_detector_obj)
	// private(read_img_name, write_img_name, img_color, recog_ok)
	#pragma omp parallel
	{
		face_detector face_detector_obj;
		std::stringstream obuf;
		cv::Mat img_color;
		std::string read_img_name, write_img_name;
		bool recog_ok;

		face_detector_obj.set_debug(true);
		#pragma omp for
		for(int i = 0; i < file_list.size(); ++i) {
			// http://stackoverflow.com/questions/15033827/multiple-threads-writing-to-stdcout-or-stdcerr
			std::cerr << file_list[i] << std::endl;

			// name
			read_img_name = in_dir_name + file_list[i];
			write_img_name = out_dir_name + file_list[i];

			if(!results[i].detected) {
				recog_ok = false;

				// read
				img_color = cv::imread(read_img_name);

				// detect
				recog_ok = face_detector_obj.detect(img_color, read_img_name, results[i]);

				// write
				if(!recog_ok) {
					std::cerr << "error detect " << read_img_name << std::endl;
					ofs_error << "error detect " << read_img_name << std::endl;
				}else{
					face_detector_obj.writeout(write_img_name, img_color);
				}
			}else{
				recog_ok = true;
			}
			obuf << file_list[i] << "\t" << ws[i] << "\t" << hs[i];
			obuf << "\t" << results[i].rx << "\t" << results[i].ry << "\t" << results[i].rw << "\t" << results[i].rh << "\t" << results[i].detected << std::endl;
		}
		#pragma omp critical
		{
			ofs << obuf.str();
		}
	}

	return 0;
}
