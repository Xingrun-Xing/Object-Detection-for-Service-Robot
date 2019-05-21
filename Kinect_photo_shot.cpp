#include <stdio.h>
#include <iostream>
#include <Kinect.h>
#include <windows.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main()
{
	IKinectSensor* m_pKinectSensor;
	IColorFrameSource* pColorFrameSource;
	IColorFrameReader* m_pColorFrameReader;
	IFrameDescription* pFrameDescription = NULL;
	ColorImageFormat imageFormat = ColorImageFormat_None;
	int nWidth, nHeight;
	uchar *pBuffer = NULL;

	UINT nBufferSize = 0;



	HRESULT hr = GetDefaultKinectSensor(&m_pKinectSensor);//��ȡĬ�ϴ�����
	assert(hr >= 0);
	hr = m_pKinectSensor->Open();//�򿪴�����
	assert(hr >= 0);
	hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);//����ɫ��ϢԴ
	assert(hr >= 0);
	pColorFrameSource->OpenReader(&m_pColorFrameReader);//����ɫ֡��ȡ��
	assert(hr >= 0);

	int i = 90;
	while (waitKey(1)!=27) {
		//��ȡ����Ĳ�ɫ֡
		IColorFrame* pColorFrame = NULL;
		while ((hr < 0) || (NULL == pColorFrame))//ѭ��ֱ����ȡ�������һ֡
		{
			hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);
		}
		//��ȡ��ɫͼƬ��Ϣ�������ߣ���ʽ
		assert(hr >= 0);
		pColorFrame->get_FrameDescription(&pFrameDescription);//��ȡͼƬ������Ϣ
		pFrameDescription->get_Width(&nWidth);
		pFrameDescription->get_Height(&nHeight);
		//cout << "width=" << nWidth << endl;//�ҵ�kinect����ͷ��Ϊ1920
		//cout << "Height=" << nHeight << endl;//�ҵ�kinect����ͷ��Ϊ1080
		pColorFrame->get_RawColorImageFormat(&imageFormat);//������Ϊ ColorImageFormat_Yuy2    = 5��ΪYuy2��ʽ
														   /*YUY2��ʽ����4:2:2��ʽ��� YUV 4:2:2
														   ÿ��ɫ���ŵ��ĳ������������ŵ���һ�룬����ˮƽ�����ɫ�ȳ�����ֻ��4:4 : 4��һ�롣�Է�ѹ����8����������ͼ����˵��
														   ÿ��������ˮƽ�������ڵ�������ɵĺ�������Ҫռ��4�ֽ��ڴ档
														   ������ĸ�����Ϊ��[Y0 U0 V0][Y1 U1 V1][Y2 U2 V2][Y3 U3 V3]
														   ��ŵ�����Ϊ��Y0 U0 Y1 V1 Y2 U2 Y3 V3
														   ӳ������ص�Ϊ��[Y0 U0 V1][Y1 U0 V1][Y2 U2 V3][Y3 U2 V3]*/
		//cout << "imageformat is " << imageFormat << endl;

		Mat colorImg(nHeight, nWidth, CV_8UC4);//�½�һ��mat�������ڱ�������ͼ��,ע������ĸ���ǰ�����ں�
		pBuffer = colorImg.data;
		nBufferSize = colorImg.rows*colorImg.step;

		/*����CopyConvertedFrameDataToArray���˺����������Ǵ�pColorFrame�����п���nBufferSize���ֽڵ�pBuffer��ָ��Mat�����У���
		ColorImageFormat_Bgra��ʽ����*/
		hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
		pColorFrame->Release();

		resize(colorImg, colorImg, Size(),0.5,0.5);
		imshow("display", colorImg);
		
		if (waitKey(1) == 13)
		{
			string name = "C:\\Users\\XingXingrun\\Desktop\\a\\"+to_string(i) + ".jpg";
			imwrite(name,colorImg);
			cout << name << " ok" << endl;
			i++;
		}		
		cout << "ok" << endl;
	}
	return 0;
}