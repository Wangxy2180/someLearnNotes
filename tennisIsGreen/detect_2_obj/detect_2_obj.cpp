//#pragma comment(lib,"opencv_world411d.lib")
////#pragma comment(lib,"opencv_world411.lib")

#include<iostream>
#include<string>
#include<deque>
#include<math.h>
#include<ctime>
#include"find_obj_by_color.h"

using namespace std;

//����hsv�ռ��µġ�tennis����ɫ�ʷ�Χ
//cv::Scalar green_lower = cv::Scalar(29, 86, 6);
//cv::Scalar green_upper = cv::Scalar(64, 255, 255);
//�����ɫ������Ͳ����ˣ�����ֽ��ͻ�ľ����
//�̻�ľ��Ҫ��ɫ����
cv::Scalar green_lower = cv::Scalar(35, 43, 46);
cv::Scalar green_upper = cv::Scalar(77, 255, 255);

//�ٶ��ϵĺ죬̫����
//cv::Scalar red_lower_0 = cv::Scalar(0, 43, 46);
//cv::Scalar red_upper_0 = cv::Scalar(10, 255, 255);
//cv::Scalar red_lower_0 = cv::Scalar(156, 43, 46);
//cv::Scalar red_upper_0 = cv::Scalar(180, 255, 255);
//stackoverflow�ϵĺ�1
//cv::Scalar red_lower_0 = cv::Scalar(0, 70, 50);
//cv::Scalar red_upper_0 = cv::Scalar(10, 255, 255);
//cv::Scalar red_lower_180 = cv::Scalar(170, 70, 50);
//cv::Scalar red_upper_180 = cv::Scalar(180, 255, 255);
//stackoverflow�ϵĺ�2
//cv::Scalar red_lower_0 = cv::Scalar(0, 100, 100);
//cv::Scalar red_upper_0 = cv::Scalar(10, 255, 255);
//cv::Scalar red_lower_180 = cv::Scalar(160, 100, 100);
//cv::Scalar red_upper_180 = cv::Scalar(179, 255, 255);
//stackoverflow�ϵĺ�3
cv::Scalar red_lower_0 = cv::Scalar(0, 135,135);
cv::Scalar red_upper_0 = cv::Scalar(20, 255, 255);
cv::Scalar red_lower_180 = cv::Scalar(159, 135, 135);
cv::Scalar red_upper_180 = cv::Scalar(179, 255, 255);


//��ľ��
cv::Scalar blue_lower = cv::Scalar(100, 43, 46);
cv::Scalar blue_upper = cv::Scalar(124, 255, 255);



int main()
{
	cv::VideoCapture capture(0);
	if (!capture.isOpened())
		return -1;

	//���ĵ��·������
	deque<cv::Point> center_path;

	clock_t start_time;
	int frame_count = 1;
	while(1)
	{
		cout << "frame  " << frame_count++ << endl;
		start_time = clock();

		cv::Mat frame;
		capture >> frame;//�������ȡ��һ֡
		//��1280*720resizeΪ640*360����������������,�ٵ�320*180�����������ԡ�
		//����������Ҫ������ת��ʱ����һ��
		cv::resize(frame, frame, cv::Size(640, 360));
		//cv::resize(frame, frame, cv::Size(320, 180));

		cv::Mat mask_green;
		mask_green= pre_proc(frame, green_lower, green_upper);
		//������
		vector<vector<cv::Point>> contours_green;
		cv::findContours(mask_green, contours_green, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		if (!contours_green.empty())
		{
			//�����������������
			vector<double> contours_green_area_vec = calc_area_by_contours(contours_green);

			//����������������Բ�ĺͰ뾶
			cv::Point center;
			int radius = 0;
			find_max_area(&contours_green, &contours_green_area_vec, &center, &radius);


			cout << "obj1 " << center.x << " " << center.y << " " << radius << endl;

			if (radius > 1)
			{
				//�߽��Բ��
				cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 255), 2);
				cv::circle(frame, center, 5, cv::Scalar(0, 0, 255), -1);
			}

			string text1 = "(x1,y1) (" + to_string(center.x) + "," + to_string(center.y) + ")";
			cv::putText(frame, text1, cv::Point(10, 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));
			
			//��Բ�ļ�����У�������ͼ
			center_path.push_front(center);
			if (center_path.size() > PATH_LENGTH)center_path.pop_back();
		}//end if (!contours.empty())

		cv::Mat mask_blue;
		//mask_blue = pre_proc(frame, blue_lower, blue_upper);
		mask_blue = pre_proc(frame, red_lower_0, red_upper_0,true,red_lower_180,red_upper_180);
		vector<vector<cv::Point>> contours_blue;
		cv::findContours(mask_blue, contours_blue, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		if (!contours_blue.empty())
		{
			//�����������������
			vector<double> contours_blue_area_vec = calc_area_by_contours(contours_blue);

			//�ټ���һ��
			cv::Point center_2;
			int radius_2 = 0;
			find_max_area(&contours_blue, &contours_blue_area_vec, &center_2, &radius_2);

			if (radius_2 > 1)
			{
				cv::circle(frame, center_2, (int)radius_2, cv::Scalar(0, 255, 255), 2);
				cv::circle(frame, center_2, 5, cv::Scalar(0, 0, 255), -1);
			}

			string text2 = "(x2,y2) (" + to_string(center_2.x) + "," + to_string(center_2.y) + ")";
			cv::putText(frame, text2, cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));

			cout << "obj2 " << center_2.x << " " << center_2.y << " " << radius_2 << endl;
		}
		
		//frame = draw_path(frame, center_path);

		cv::imshow("tennisIsGreen ������˳�", frame);
		if (cv::waitKey(1) >= 0)break;

		cout << "����ʱ��Ϊ" << (double)(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "--------------------------------------" << endl;
	}
}
