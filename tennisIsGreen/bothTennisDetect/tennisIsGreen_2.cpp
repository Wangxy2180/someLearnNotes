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
cv::Scalar tennis_lower = cv::Scalar(29, 86, 6);
cv::Scalar tennis_upper = cv::Scalar(64, 255, 255);
//���������Ͳ����ˣ�����ֽ��ͻ�ľ����
cv::Scalar green_lower = cv::Scalar(35, 43, 46);
cv::Scalar green_upper = cv::Scalar(77, 255, 255);
////��ľ�� ���޸�
//cv::Scalar red_lower = cv::Scalar(35, 43, 46);
//cv::Scalar red_upper = cv::Scalar(77, 255, 255);
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

		cv::Mat mask;
		mask= pre_proc(&frame, green_lower, green_upper);
		//������
		vector<vector<cv::Point>> contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		if (!contours.empty())
		{
			//�����������������
			vector<double> contours_area_vec = calc_area_by_contours(contours);

			//����������������Բ�ĺͰ뾶
			cv::Point center;
			int radius = 0;
			find_max_area(&contours, &contours_area_vec, &center, &radius);

			//�ټ���һ��
			cv::Point center_2;
			int radius_2 = 0;
			find_max_area(&contours, &contours_area_vec, &center_2, &radius_2);

			cout << "obj1 " << center.x << " " << center.y << " " << radius << endl
				<< "obj2 " << center_2.x << " " << center_2.y << " " << radius_2 << endl;

			if (radius > 1)
			{
				//�߽��Բ��
				cv::circle(frame, center, (int)radius, cv::Scalar(0, 255, 255), 2);
				cv::circle(frame, center, 5, cv::Scalar(0, 0, 255), -1);
			}
			//�ڶ���
			if (radius_2 > 1)
			{
				cv::circle(frame, center_2, (int)radius_2, cv::Scalar(0, 255, 255), 2);
				cv::circle(frame, center_2, 5, cv::Scalar(0, 0, 255), -1);
			}

			string text1 = "(x1,y1) (" + to_string(center.x) + "," + to_string(center.y) + ")";
			cv::putText(frame, text1, cv::Point(10, 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));
			string text2 = "(x2,y2) (" + to_string(center_2.x) + "," + to_string(center_2.y) + ")";
			cv::putText(frame, text2, cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));

			//��Բ�ļ�����У�������ͼ
			center_path.push_front(center);
			if (center_path.size() > PATH_LENGTH)center_path.pop_back();
		}//end if (!contours.empty())
		
		frame = draw_path(frame, center_path);

		cv::imshow("tennisIsGreen ������˳�", frame);
		if (cv::waitKey(1) >= 0)break;

		cout << "����ʱ��Ϊ" << (double)(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "--------------------------------------" << endl;
	}
}
