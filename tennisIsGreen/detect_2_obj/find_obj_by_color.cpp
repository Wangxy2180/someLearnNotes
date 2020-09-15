#include"find_obj_by_color.h"
/*@ brief���������������
@param	contours ����������vector
@return area	 �����������������vector
*/
vector<double> calc_area_by_contours(vector<vector<cv::Point>> contours)
{
	if (!contours.empty())
	{
		vector<double> area;
		for (int i = 0; i < contours.size(); i++)
		{
			//�������Ż���һ��
			double temp_area;
			temp_area = cv::contourArea(contours[i]);
			area.push_back(temp_area);
		}
		return area;
	}
}

/*@brief ͼ��Ԥ����
@param	*frame		 �ȴ�Ԥ�����֡��ֱ�Ӷ�������޸�
@param	color_lower  hsv���½�
@param	color_upper	 hsv���Ͻ�
@return mask		 ��ֵͼ������ɫ��Χ��Ϊ1������0
*/
cv::Mat pre_proc(cv::Mat frame, cv::Scalar color_lower, cv::Scalar color_upper, bool isRed,
	cv::Scalar red_lower, cv::Scalar red_upper)
{
	cv::Mat blurred;
	cv::Mat hsv;
	cv::Mat mask;
	

	//�Ѹ�˹�˳ߴ��11��Ϊ5������ܽ�ʡ0.004s������Ҳ���á�
	cv::GaussianBlur(frame, blurred, cv::Size(11, 11), 0);
	//cv::GaussianBlur(frame, blurred, cv::Size(5, 5),0);

	cv::cvtColor(blurred, hsv, cv::COLOR_BGR2HSV);
	//��ֵ������ֵ�ڰ�ɫ��������ɫ
	cv::inRange(hsv, color_lower, color_upper, mask);

	if (isRed)
	{//�����漼���ɣ���BGR����RGBȥ����Ȼ��BRͨ���ͻ������Ϳ�������ɫȥʶ��
		cv::Mat mask_red_180;
		cv::inRange(hsv, red_lower, red_upper, mask_red_180);
		cv::addWeighted(mask,1.0,mask,1.0,0.0,mask);
	}

	//�������飬����ʱ���࣬0.001s����
	//��ʵ�ϣ�������һ�����������Ҳûɶ���ԭ���õ�None����֪��Ϊɶ������
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::erode(mask, mask, element, cv::Point(-1, -1), 2);
	//cv::dilate(mask, mask, element, cv::Point(-1, -1), 2);
	cv::morphologyEx(mask, mask, cv::MORPH_OPEN, element, cv::Point(-1, -1), 2);

	return mask;
}

/*@brief �ҵ��������������������������vector��ɾ��
@param	*contours		������ı߽�
@param	*contours_area  ����������
@param	*center			��õĵ�ǰ��������С��ΧԲ��Բ��
@param	*radius			��õĵ�ǰ��������С��ΧԲ�İ뾶
*/
void find_max_area(vector<vector<cv::Point>> *contours, vector<double> *contours_area,
	cv::Point *center, int *radius)
{
	//�ܾ�����ôд������⣬���ǣ���������
	if (!(*contours).empty())
	{
		cv::Point2f cen;
		float rad;
		//�������������
		int max_index = max_element((*contours_area).begin(), (*contours_area).end()) - (*contours_area).begin();
		vector<cv::Point> max_contour = (*contours)[max_index];

		//�ĺ󼴷٣����꼴ɾ
		(*contours_area).erase((*contours_area).begin() + max_index);
		(*contours).erase((*contours).begin() + max_index);

		//��С�ڽ�Բ���ĵ�Ͱ뾶
		cv::minEnclosingCircle(max_contour, cen, rad);
		*center = cen;
		*radius = rad + 0.5;
	}
}

/*@brief ����·��
@param	frame		������ͼ
@param	center_path �洢Բ��·���Ķ���
@return	frame		���õ�ͼ
*/
cv::Mat draw_path(cv::Mat frame, deque<cv::Point> center_path)
{
	//��û�п��ܻ������������ܻ�����·������ֻ����һ��
	for (int i = 1; i < center_path.size(); i++)
	{
		if (center_path.empty())continue;
		int tickness = sqrt(PATH_LENGTH / (i + 1.0)*2.5);
		cv::line(frame, center_path[i - 1], center_path[i], cv::Scalar(0, 255, 255), tickness);
	}
	return frame;
}
