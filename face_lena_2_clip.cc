#include<iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[])
{
  // const char *imagename = argc > 1 ? argv[1] : "../../image/lenna.png";
	const char *imagename = "lena.jpg";
  cv::Mat img = cv::imread(imagename, 1);
	if(img.empty()) {
		std::cout << "error" << std::endl;
		return -1; 
	}
  
  // double scale = 4.0;
	double scale = 1.0;
  cv::Mat gray, smallImg(cv::saturate_cast<int>(img.rows/scale), cv::saturate_cast<int>(img.cols/scale), CV_8UC1);
  // グレースケール画像に変換
  cv::cvtColor(img, gray, CV_BGR2GRAY);
  // 処理時間短縮のために画像を縮小
//  cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
//  cv::equalizeHist( smallImg, smallImg);
	cv::equalizeHist( gray, gray);

  // 分類器の読み込み
  std::string cascadeName = "./haarcascade_frontalface_alt.xml"; // Haar-like
  //std::string cascadeName = "./lbpcascade_frontalface.xml"; // LBP
  cv::CascadeClassifier cascade;
  if(!cascade.load(cascadeName))
    return -1;

  std::vector<cv::Rect> faces;
  /// マルチスケール（顔）探索xo
  // 画像，出力矩形，縮小スケール，最低矩形数，（フラグ），最小矩形
  cascade.detectMultiScale(gray, faces,
                           1.1, 2,
                           CV_HAAR_SCALE_IMAGE,
                           cv::Size(30, 30));

  // 結果の描画
  std::vector<cv::Rect>::const_iterator r = faces.begin();
  for(; r != faces.end(); ++r) {
    cv::Point center;
    int radius;
    center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
    center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
    radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);
    cv::circle( img, center, radius, cv::Scalar(80,80,255), 3, 8, 0 );
  	
  	cv::Point tl, br;
  	tl.x = r->x;
  	tl.y = r->y;
  	br.x = r->x + r->width;
  	br.y = r->y + r->height;
    cv::rectangle( img, tl, br, cv::Scalar(128,128,255), 3, 8, 0 );
  }
	
	
  // 分類器の読み込み
  cascadeName = "./haarcascade_upperbody.xml"; // Haar-like
  // cascade;
	if(!cascade.load(cascadeName)) {
		std::cout << "error" << std::endl;
    	return -1;
	}
  cascade.detectMultiScale(gray, faces,
                           1.1, 2,
                           CV_HAAR_SCALE_IMAGE,
                           cv::Size(30, 30));

  // 結果の描画
  r = faces.begin();
  for(; r != faces.end(); ++r) {
    cv::Point center;
    int radius;
    center.x = cv::saturate_cast<int>((r->x + r->width*0.5)*scale);
    center.y = cv::saturate_cast<int>((r->y + r->height*0.5)*scale);
    radius = cv::saturate_cast<int>((r->width + r->height)*0.25*scale);
    cv::circle( img, center, radius, cv::Scalar(80,255,80), 3, 8, 0 );
  	
  	cv::Point tl, br;
  	tl.x = r->x;
  	tl.y = r->y;
  	br.x = r->x + r->width;
  	br.y = r->y + r->height;
    cv::rectangle( img, tl, br, cv::Scalar(128,255,128), 3, 8, 0 );
  }


//  cv::namedWindow("result", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
//  cv::imshow( "result", img );    
//  cv::waitKey(0);
	cv::imwrite("result.jpg", img);
	return 0;
}
