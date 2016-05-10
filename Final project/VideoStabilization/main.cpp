/* main.cpp
//
// The video stabilization is an off-line process for the input video or image sequence, 
//	 major steps are descibed as following:
//
// 1. Get previous to current frame transformation (dx, dy, da) for all frames
// 2. Find fast-motion frames and low-motion frames       
// 3. Accumulate the transformations to get the image trjct
// 4. Smooth out the trjct using an averaging window
// 5. Generate new set of previous to current transform, such that the trjct ends up being the same as the smoothed trjct
// 6. Apply the new transformation to the video
//
//  Note: Input of the program is .mp4 video file, not .rgb sequence since adopting OpenCV video object for convenience
//
//  Created:  Apr 15, 2016
//  Author:     Hengyue Liu
*/
using namespace std;//place before <vector>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/videostab/global_motion.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/opencv.hpp>
#include "Image.h"
using namespace cv;
//Program parameters
double MOTION_THRESHOLD = 30; // threshold of fast/low motion frame
const int SMOOTHING_RADIUS = 30; // Window size in frames. The larger the more stable the video, but less reactive to sudden panning
const int HORIZONTAL_BORDER_CROP = 50; // In pixels. Crops the border to reduce the black borders from stabilization being too noticeable.
//define start frame and end frame, good for different scenes and testing
const int START_FRAME = 0;
const int END_FRAME = 500;
const int FRAME_RATE = 15;//frame rate

//define transformation parameters structure
struct transParams
{
	transParams() {}
	transParams(double _dx, double _dy, double _da) {
		dx = _dx;
		dy = _dy;
		da = _da;
	}

	double dx;
	double dy;
	double da; // angle
};

//trjct is the accumulation of  transformation
struct trjct
{
	trjct() {}
	trjct(double _x, double _y, double _a) {
		x = _x;
		y = _y;
		a = _a;
	}

	double x;
	double y;
	double a; // angle
};

//global variables
//paths
char FramePath[_MAX_PATH];
char AudioPath[_MAX_PATH];
//video dimension
int w = 480;
int h = 270;
MyImage inImage, outImage;//frame for processing
Mat cur_frame, pre_frame;//current frame & previous frame
Mat cur_frame_gray, pre_frame_gray;//current frame & previous frame in gray-level scale
int cur_idx=0;//current frame index
vector<Mat> video_buffer;//video buffer




	int main(int argc, char **argv)
	{
		//system input
		if (argc < 3) {
			cout << "./VideoStabilization video.rgb audio.wav" << endl;
			getchar();
			return false;
		}
		sscanf(argv[1], "%s", &FramePath, argv[2], "%s", &AudioPath);
		inImage.setWidth(w);
		inImage.setHeight(h);
		//argv[1] FramePath; argv[2] AudioPath
		// Display the main dialog box.
		int frame = 0;
		if (strstr(FramePath, ".rgb") == NULL)
		{
			cout << "Please enter .rgb file as input." << endl;
			getchar();
			return false;
		}
		else
		{
			inImage.setImagePath(FramePath);
			if (!inImage.ReadFrame(cur_idx))
			{
				cout << "error reading the input image sequence." << endl;
				getchar();
				return false;
			}
			else {
				outImage = inImage;
				//PlayVideo();
				cur_frame = Mat(h, w, CV_8UC3, outImage.getImageData());
			}

			//return true;
		}
		// Keep track of transforms and trajectories
		ofstream out_transform("prev_to_cur_transformation.txt");
		ofstream out_trjct("trjct.txt");
		ofstream out_smoothed_trjct("smoothed_trjct.txt");
		ofstream out_new_transform("new_prev_to_cur_transformation.txt");
		ofstream out_motion_distance("motion_distance.txt");

		//skip frames, start from START_FRAME
		outImage.setImagePath(FramePath);
		outImage.ReadFrame(START_FRAME);
		cur_frame = Mat(h, w, CV_8UC3, outImage.getImageData());
		//first frame
		cur_frame.copyTo(pre_frame);
		cvtColor(pre_frame, pre_frame_gray, COLOR_BGR2GRAY);//input of goodfeatures is gray-level scale

		/*************************************************************************
		  * STEP1 - Get previous to current frame transformation (dx, dy, da) for all frames  *
		  *************************************************************************/
		cout << "Video segment: FIRST FRAME - " << START_FRAME << ", LAST FRAME - " << END_FRAME << endl;
		cout << "Step 1: Start computing good features (strong corners) and sparse optical flow...";
		vector <transParams> prev_to_cur_transform; // previous to current
		vector<double> prev_to_cur_distance;//the magnitude of the motion vectors
		//int k ;//keep track of current frame index
		//int max_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
		//if (END_FRAME >= max_frames) {
		//	cout << "Then last frame is out of range." << endl;
		//	return -1;
		//}
		
		//LOOP for each frame 
		for (cur_idx = START_FRAME; cur_idx < END_FRAME; cur_idx++) {
			//cap >> cur;
			if (!outImage.ReadFrame(cur_idx)) {
				break;
			}
			cur_frame = Mat(h, w, CV_8UC3, outImage.getImageData());
			cvtColor(cur_frame, cur_frame_gray, COLOR_BGR2GRAY);

			// vector from prev to cur
			vector <Point2f> prev_corner, cur_corner;
			vector <Point2f> prev_corner2, cur_corner2;
			vector <uchar> status;
			vector <float> err;

			goodFeaturesToTrack(pre_frame_gray, prev_corner, 200, 0.01, 30);//maximum number of corners is 200; minimum Euclidean dis 30
			calcOpticalFlowPyrLK(pre_frame_gray, cur_frame_gray, prev_corner, cur_corner, status, err);//status is the indicator of if found flow

			// filter out bad matches
			for (size_t i = 0; i < status.size(); i++) {
				if (status[i]) {
					prev_corner2.push_back(prev_corner[i]);
					cur_corner2.push_back(cur_corner[i]);
				}
			}


			//Rigid Transformation matrix (translation + rotation only)
			//[  cos -sin x
			//    sin cos y  ]
			Mat T = estimateRigidTransform(prev_corner2, cur_corner2, false); // false = rigid transform, no scaling/shearing

			// decompose T
			double dx = T.at<double>(0, 2);
			double dy = T.at<double>(1, 2);
			double da = atan2(T.at<double>(1, 0), T.at<double>(0, 0));
			double dis = sqrt(dx*dx + dy*dy);
			//store for later processing
			prev_to_cur_transform.push_back(transParams(dx, dy, da));
			prev_to_cur_distance.push_back(dis);

			out_transform << cur_idx << " " << dx << " " << dy << " " << da << endl;
			out_motion_distance << cur_idx << " " << dis << endl;
			//update previous frame
			cur_frame.copyTo(pre_frame);
			cur_frame_gray.copyTo(pre_frame_gray);
			outImage.~MyImage();
		}
		cout << "Done!" << endl;
		waitKey(1000);
		

		/************************************************************************
		*                       STEP2 - Find fast-motion frames and low-motion frames                        *
		*************************************************************************/
		cout << "Step 2: Finding fast-motion frames and low-motion frames...";
		
		for (int j = 0; j < prev_to_cur_distance.size(); j++) {
			if (prev_to_cur_distance[j] > MOTION_THRESHOLD) {
				prev_to_cur_distance[j] = 1;//label as fast motion frame
				prev_to_cur_transform[j].dx = 0;
				prev_to_cur_transform[j].dy = 0;
			}
			else prev_to_cur_distance[j] = 0;//label as low motion
		}
		cout << "Done!" << endl;
		waitKey(1000);

		/************************************************************************
		*           STEP 3 - Accumulate the transformations to get the image trjct              *
		*************************************************************************/
		cout << "Step 3: Accumulate the transformations to get the image trjct ...";

		// Accumulated frame to frame transform
		double a = 0;
		double x = 0;
		double y = 0;

		vector <trjct> myTrjct; // trajectory at all frames

		for (size_t i = 0; i < prev_to_cur_transform.size(); i++) {
			x += prev_to_cur_transform[i].dx;
			y += prev_to_cur_transform[i].dy;
			a += prev_to_cur_transform[i].da;

			myTrjct.push_back(trjct(x, y, a));

			out_trjct << (i + 1) << " " << x << " " << y << " " << a << endl;
		}

		cout << "Done!" << endl;
		waitKey(1000);

		/************************************************************************
		*               STEP 4 -  Smooth out the trjct using an averaging window                   *
		*************************************************************************/
		cout << "Step 4: Smooth out the trjct using an averaging window ...";

		vector <trjct> smoothed_trjct; // smoothed trajectory at all frames

		for (size_t i = 0; i < myTrjct.size(); i++) {
			double sum_x = 0;
			double sum_y = 0;
			double sum_a = 0;
			int count = 0;

			for (int j = -SMOOTHING_RADIUS; j <= SMOOTHING_RADIUS; j++) {
				if (i + j >= 0 && i + j < myTrjct.size()) {
					sum_x += myTrjct[i + j].x;
					sum_y += myTrjct[i + j].y;
					sum_a += myTrjct[i + j].a;

					count++;
				}
			}

			double avg_a = sum_a / count;
			double avg_x = sum_x / count;
			double avg_y = sum_y / count;

			smoothed_trjct.push_back(trjct(avg_x, avg_y, avg_a));

			out_smoothed_trjct << (i + 1) << " " << avg_x << " " << avg_y << " " << avg_a << endl;
		}

		cout << "Done!" << endl;
		waitKey(1000);

		/*************************************************************************************************
		*               STEP 5 -  Generate new set of previous to current transforms based on smoothed trjct                   *
		*************************************************************************************************/
		cout << "Step 5: Generate new set of previous to current transforms based on smoothed trjct ...";

		vector <transParams> new_prev_to_cur_transform;

		// Accumulated frame to frame transform
		a = 0;
		x = 0;
		y = 0;

		for (size_t i = 0; i < prev_to_cur_transform.size(); i++) {
			x += prev_to_cur_transform[i].dx;
			y += prev_to_cur_transform[i].dy;
			a += prev_to_cur_transform[i].da;

			// difference to compensate
			double diff_x = smoothed_trjct[i].x - x;
			double diff_y = smoothed_trjct[i].y - y;
			double diff_a = smoothed_trjct[i].a - a;

			double dx = prev_to_cur_transform[i].dx + diff_x;
			double dy = prev_to_cur_transform[i].dy + diff_y;
			double da = prev_to_cur_transform[i].da + diff_a;

			new_prev_to_cur_transform.push_back(transParams(dx, dy, da));

			out_new_transform << (i + 1) << " " << dx << " " << dy << " " << da << endl;
		}

		cout << "Done!" << endl;
		waitKey(1000);

		/**************************************************************
		*               STEP 6 -  Apply the new transformation to the video                   *
		***************************************************************/
		cout << "Step 6: Apply the new transformation to the video ...";

		//cap.set(CV_CAP_PROP_POS_FRAMES, START_FRAME);
		cur_idx = START_FRAME;

		Mat T(2, 3, CV_64F);
		// based on the horizontal crop, getting the number of columns to crop (vertical) 
		int vertical_crop = HORIZONTAL_BORDER_CROP * pre_frame.rows / pre_frame.cols; 
		cout << "press key to start view the output" << endl;
		getchar();//waitKey(0);
		while (cur_idx < END_FRAME -1) { // ignore the last frame

			outImage.setImagePath(FramePath);
			if (!outImage.ReadFrame(cur_idx)) {
				break;
			}
			cur_frame = Mat(h, w, CV_8UC3, outImage.getImageData());

			//construct new transformation matrix
			T.at<double>(0, 0) = cos(new_prev_to_cur_transform[cur_idx-START_FRAME].da);
			T.at<double>(0, 1) = -sin(new_prev_to_cur_transform[cur_idx - START_FRAME].da);
			T.at<double>(1, 0) = sin(new_prev_to_cur_transform[cur_idx - START_FRAME].da);
			T.at<double>(1, 1) = cos(new_prev_to_cur_transform[cur_idx - START_FRAME].da);

			T.at<double>(0, 2) = new_prev_to_cur_transform[cur_idx - START_FRAME].dx;
			T.at<double>(1, 2) = new_prev_to_cur_transform[cur_idx - START_FRAME].dy;
			//apply affine transformation to current frame
			Mat cur_stable;
			warpAffine(cur_frame, cur_stable, T, cur_frame.size());

			cur_stable = cur_stable(Range(vertical_crop, cur_stable.rows - vertical_crop), Range(HORIZONTAL_BORDER_CROP, cur_stable.cols - HORIZONTAL_BORDER_CROP));

			//(optional) Resize cur_stable back to cur_frame size
			//resize(cur_stable, cur_stable, cur.size());

			// Draw image panel for comparision (10 cols margin added)
			Mat panel = Mat::zeros(cur_frame.rows, cur_frame.cols * 2 + 10, cur_frame.type());

			cur_frame.copyTo(panel(Range::all(), Range(0, cur_frame.cols)));
			cur_stable.copyTo(panel(Range(vertical_crop,cur_stable.rows+ vertical_crop), Range(HORIZONTAL_BORDER_CROP+ cur_frame.cols + 10, HORIZONTAL_BORDER_CROP+ cur_frame.cols+cur_stable.cols  + 10)));

			//video_buffer.push_back(panel);
			imshow("Original video & stabilized one", panel);

			//frame rate
			waitKey(500./FRAME_RATE);

			cur_idx++;
		}
		//play the video
		/*
		const char* new_video_name = "stabilized_video.rgb";
		outImage.setImagePath(new_video_name);
		//FILE *fs;
		//fs=fopen(new_video_name, "wb+");
		//FileStorage fs(new_video_name, FileStorage::WRITE);
		
		for (int i = 0; i < video_buffer.size();i++) {
			imshow("Original video & stabilized one", video_buffer[i]);
			//outImage.setImageData(video_buffer[i]);
			//outImage.WriteFrame(i);
			//waitKey(10);

			waitKey(1000. / FRAME_RATE);
		}
		//fs.release();
		*/
		cout << "All done!" << endl<<"Press any key to stop the program...";
		waitKey(0);
		return 0;
	}




