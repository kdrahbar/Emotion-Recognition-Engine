///////////////////////////////////////////////////////////////////////////////////////////////////////
/// DemoTracker.cpp
/// 
/// Description:
/// This program shows you how to use FaceAlignment class in tracking facial landmarks in a video or realtime. 
/// There are two modes: VIDEO, REALTIME.
/// In the VIDEO mode, the program reads input from a video and perform tracking.
/// In the REALTIME mode, the program reads input from the first camera it finds and perform tracking.
/// Note that tracking is performed on the largest face found. The face is detected through OpenCV.
///
/// Dependencies: None. OpenCV DLLs and include folders are copied.
///
/// Author: Xuehan Xiong, xiong828@gmail.com
///
/// Creation Date: 10/15/2013
///
/// Version: 1.0
///
/// Citation: 
/// Xuehan Xiong, Fernando de la Torre, Supervised Descent Method and Its Application to Face Alignment. CVPR, 2013
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <time.h>
#include <Windows.h>
#include "frame.h"
#include <intraface/FaceAlignment.h>
#include <intraface/XXDescriptor.h>

using namespace std;

static cv::Mat cvCurrent;
static cv::Mat cvNeutral;
frame *neutral, *current;
static string imageText;
static bool neutralSelected= false;
bool compareRect(cv::Rect r1, cv::Rect r2) { return r1.height < r2.height; }
static void onMouse( int event, int x, int y, int, void* landmarksFileName )
{
	if( event != cv::EVENT_LBUTTONDOWN )
		return;
	else{
		imageText= "Neutral Frame Selected";
		neutralSelected= true;
		cout << "*** Image Clicked *****\n";
		ofstream landmarksFile;
		landmarksFile.open((char*)landmarksFileName);
		for (int i = 0 ; i < cvCurrent.cols ; i++){
			landmarksFile << (int)cvCurrent.at<float>(0,i)<<"," <<(int)cvCurrent.at<float>(1,i) ;
			if(i<cvCurrent.cols-1)
				landmarksFile << ",";
		}
		landmarksFile << endl;
		landmarksFile.close();
		cout << "*** Wrote Neutral Frame to file  *****\n";
		cvNeutral= cvCurrent;
		neutral = new frame(cvNeutral);
		/*cout << "Neutral LMs: ";
		for(int i=0; i<neutral->landmarks.size(); i++){
		cout << neutral->landmarks[i].x << "," << neutral->landmarks[i].y << ",";
		}
		cout << endl;*/
		neutral->rotate();
		neutral->extract_eye_region_landmarks();
		/*cout << "Neutral LMs After Rotation: ";
		for(int i=0; i<neutral->landmarks.size(); i++){
		cout << neutral->landmarks[i].x << "," << neutral->landmarks[i].y << ",";
		}
		cout << endl;*/
		neutral->calc_centroids();
		/*cout << "Left Centroid: " << neutral->l_centroid.x << "," << neutral->l_centroid.y ;
		cout << "Right Centroid: " << neutral->r_centroid.x << "," << neutral->r_centroid.y << endl;*/
		neutral->l_eye_feats= neutral->extract_eye_feat(neutral->l_centroid, neutral->l_eye_region_landmks);
		neutral->r_eye_feats= neutral->extract_eye_feat(neutral->r_centroid, neutral->r_eye_region_landmks);
		neutral->l_eyebrow_feats= neutral->extract_eyebrow_feat(neutral->l_centroid, neutral->l_eye_region_landmks);
		neutral->r_eyebrow_feats= neutral->extract_eyebrow_feat(neutral->r_centroid, neutral->r_eye_region_landmks);
		/*cout << "Neutral Left Features: ";
		for(int i=0; i<neutral->l_eyebrow_feats.size(); i++){
		cout << neutral->l_eyebrow_feats[i] << ",";
		}
		cout << endl;
		cout << "Neutral Right Features: ";
		for(int i=0; i<neutral->r_eyebrow_feats.size(); i++){
		cout << neutral->r_eyebrow_feats[i] << ",";
		}
		cout << endl;*/

	}

}

// Generates a vector with the classifier parameters for 12 features for class A and B each
// The first half of the vector corresponds to the values for A
// The second half corresponds to the values for B
std::vector<float> bayes_parser(char filename[])
{
	std::ifstream input_file;
	input_file.open(filename);
	std::vector<float> result;
	std::string line;
	// skip the first two lines the numClasses and numFeatures
	std::getline(input_file, line);
	std::getline(input_file, line);
	while (std::getline(input_file, line)) 
	{
		std::istringstream iss(line);
		float value;
		while (iss >> value)
		{
			result.push_back(value);
		}
	}
	input_file.close();
	return result;
}

int findBestClass(std::vector<float> logprob_vec)
{
	
	// find the best class
	int maxClass = 0;
	float maxProb1 = -100000.0, maxProb2 = -100000.0;
	for(int c=0; c< logprob_vec.size(); c++){
		if(logprob_vec[c] > maxProb1){
			maxClass= c;
			if(maxProb1 > maxProb2)
				maxProb2 = maxProb1;
			maxProb1= logprob_vec[c];
		}
		else if(logprob_vec[c] > maxProb2){
			maxProb2= logprob_vec[c];
		}
	}
	// if the difference between the first and second best probability is less then it is a random prediction
	if( abs(maxProb1-maxProb2) < 10 )
		maxClass = logprob_vec.size();
	return maxClass;
}

// 2 modes: REALTIME,VIDEO
#define REALTIME

int main(int argc, char** argsv)
{
	char detectionModel[] = "../models/DetectionModel-v1.5.bin";
	char trackingModel[]  = "../models/TrackingModel-v1.10.bin";
	string faceDetectionModel("../models/haarcascade_frontalface_alt2.xml");

	// initialize a XXDescriptor object
	INTRAFACE::XXDescriptor xxd(4);
	// initialize a FaceAlignment object
	INTRAFACE::FaceAlignment fa(detectionModel, trackingModel, &xxd);
	if (!fa.Initialized()) {
		cerr << "FaceAlignment cannot be initialized." << endl;
		return -1;
	}
	// load OpenCV face detector model
	cv::CascadeClassifier face_cascade;
	if( !face_cascade.load( faceDetectionModel ) )
	{ 
		cerr << "Error loading face detection model." << endl;
		return -1; 
	}

	char* logFileName= argsv[1];
	ofstream logFile;
	logFile.open((char*)logFileName, fstream::out);
	// load the classifiers
	logFile << "Loading Eye Classifier: " << endl;
	std::vector<float> eye_classifier= bayes_parser("AUEyeClassifier.txt");
	string eyeAUNames[]= {"LidRaiser","LidTightener","Neutral"};
	logFile << "Loading Eyebrow Classifier: " << endl ;
	std::vector<float> eyebrow_classifier= bayes_parser("AUEyebrowClassifier.txt");
	string eyeBrowAUNames[]= {"BrowRaiser","BrowLowerer","Neutral"};

#ifdef REALTIME
	// use the first camera it finds
	cv::VideoCapture cap(0); 
#endif 

#ifdef VIDEO
	string filename("../data/vid.wmv");
	cv::VideoCapture cap(filename); 
#endif

	if(!cap.isOpened())  
		return -1;

	int key = 0;
	bool isDetect = true;
	bool eof = false;
	float score, notFace = 0.3;
	cv::Mat X;
	char* landmarksFileName= argsv[2];
	
	cvNamedWindow("AU Detector",CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback( "AU Detector",onMouse,landmarksFileName );
	int samplingFrame=1;
	while (key!=27) // Press Esc to quit
	{
		cv::Mat cvFrame;
		cap >> cvFrame; // get a new frame from camera
		while (cvFrame.rows == 0 || cvFrame.cols == 0)
		{
			cap >> cvFrame;
		}

		if (isDetect)
		{
			// face detection
			vector<cv::Rect> faces;
			face_cascade.detectMultiScale(cvFrame, faces, 1.2, 2, 0, cv::Size(50, 50));
			// if no face found, do nothing
			if (faces.empty()) {
				//cv::imshow(winname,frame);
				key = cv::waitKey(5);
				continue ;
			}
			// facial feature detection on largest face found
			if (fa.Detect(cvFrame,*max_element(faces.begin(),faces.end(),compareRect),cvCurrent,score) != INTRAFACE::IF_OK)
				break;
			isDetect = false;
		}
		else
		{
			// facial feature tracking
			if (fa.Track(cvFrame,cvCurrent,X,score) != INTRAFACE::IF_OK)
				break;
			cvCurrent = X;
		}
		if (score < notFace) // detected face is not reliable
			isDetect = true;
		else
		{
			// if the neutral frame is already selected then calculate the features
			// for the current frame
			if(neutralSelected==true && samplingFrame % 30 == 0){
				// write the landmarks to file
				ofstream landmarksFile;
				landmarksFile.open((char*)landmarksFileName,fstream::in | fstream::out | fstream::app);
				for (int i = 0 ; i < cvCurrent.cols ; i++){
					landmarksFile << (int)cvCurrent.at<float>(0,i)<<"," <<(int)cvCurrent.at<float>(1,i) ;
					if(i<cvCurrent.cols-1)
						landmarksFile << ",";
				}
				landmarksFile << endl;
				landmarksFile.close();
				std::vector<float> final_features_l;
				std::vector<float> final_features_r;
				logFile << "Loaded Landmarks" << endl;
				// extract features
				current = new frame(cvCurrent);
				current->rotate();
				current->extract_eye_region_landmarks();
				current->calc_centroids();
				logFile << "Calculated centroids" << endl;
				current->l_eye_feats= current->extract_eye_feat(neutral->l_centroid, current->l_eye_region_landmks);
				current->r_eye_feats= current->extract_eye_feat(neutral->r_centroid, current->r_eye_region_landmks);
				final_features_l = frame::eye_feat_ratio(current->l_eye_feats, neutral->l_eye_feats);
				final_features_r = frame::eye_feat_ratio(current->r_eye_feats, neutral->r_eye_feats);
				final_features_l.insert(final_features_l.end(), final_features_r.begin(), final_features_r.end());
				std::vector<float> eye_features = final_features_l;
				logFile << "Extracted Eye Features" << endl;
				std::vector<float> logprob_vec = frame::calc_probs(eye_classifier, eye_features);
				logFile << "Classified Eye Class" << endl;
				int maxClass= findBestClass(logprob_vec);
				string probsString = "[";
				for(int c=0; c< logprob_vec.size(); c++){
					ostringstream tempStr;
					tempStr << logprob_vec[c]; 
					probsString += tempStr.str() + ",";
				}
				probsString += "] ";
				string eyeAU= eyeAUNames[maxClass];
				logFile << "Eye AU: " << probsString << eyeAU << endl;
				imageText = probsString + " " + eyeAU ;

				current->l_eyebrow_feats= current->extract_eyebrow_feat(neutral->l_centroid, current->l_eye_region_landmks);
				current->r_eyebrow_feats= current->extract_eyebrow_feat(neutral->r_centroid, current->r_eye_region_landmks);
				final_features_l = frame::eyebrow_feat_ratio(current->l_eyebrow_feats, neutral->l_eyebrow_feats);
				final_features_r = frame::eyebrow_feat_ratio(current->r_eyebrow_feats, neutral->r_eyebrow_feats);
				final_features_l.insert(final_features_l.end(), final_features_r.begin(), final_features_r.end());
				std::vector<float> eyebrow_features = final_features_l;
				logprob_vec = frame::calc_probs(eyebrow_classifier, eyebrow_features);
				maxClass= findBestClass(logprob_vec);
				string eyebrowAU= eyeBrowAUNames[maxClass];
				probsString = "[";
				for(int c=0; c< logprob_vec.size(); c++){
					ostringstream tempStr;
					tempStr << logprob_vec[c]; 
					probsString += tempStr.str() + ",";
				}
				probsString += "] ";
				logFile << "Eyebrow AU: " << probsString << eyebrowAU << endl;
				imageText += probsString + " " + eyebrowAU ;
				samplingFrame=1;
			}
			else{
				samplingFrame++;
			}
			// plot facial landmarks and write text
			for (int i = 0 ; i < cvCurrent.cols ; i++)
				cv::circle(cvFrame,cv::Point((int)cvCurrent.at<float>(0,i), (int)cvCurrent.at<float>(1,i)), 1, cv::Scalar(0,255,0), -1);
			cv::putText(cvFrame,imageText,cv::Point(20,20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,0,0),2,8);
		}
		cv::imshow("AU Detector",cvFrame);	
		key = cv::waitKey(5);
	}

	logFile.close();
	return 0;

}






