#pragma once
//#pragma comment(lib,"opencv_world411d.lib")
#pragma comment(lib,"opencv_world411.lib")

#include<vector>
#include<opencv2/opencv.hpp>
//����켣����Ϊ32
#define PATH_LENGTH 32


using namespace std;

/*@ brief���������������
@param	contours ����������vector
@return area	 �����������������vector
*/
vector<double> calc_area_by_contours(vector<vector<cv::Point>> contours);

/*@brief ͼ��Ԥ����
@param	*frame		 �ȴ�Ԥ�����֡��ֱ�Ӷ�������޸�
@param	color_lower  hsv���½�
@param	color_upper	 hsv���Ͻ�
@return mask		 ��ֵͼ������ɫ��Χ��Ϊ1������0
*/
cv::Mat pre_proc(cv::Mat *frame, cv::Scalar color_lower, cv::Scalar color_upper);

/*@brief �ҵ��������������������������vector��ɾ��
@param	*contours		������ı߽�
@param	*contours_area  ����������
@param	*center			��õĵ�ǰ��������С��ΧԲ��Բ��
@param	*radius			��õĵ�ǰ��������С��ΧԲ�İ뾶
*/
void find_max_area(vector<vector<cv::Point>> *contours, vector<double> *contours_area,
	cv::Point *center, int *radius);

/*@brief ����·��
@param	frame		������ͼ
@param	center_path �洢Բ��·���Ķ���
@return	frame		���õ�ͼ
*/
cv::Mat draw_path(cv::Mat frame, deque<cv::Point> center_path);
