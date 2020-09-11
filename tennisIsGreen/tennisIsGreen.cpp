//#pragma comment(lib,"opencv_world411d.lib")
#pragma comment(lib,"opencv_world411.lib")

#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<deque>
#include<math.h>
#include<ctime>
//����켣����Ϊ32
#define PATH_LENGTH 32

using namespace std;

//����hsv�ռ��µġ���ɫ����ɫ�ʷ�Χ
cv::Scalar green_lower = cv::Scalar(29, 86, 6);
cv::Scalar green_upper = cv::Scalar(64, 255, 255);

//�ƺ�û����
typedef struct{
	int x;
	int y;
}posXY;

clock_t start_time, end_time, whole_time;

int main()
{
	int path_counter = 0;
	int dX = 0;
	int dY = 0;
	string direction = "";
	//cout << green_higher[1] << endl;
	cv::VideoCapture capture(0);

	if (!capture.isOpened())
		return -1;

	deque<cv::Point> center_path;

	//���������棬��Ȼ����û��Ч��
	//�ֱ�������˹ģ����ɫ�ʿռ�ת������ʴ���Ͳ�����?�ɲ����Ըĳɿ�������
	cv::Mat blurred;
	cv::Mat hsv;
	cv::Mat mask;
		
	int boom_count = 0;
	while (1)
	{
		cout << "boom  " << boom_count++ << endl;
		start_time = clock();
		whole_time = clock();
		//��ȡ��Ԥ����
		clock_t read_time = clock();
		cv::Mat frame;
		capture >> frame;//�������ȡ��һ֡
		//��1280*720resizeΪ640*360���������������ܣ����ǻ�
		clock_t resize_time = clock();
		cv::resize(frame, frame, cv::Size(640, 360));
		//cv::resize(frame, frame, cv::Size(320, 180));
		cout << "frame�ĳߴ���" << frame.rows << "*" << frame.cols << "*" << frame.channels() << endl;
		cout << "*��ȡ��resizeʱ��Ϊ" << (double)(clock() - read_time) / CLOCKS_PER_SEC << "s" << endl;
		//cv::Mat blurred;
		clock_t pre_proc = clock();
		//�Ѹ�˹�˳ߴ��11��Ϊ5���ܽ�ʡ0.045s
		cv::GaussianBlur(frame, blurred, cv::Size(11, 11),0);
		//cv::GaussianBlur(frame, blurred, cv::Size(5, 5),0);
		cout << "*resize+blur time" << (double)(clock() - resize_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "*blur time" << (double)(clock() - pre_proc) / CLOCKS_PER_SEC << "s" << endl;
		cout << "**������ģ��ʱ��Ϊ" << (double)(clock() - pre_proc) / CLOCKS_PER_SEC << "s" << endl;

		cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);
		cout << "*ģ����hsvʱ��Ϊ" << (double)(clock() - pre_proc) / CLOCKS_PER_SEC << "s" << endl;

		clock_t mask_time = clock();
		cv::inRange(hsv, green_lower, green_upper, mask);
		//�������飬����ʱ����
		//cv::erode(mask, mask, NULL, cv::Point(-1, -1), 2);
		//cv::dilate(mask, mask, NULL, cv::Point(-1, -1), 2);
		cv::morphologyEx(mask, mask, cv::MORPH_OPEN, NULL, cv::Point(-1, -1), 2);
		end_time = clock();
		cout << "*mask�׶ε�ʱ����" << (double)(end_time - mask_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "��ȡ��Ԥ����׶ε�ʱ����" << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;
		
		start_time = clock();
		//������
		vector<vector<cv::Point>> contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		end_time = clock();
		cout << "������ʱ��Ϊ" << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;

		cv::Point center_int;

		start_time = clock();
		//��������ı߽�
		if (contours.size()>0)
		{
			//��������ı߽磬�û���Python
			//�����������������
			clock_t max_time = clock();
			vector<double> contours_area_vec;
			for (int i = 0; i < contours.size(); i++)
			{
				double temp_area;
				temp_area = cv::contourArea(contours[i]);
				contours_area_vec.push_back(temp_area);
			}
			//��������±�
			int max_index = max_element(contours_area_vec.begin(), contours_area_vec.end())-contours_area_vec.begin();
			//�����������
			vector<cv::Point> max_contour = contours[max_index];
			cout << "*max_time "<< (double)(clock() - max_time) / CLOCKS_PER_SEC<<"s"<<endl;
			cv::Point2f center;
			float radius;
			cv::minEnclosingCircle(max_contour, center, radius);
			
			center_int.x = center.x;
			center_int.y = center.y;
			if (radius > 10)
			{
				//�߽��Բ��
				cv::circle(frame, center_int, (int)radius,cv::Scalar(0,255,255),2);
				cv::circle(frame, center_int, 5,cv::Scalar(0,0,255),-1);
				string text = "(x,y) (" + to_string(center_int.x) + "," + to_string(center_int.y)+")";
				cv::putText(frame, text, cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));
			}
		}
		end_time = clock();
		cout << "��������߽�ʱ��Ϊ" << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;

		center_path.push_front(center_int);
		if (center_path.size() > PATH_LENGTH)center_path.pop_back();

		start_time = clock();
		//���켣
		for (int i = 1;i<center_path.size();i++)
		{
			if (center_path.empty())continue;

			//cout << "111" << endl;
			int tickness = sqrt(PATH_LENGTH / (i + 1.0)*2.5);
			//cout << tickness << endl;
			cv::line(frame, center_path[i - 1], center_path[i], cv::Scalar(0, 255, 255), tickness);
		}

		cv::imshow("tennisIsGreen ������˳�", frame);
		end_time = clock();
		cout << "���켣ʱ��Ϊ" << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;
		
		start_time = clock();
		if (cv::waitKey(1)>=0)break;
		end_time = clock();
		cout << "waitʱ��Ϊ" << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "����ʱ��Ϊ" << (double)(end_time - whole_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "--------------------------------------" << endl;

	}
}
