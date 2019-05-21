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



	HRESULT hr = GetDefaultKinectSensor(&m_pKinectSensor);//获取默认传感器
	assert(hr >= 0);
	hr = m_pKinectSensor->Open();//打开传感器
	assert(hr >= 0);
	hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);//打开颜色信息源
	assert(hr >= 0);
	pColorFrameSource->OpenReader(&m_pColorFrameReader);//打开颜色帧读取器
	assert(hr >= 0);

	int i = 90;
	while (waitKey(1)!=27) {
		//获取最近的彩色帧
		IColorFrame* pColorFrame = NULL;
		while ((hr < 0) || (NULL == pColorFrame))//循环直到获取到最近的一帧
		{
			hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);
		}
		//获取彩色图片信息包括宽，高，格式
		assert(hr >= 0);
		pColorFrame->get_FrameDescription(&pFrameDescription);//获取图片描述信息
		pFrameDescription->get_Width(&nWidth);
		pFrameDescription->get_Height(&nHeight);
		//cout << "width=" << nWidth << endl;//我的kinect摄像头宽为1920
		//cout << "Height=" << nHeight << endl;//我的kinect摄像头高为1080
		pColorFrame->get_RawColorImageFormat(&imageFormat);//输出结果为 ColorImageFormat_Yuy2    = 5，为Yuy2格式
														   /*YUY2格式，以4:2:2方式打包 YUV 4:2:2
														   每个色差信道的抽样率是亮度信道的一半，所以水平方向的色度抽样率只是4:4 : 4的一半。对非压缩的8比特量化的图像来说，
														   每个由两个水平方向相邻的像素组成的宏像素需要占用4字节内存。
														   下面的四个像素为：[Y0 U0 V0][Y1 U1 V1][Y2 U2 V2][Y3 U3 V3]
														   存放的码流为：Y0 U0 Y1 V1 Y2 U2 Y3 V3
														   映射出像素点为：[Y0 U0 V1][Y1 U0 V1][Y2 U2 V3][Y3 U2 V3]*/
		//cout << "imageformat is " << imageFormat << endl;

		Mat colorImg(nHeight, nWidth, CV_8UC4);//新建一个mat对象，用于保存读入的图像,注意参数的高在前，宽在后
		pBuffer = colorImg.data;
		nBufferSize = colorImg.rows*colorImg.step;

		/*调用CopyConvertedFrameDataToArray，此函数的作用是从pColorFrame对象中拷贝nBufferSize个字节到pBuffer所指的Mat矩阵中，按
		ColorImageFormat_Bgra格式保存*/
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