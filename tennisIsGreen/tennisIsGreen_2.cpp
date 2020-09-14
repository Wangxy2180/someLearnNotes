//#pragma comment(lib,"opencv_world411d.lib")
#pragma comment(lib,"opencv_world411.lib")

#include<iostream>
#include<string>
#include<deque>
#include<math.h>
#include<ctime>
#include<opencv2/opencv.hpp>
//����켣����Ϊ32
#define PATH_LENGTH 32

using namespace std;

//����hsv�ռ��µġ���ɫ����ɫ�ʷ�Χ
cv::Scalar green_lower = cv::Scalar(29, 86, 6);
cv::Scalar green_upper = cv::Scalar(64, 255, 255);

int main()
{
	cv::VideoCapture capture(0);

	if (!capture.isOpened())
		return -1;

	//���ĵ��·������
	deque<cv::Point> center_path;

	//���������棬��Ȼ����û��Ч��
	//�ֱ�������˹ģ����ɫ�ʿռ�ת������ʴ���Ͳ�����?�ɲ����Ըĳɿ�������
	cv::Mat blurred;
	cv::Mat hsv;
	cv::Mat mask;

	clock_t start_time;
	int frame_count = 1;
	while(1)
	{
		cout << "frame  " << frame_count++ << endl;
		start_time = clock();
		//��ȡ��Ԥ����
		cv::Mat frame;
		capture >> frame;//�������ȡ��һ֡
		
		//��1280*720resizeΪ640*360����������������,�ٵ�320*180�����������ԡ�
		//����������Ҫ������ת��ʱ����һ��
		cv::resize(frame, frame, cv::Size(640, 360));
		//cv::resize(frame, frame, cv::Size(320, 180));

		//�Ѹ�˹�˳ߴ��11��Ϊ5������ܽ�ʡ0.004s������Ҳ���á�
		cv::GaussianBlur(frame, blurred, cv::Size(11, 11), 0);
		//cv::GaussianBlur(frame, blurred, cv::Size(5, 5),0);

		cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);

		//��ֵ������ֵ�ڰ�ɫ��������ɫ
		cv::inRange(hsv, green_lower, green_upper, mask);

		//�������飬����ʱ���࣬0.001s����
		//��ʵ�ϣ�������һ�����������Ҳûɶ���ԭ���õ�None����֪��Ϊɶ������
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
		//cv::erode(mask, mask, element, cv::Point(-1, -1), 2);
		//cv::dilate(mask, mask, element, cv::Point(-1, -1), 2);
		cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element, cv::Point(-1, -1), 2);

		//������
		vector<vector<cv::Point>> contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		//��������ı߽磬����if0.001s��������
		if (contours.size() > 0)
		{
			//��ѭ�������������������
			vector<double> contours_area_vec;
			for (int i = 0; i < contours.size(); i++)
			{
				double temp_area;
				temp_area = cv::contourArea(contours[i]);
				contours_area_vec.push_back(temp_area);
			}

			//��������±�
			int max_index = max_element(contours_area_vec.begin(), contours_area_vec.end()) - contours_area_vec.begin();
			//�����������
			vector<cv::Point> max_contour = contours[max_index];

			//��С�ڽ�Բ���ĵ�Ͱ뾶
			cv::Point2f center;
			float radius;
			cv::minEnclosingCircle(max_contour, center, radius);

			//�����洢center��int������ֵ
			cv::Point center_int;
			center_int.x = center.x;
			center_int.y = center.y;

			//����ж�Ҫ��Ҫɾ����
			if (radius > 10)
			{
				//�߽��Բ��
				cv::circle(frame, center_int, (int)radius, cv::Scalar(0, 255, 255), 2);
				cv::circle(frame, center_int, 5, cv::Scalar(0, 0, 255), -1);
			}
			string text = "(x,y) (" + to_string(center_int.x) + "," + to_string(center_int.y) + ")";
			cv::putText(frame, text, cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));

			//��Բ�ļ�����У�������ͼ
			center_path.push_front(center_int);
			if (center_path.size() > PATH_LENGTH)center_path.pop_back();

		}//end if (contours.size() > 0)

		//���켣
		for (int i = 1; i < center_path.size(); i++)
		{
			if (center_path.empty())continue;

			//cout << "111" << endl;
			int tickness = sqrt(PATH_LENGTH / (i + 1.0)*2.5);
			//cout << tickness << endl;
			cv::line(frame, center_path[i - 1], center_path[i], cv::Scalar(0, 255, 255), tickness);
		}

		cv::imshow("tennisIsGreen ������˳�", frame);
		if (cv::waitKey(1) >= 0)break;

		cout << "����ʱ��Ϊ" << (double)(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "--------------------------------------" << endl;
	}
}
