#include <stdio.h>
#include <Kinect.h>
#include <windows.h>
#include<opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strstream>
#include <Eigen/Dense>
#include<Eigen/Core> 
#include <iostream>
#include <array>

using namespace cv;
using namespace std;
using namespace Eigen;


//����hsv�ռ��µġ���ɫ����ɫ�ʷ�Χ
cv::Scalar green_lower = cv::Scalar(35, 83, 86);
cv::Scalar green_upper = cv::Scalar(77, 255, 255);

int main()
{
	//���崫����ָ��
	IKinectSensor*          m_pKinectSensor = NULL;

	//�����ɫ����Դָ��Ͳ�ɫ���ݶ�ȡָ��
	IDepthFrameSource*      pDepthFrameSource = NULL;
	IDepthFrameReader*      m_pDepthFrameReader = NULL;


	//�����������Դָ���������ݶ�ȡָ��
	IColorFrameSource*      ColorFrameSource = NULL;
	IColorFrameReader*      ColorFrameReader = NULL;


	//����ͼƬ����ָ��
	IFrameDescription*      FrameDescription = NULL;

	//��ȡ������
	GetDefaultKinectSensor(&m_pKinectSensor);

	//�򿪴�����
	m_pKinectSensor->Open();

	m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);

	m_pKinectSensor->get_ColorFrameSource(&ColorFrameSource);

	pDepthFrameSource->OpenReader(&m_pDepthFrameReader);

	ColorFrameSource->OpenReader(&ColorFrameReader);

	clock_t start_time;
	int img_count = 1;

	while (1)
	{
		cout << "frame  " << img_count++ << endl;
		start_time = clock();
		// �õ����µ�һ֡�������
		IDepthFrame*          pDepthFrame = NULL;

		// �õ����µ�һ֡��ɫ����
		IColorFrame*          pColorFrame = NULL;

		//���ͼ��ȡ
		while (pDepthFrame == NULL){
			m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
		}
		pDepthFrame->get_FrameDescription(&FrameDescription);
		int depth_width, depth_height;

		FrameDescription->get_Width(&depth_width);
		FrameDescription->get_Height(&depth_height);

		Mat DepthImg(depth_height, depth_width, CV_16UC1);
		UINT uDepthPointNum = depth_width * depth_height;

		Mat DepthImg8(depth_height, depth_width, CV_8UC1);

		pDepthFrame->CopyFrameDataToArray(uDepthPointNum, (UINT16 *)DepthImg.data);

		//ת��Ϊ8λͼ��,convertTo(Ŀ�����
		DepthImg.convertTo(DepthImg8, CV_8UC1, 255.0 / 4500);



		//��ɫͼ��ȡ
		while (pColorFrame == NULL){
			ColorFrameReader->AcquireLatestFrame(&pColorFrame);
		}
		pColorFrame->get_FrameDescription(&FrameDescription);
		int CWidth, CHeight;

		FrameDescription->get_Width(&CWidth);
		FrameDescription->get_Height(&CHeight);

		cout << CWidth << CHeight << endl;

		Mat ColorImg(CHeight, CWidth, CV_8UC4);
		UINT uColorPointNum = CWidth * CHeight;
		UINT uColorBufferSize = uColorPointNum * 4 * sizeof(unsigned char);

		pColorFrame->CopyConvertedFrameDataToArray(uColorBufferSize, (BYTE *)ColorImg.data, ColorImageFormat_Bgra);

		ICoordinateMapper* pCoordinateMapper = nullptr;
		if (m_pKinectSensor->get_CoordinateMapper(&pCoordinateMapper) != S_OK)
		{
			cerr << "get_CoordinateMapper failed" << endl;
			return -1;
		}

		UINT32 uTableSize = 0;
		PointF* aTable = nullptr;
		auto res = pCoordinateMapper->GetDepthFrameToCameraSpaceTable(&uTableSize, &aTable);
		// Enter main loop
		DepthSpacePoint* pPointArray = new DepthSpacePoint[uColorPointNum];

		pCoordinateMapper->MapColorFrameToDepthSpace(uDepthPointNum, reinterpret_cast<UINT16*>(DepthImg.data), uColorPointNum, pPointArray);

		cv::Mat blurred;
		cv::Mat hsv;
		cv::Mat mask;

		//���سߴ�
		cv::resize(ColorImg, ColorImg, Size(480, 270));

		//��˹�˲�ȥ��
		cv::GaussianBlur(ColorImg, blurred, cv::Size(5, 5), 0);

		//ת����hsvɫ�ʿռ�
		cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);

		//��ֵ������ֵ�ڰ�ɫ��������ɫ
		cv::inRange(hsv, green_lower, green_upper, mask);

		// ������(open)���ȸ�ʴ�����͵Ĺ���,ƽ���ϴ�����ı߽��   ͬʱ�������Ըı��������
		cv::morphologyEx(mask, mask, cv::MORPH_OPEN, NULL, cv::Point(-1, -1), 2);

		//������
		vector<vector<cv::Point>> contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		cv::Point2f center1;
		cv::Point center1_int;
		float radius1;

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

			//������������С�ڽ�Բ���ĵ�Ͱ뾶
			cv::minEnclosingCircle(max_contour, center1, radius1);

			//�����洢center��int������ֵ
			center1_int.x = center1.x;
			center1_int.y = center1.y;

			//������߽��Բ��
			cv::circle(ColorImg, center1_int, (int)radius1, cv::Scalar(0, 255, 255), 2);
			cv::circle(ColorImg, center1_int, 5, cv::Scalar(0, 0, 255), -1);

			//��ʾ��ɫͼ��Բ������
			string text1 = "(x,y) (" + to_string(center1_int.x) + "," + to_string(center1_int.y) + ")";
			cv::putText(ColorImg, text1, cv::Point(10, 30), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 255));
			cout << center1 << endl;
			cout <<"real xy in rgb "<<"center1.x=" << center1_int.x*4 << "   " << "center1.y=" << center1_int.y *4<< endl;
	

		}
		//��ʾһ�����ֵ
		//��Ϊkinect��������ͼ���ݸ�ʽ��CV_16UC1����˻���������Ҳ��ԭʼ��16���ݸ�ʽ

		/*	int z = DepthImg.at<unsigned short>(center1_int.x, center1_int.y);
		cout << z << endl;*/


		int a = 0;
		int b = 0;
		int z = 0;
		//��ʼ����
			cout << pPointArray[center1_int.y*4* 1920 + center1_int.x*4].X << " " << pPointArray[center1_int.y*4 * 1920 + center1_int.x*4].Y << "  " << endl;

			a = (int)(pPointArray[center1_int.y *4* 1920 + center1_int.x*4].X);
			b = (int)(pPointArray[center1_int.y *4* 1920 + center1_int.x*4].Y);
			cout << a << b << endl;


			if (a >= 0 && a < DepthImg.cols && b >= 0 && b < DepthImg.rows)
			{
				z = DepthImg.at<unsigned short>(cv::Point(a, b));
				cout << z << endl;
			}
			else z = 0;

		Matrix3d k;

		//1920*1080ԭ�ߴ��µ�k����
		// ��Ӧrgbͼ�ߴ����Ŷ��٣��͸��ڲ����Ŷ��٣���0,0����0,2����Ӧx�����ţ���1,1����1,2����Ӧy������
		/*k << 1054.1, 0, 960,
			0, 1053.7, 540,
			0, 0, 1;
*/
		k << 263.525, 0, 240,
			0, 263.425, 135,
			0, 0, 1;

		Eigen::MatrixXd uv(3, 1);
		uv(0, 0) = z*center1.x;
		uv(1, 0) = z*center1.y;
		uv(2, 0) = z;

		Eigen::MatrixXd coord(3, 1);
		coord = k.inverse()*uv;
		cout << "�������ȴ�����(����С��������λ�ã���xyz������Ϊ��λ��"<<endl;
		cout << coord << endl;

		//��ʾͼ��
		cv::imshow("depthImg", DepthImg8);
		cv::imshow("ColorImg", ColorImg);
		delete pPointArray;
		DepthImg.release();
		DepthImg8.release();
		ColorImg.release();

		pDepthFrame->Release();
		pColorFrame->Release();

		waitKey(1);

		cout << "����ʱ��Ϊ" << (double)(clock() - start_time) / CLOCKS_PER_SEC << "s" << endl;
		cout << "--------------------------------------" << endl;
	}
}