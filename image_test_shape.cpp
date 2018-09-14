#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

using namespace dlib;
using namespace std;

//Intrisics can be calculated using opencv sample code under opencv/sources/samples/cpp/tutorial_code/calib3d
//Normally, you can also apprximate fx and fy by image width, cx by half image width, cy by half image height instead
double K[9] = { 6.5308391993466671e+002, 0.0, 3.1950000000000000e+002, 0.0, 6.5308391993466671e+002, 2.3950000000000000e+002, 0.0, 0.0, 1.0 };
double D[5] = { 7.0834633684407095e-002, 6.9140193737175351e-002, 0.0, 0.0, -1.3073460323689292e+000 };

int main(int argc, char* argv[])
{

    frontal_face_detector detector = get_frontal_face_detector();
    shape_predictor predictor;
	std::cerr << __LINE__ << std::endl;
    deserialize("shape_predictor_68_face_landmarks.dat") >> predictor;

	std::cerr << __LINE__ << std::endl;

    //fill in cam intrinsics and distortion coefficients
	/*
    cv::Mat cam_matrix = cv::Mat(3, 3, CV_64FC1, K);
    cv::Mat dist_coeffs = cv::Mat(5, 1, CV_64FC1, D);
	*/
	// https://qiita.com/TaroYamada/items/e3f3d0ea4ecc0a832fac
    cv::Mat cam_matrix;
    cv::Mat dist_coeffs;

    //fill in 3D ref points(world coordinates), model referenced from http://aifi.isr.uc.pt/Downloads/OpenGL/glAnthropometric3DModel.cpp
    std::vector<cv::Point3d> object_pts;
    object_pts.push_back(cv::Point3d(6.825897, 6.760612, 4.402142));     //#33 left brow left corner
    object_pts.push_back(cv::Point3d(1.330353, 7.122144, 6.903745));     //#29 left brow right corner
    object_pts.push_back(cv::Point3d(-1.330353, 7.122144, 6.903745));    //#34 right brow left corner
    object_pts.push_back(cv::Point3d(-6.825897, 6.760612, 4.402142));    //#38 right brow right corner
    object_pts.push_back(cv::Point3d(5.311432, 5.485328, 3.987654));     //#13 left eye left corner
    object_pts.push_back(cv::Point3d(1.789930, 5.393625, 4.413414));     //#17 left eye right corner
    object_pts.push_back(cv::Point3d(-1.789930, 5.393625, 4.413414));    //#25 right eye left corner
    object_pts.push_back(cv::Point3d(-5.311432, 5.485328, 3.987654));    //#21 right eye right corner
    object_pts.push_back(cv::Point3d(2.005628, 1.409845, 6.165652));     //#55 nose left corner
    object_pts.push_back(cv::Point3d(-2.005628, 1.409845, 6.165652));    //#49 nose right corner
    object_pts.push_back(cv::Point3d(2.774015, -2.080775, 5.048531));    //#43 mouth left corner
    object_pts.push_back(cv::Point3d(-2.774015, -2.080775, 5.048531));   //#39 mouth right corner
    object_pts.push_back(cv::Point3d(0.000000, -3.116408, 6.097667));    //#45 mouth central bottom corner
    object_pts.push_back(cv::Point3d(0.000000, -7.415691, 4.070434));    //#6 chin corner

    //2D ref points(image coordinates), referenced from detected facial feature
    std::vector<cv::Point2d> image_pts;

    //result
    cv::Mat rotation_vec;                           //3 x 1
    cv::Mat rotation_mat;                           //3 x 3 R
    cv::Mat translation_vec;                        //3 x 1 T
    cv::Mat pose_mat = cv::Mat(3, 4, CV_64FC1);     //3 x 4 R | T
    cv::Mat euler_angle = cv::Mat(3, 1, CV_64FC1);

    //reproject 3D points world coordinate axis to verify result pose
    std::vector<cv::Point3d> reprojectsrc;
    reprojectsrc.push_back(cv::Point3d(10.0, 10.0, 10.0));
    reprojectsrc.push_back(cv::Point3d(10.0, 10.0, -10.0));
    reprojectsrc.push_back(cv::Point3d(10.0, -10.0, -10.0));
    reprojectsrc.push_back(cv::Point3d(10.0, -10.0, 10.0));
    reprojectsrc.push_back(cv::Point3d(-10.0, 10.0, 10.0));
    reprojectsrc.push_back(cv::Point3d(-10.0, 10.0, -10.0));
    reprojectsrc.push_back(cv::Point3d(-10.0, -10.0, -10.0));
    reprojectsrc.push_back(cv::Point3d(-10.0, -10.0, 10.0));

    //reprojected 2D points
    std::vector<cv::Point2d> reprojectdst;
    reprojectdst.resize(8);

    //temp buf for decomposeProjectionMatrix()
    cv::Mat out_intrinsics = cv::Mat(3, 3, CV_64FC1);
    cv::Mat out_rotation = cv::Mat(3, 3, CV_64FC1);
    cv::Mat out_translation = cv::Mat(3, 1, CV_64FC1);

    //text on screen
    ostringstream outtext;
	std::cerr << __LINE__ << std::endl;

    //main loop
    do
    {
        // Grab a frame
        cv::Mat temp;
        // cap >> temp;
        if (argc < 2) {
        	temp = cv::imread("IMG_4899.jpg");
        }else{
        	temp = cv::imread(argv[1]);
        }
        cv_image<bgr_pixel> cimg(temp);

    	/*
    	 camera matrix
    	*/
    	double focal_length = temp.cols;
    	cv::Point2d center = cv::Point2d(temp.cols/2,temp.rows/2);
    	cam_matrix = (cv::Mat_<double>(3,3) << focal_length, 0, center.x, 0 , focal_length, center.y, 0, 0, 1);
    	dist_coeffs = cv::Mat::zeros(4,1,cv::DataType<double>::type); // 歪なし

    	std::cerr << __LINE__ << std::endl;
        // Detect faces
        std::vector<rectangle> faces = detector(cimg);

    	std::cerr << __LINE__ << std::endl;
        // Find the pose of each face
        if (faces.size() > 0)
            {
            //track features
            full_object_detection shape = predictor(cimg, faces[0]);

            //draw features
            std::cerr << "points" << std::endl;
            for (unsigned int i = 0; i < 68; ++i)
                {
                circle(temp, cv::Point(shape.part(i).x(), shape.part(i).y()), 2, cv::Scalar(0, 0, 255), -1);
                std::cerr << shape.part(i).x() << "\t" <<  shape.part(i).y() << std::endl;
                }

            //fill in 2D ref points, annotations follow https://ibug.doc.ic.ac.uk/resources/300-W/
            image_pts.push_back(cv::Point2d(shape.part(17).x(), shape.part(17).y())); //#17 left brow left corner
            image_pts.push_back(cv::Point2d(shape.part(21).x(), shape.part(21).y())); //#21 left brow right corner
            image_pts.push_back(cv::Point2d(shape.part(22).x(), shape.part(22).y())); //#22 right brow left corner
            image_pts.push_back(cv::Point2d(shape.part(26).x(), shape.part(26).y())); //#26 right brow right corner
            image_pts.push_back(cv::Point2d(shape.part(36).x(), shape.part(36).y())); //#36 left eye left corner
            image_pts.push_back(cv::Point2d(shape.part(39).x(), shape.part(39).y())); //#39 left eye right corner
            image_pts.push_back(cv::Point2d(shape.part(42).x(), shape.part(42).y())); //#42 right eye left corner
            image_pts.push_back(cv::Point2d(shape.part(45).x(), shape.part(45).y())); //#45 right eye right corner
            image_pts.push_back(cv::Point2d(shape.part(31).x(), shape.part(31).y())); //#31 nose left corner
            image_pts.push_back(cv::Point2d(shape.part(35).x(), shape.part(35).y())); //#35 nose right corner
            image_pts.push_back(cv::Point2d(shape.part(48).x(), shape.part(48).y())); //#48 mouth left corner
            image_pts.push_back(cv::Point2d(shape.part(54).x(), shape.part(54).y())); //#54 mouth right corner
            image_pts.push_back(cv::Point2d(shape.part(57).x(), shape.part(57).y())); //#57 mouth central bottom corner
            image_pts.push_back(cv::Point2d(shape.part(8).x(), shape.part(8).y()));   //#8 chin corner

            //calc pose
            cv::solvePnP(object_pts, image_pts, cam_matrix, dist_coeffs, rotation_vec, translation_vec);

            //reproject
            cv::projectPoints(reprojectsrc, rotation_vec, translation_vec, cam_matrix, dist_coeffs, reprojectdst);

            //draw axis
            line(temp, reprojectdst[0], reprojectdst[1], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[1], reprojectdst[2], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[2], reprojectdst[3], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[3], reprojectdst[0], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[4], reprojectdst[5], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[5], reprojectdst[6], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[6], reprojectdst[7], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[7], reprojectdst[4], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[0], reprojectdst[4], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[1], reprojectdst[5], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[2], reprojectdst[6], cv::Scalar(0, 0, 255));
            line(temp, reprojectdst[3], reprojectdst[7], cv::Scalar(0, 0, 255));

            //calc euler angle
            cv::Rodrigues(rotation_vec, rotation_mat);
            cv::hconcat(rotation_mat, translation_vec, pose_mat);
            cv::decomposeProjectionMatrix(pose_mat, out_intrinsics, out_rotation, out_translation, cv::noArray(), cv::noArray(), cv::noArray(), euler_angle);

            //show angle result
            outtext << "X: " << setprecision(3) << euler_angle.at<double>(0);
            cv::putText(temp, outtext.str(), cv::Point(50, 40), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 0));
            outtext.str("");
            outtext << "Y: " << setprecision(3) << euler_angle.at<double>(1);
            cv::putText(temp, outtext.str(), cv::Point(50, 60), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 0));
            outtext.str("");
            outtext << "Z: " << setprecision(3) << euler_angle.at<double>(2);
            cv::putText(temp, outtext.str(), cv::Point(50, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 0));
            outtext.str("");

            image_pts.clear();
            }

        //press esc to end
    	cv::imwrite("result.jpg", temp);
    }while (false);



    return 0;
}

