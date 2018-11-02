#include <stdio.h>
#include <Kinect.h>
#include <windows.h>
#include "opencv2\highgui.hpp"
#include "opencv\cv.h"

using namespace cv;

// ת��depthͼ��cv::Mat
Mat ConvertMat(const UINT16* pBuffer, int nWidth, int nHeight)
{
	Mat img(nHeight, nWidth, CV_16UC1);
	UINT16* p_mat = (UINT16*)img.data;//ָ��ͷָ��

	const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);//ָ�����һ��Ԫ�ص�ָ��

	while (pBuffer < pBufferEnd)//16λ���ֵΪ65536
	{
		*p_mat++ = *pBuffer++;// / 65536.0 * 256;
		//USHORT depth = *pBuffer;
		//*p_mat = (depth >= nMinDepth) && (depth <= nMaxDepth) ? depth << 3 : 0;
		//p_mat++;
		//++pBuffer;
	}
	return img;
}
int mainhh()
{
	IKinectSensor*          m_pKinectSensor;
	IDepthFrameReader*      m_pDepthFrameReader;
	IDepthFrame* pDepthFrame = NULL;
	IFrameDescription* pFrameDescription = NULL;
	IDepthFrameSource* pDepthFrameSource = NULL;

	HRESULT hr = GetDefaultKinectSensor(&m_pKinectSensor);//��ȡĬ��kinect������
	assert(hr >= 0);
	printf("��kinect�������ɹ�\n");
	hr = m_pKinectSensor->Open();//�򿪴�����
	hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);//��������Ϣ������
	hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);//�������Ϣ֡��ȡ��
	pDepthFrameSource->get_FrameDescription(&pFrameDescription);
	int depth_width, depth_height;
	pFrameDescription->get_Width(&depth_width);
	pFrameDescription->get_Height(&depth_height);
	printf("width=%d height=%d\n", depth_width, depth_height);
	assert(hr >= 0);
	int i = 0;
	while (1)
	{
		if (m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame) == S_OK)
		{
			USHORT nDepthMinReliableDistance = 0;//��ȡ�����С��Ⱦ�����Ϣ
			USHORT nDepthMaxReliableDistance = 0;
			pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
			hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance);
			printf("nDepthMinReliableDistance=%d nDepthMaxReliableDistance=%d\n", nDepthMinReliableDistance, nDepthMaxReliableDistance);
			UINT nBufferSize_depth = 0;
			ushort *pBuffer_depth = NULL;
			pDepthFrame->AccessUnderlyingBuffer(&nBufferSize_depth, &pBuffer_depth);//��ȡͼ�����ظ�����ָ��ͼ���ָ��																//ת��ΪMAT��ʽ
			Mat depthImg = ConvertMat(pBuffer_depth, depth_width, depth_height);//ת��Ϊushort��mat
			Mat depthImg_show;
			depthImg.convertTo(depthImg_show, CV_8UC1, 255.0 / nDepthMaxReliableDistance);
			imshow("display", depthImg_show);
			char c = waitKey(1);
			if (c == ' ')
			{
				std::string imagepath = "E:\\kinfu\\data\\dataCaptured\\MyFirstKinectImg" + std::to_string(i) + ".png";
				imwrite(imagepath, depthImg);	//����ͼƬ	
				i++;
				printf("****************** img saved **************");
			}
			pDepthFrame->Release();
		}
	}
	return 0;
}