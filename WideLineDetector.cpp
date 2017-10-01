#include "WideLineDetector.h"

WLD::WLD(IplImage* in) //构造函数
{
	InputImage = cvCreateImage(cvGetSize(in),in->depth,1);
	if (in->nChannels>1)
	{
		cvCvtColor(in,InputImage,CV_BGR2GRAY);
	}
	else
	{
		InputImage = cvCloneImage(in);
	}

	r = 10;     ////圆形掩模的半径

	areaMin = 15;

	circularMask_w = cvCreateImage(cvSize(2*r+1,2*r+1),IPL_DEPTH_32F,1);
	circularMask_m = cvCreateImage(cvGetSize(in),IPL_DEPTH_32F,1);
	L = cvCreateImage(cvGetSize(in),IPL_DEPTH_32F,1);
	OutputImage = cvCreateImage(cvGetSize(in),IPL_DEPTH_8U,1);
}

WLD::~WLD()//析构函数
{
	cvReleaseImage(&InputImage);
	cvReleaseImage(&circularMask_w);
	cvReleaseImage(&circularMask_m);
	cvReleaseImage(&L);
	cvReleaseImage(&OutputImage);
}

void WLD::Calculate_t() //计算灰度对比阈值(消除背景对t计算的影响)
{
	//需要进一步完善（通过消除黑色背景影响）
	/*CvScalar std_dev = cvScalarAll(0);
	cvAvgSdv(InputImage,NULL,&std_dev,NULL);
	t = cvRound(std_dev.val[0]);
	printf("t=%d\n",t);*/

	int N = 0;
	double Avg = 0.0,std_dev = 0.0;
	for (int y=0;y<InputImage->height;y++)
	{
		for (int x=0;x<InputImage->width;x++)
		{
			double pixel = cvGetReal2D(InputImage,y,x);
			if (pixel<250)
			{
				N++;
				Avg += pixel;
			}
		}
	}
	Avg /= N;
	for (int y=0;y<InputImage->height;y++)
	{
		for (int x=0;x<InputImage->width;x++)
		{
			double pixel = cvGetReal2D(InputImage,y,x);
			if (pixel<250)
			{
				std_dev += (pixel-Avg)*(pixel-Avg);
			}
		}
	}
	std_dev /= N-1;
	std_dev = sqrt(std_dev);
	t = cvRound(std_dev);
	if (t>20)
	{
		t = 16;
	}
	//printf("t=%d\n",t);
}

void WLD::Calculate_w()//计算圆形掩模权值
{
	for (int y=0;y<circularMask_w->height;y++)
	{
		for (int x=0;x<circularMask_w->width;x++)
		{
			int dis;
			dis = (x-r)*(x-r)+(y-r)*(y-r);
			double power = -((double)dis)/((double)(2*r*r));
			double w = exp(power);
			cvSetReal2D(circularMask_w,y,x,w);
		}
	}
	cvShowImage("w",circularMask_w);
	//归一化
	cvNormalize(circularMask_w,circularMask_w,1.0,0.0,CV_L1,NULL);
}

void WLD::Calculate_Iw()//计算圆形掩模权值(相同的r下，可检测出更宽的暗线,提高检测速度)
{
	for (int y=0;y<circularMask_w->height;y++)
	{
		for (int x=0;x<circularMask_w->width;x++)
		{
			int dis;
			dis = (x-r)*(x-r)+(y-r)*(y-r);
			double power = -((double)dis)/((double)(2*r*r));
			double w = 1 - exp(power);
			cvSetReal2D(circularMask_w,y,x,w);
		}
	}
	cvShowImage("Iw",circularMask_w);
	//归一化
	cvNormalize(circularMask_w,circularMask_w,1.0,0.0,CV_L1,NULL);
}

void WLD::Calculate_c(int x0,int y0)//计算圆形掩模 相似度(前提是先求出t,circularMask_w,circularMask_pixel)
{
	double m = 0.0;//sum of c in each patch
	double intensityCenter = cvGetReal2D(InputImage,y0,x0); //获取参考点的灰度值
	for (int y=y0-r;y-y0+r<2*r+1;y++)
	{
		for (int x=x0-r;x-x0+r<2*r+1;x++)
		{
			double dis;

			//图像边界处理
			double pixel;
			int xx = x,yy = y;
			if (xx<0)
			{
				xx = 0;
			}
			if (xx>=InputImage->width)
			{
				xx = InputImage->width - 1;
			}
			if (yy<0)
			{
				yy = 0;
			}
			if (yy>=InputImage->height)
			{
				yy = InputImage->height - 1;
			}
			pixel = cvGetReal2D(InputImage,yy,xx);

			dis = (pixel-intensityCenter)/t;//提前求出t
			double vairant = pow(dis,5);
			double s = ((double)2)/(exp(vairant)+exp(-vairant));

			////======================简单快速计算s===========================
			//double s;
			//if (abs(pixel-intensityCenter)<=t)
			//{
			//	s = 1;
			//}
			//else
			//{
			//	s = 0;
			//}
			////==============================================================

			double c;
			if (pixel>intensityCenter)//消除舌面反光影响
			{
				c = cvGetReal2D(circularMask_w,y-y0+r,x-x0+r)*s;
			}
			else
			{
				c = cvGetReal2D(circularMask_w,y-y0+r,x-x0+r);
			}
			m += c;
		}
	}
	cvSetReal2D(circularMask_m,y0,x0,m);
}

void WLD::Calculate_WMSBmass()//计算WMSB mass
{
	Calculate_t(); //计算灰度对比阈值t
	Calculate_Iw(); //计算圆形掩模权值circularMask_w
	//求WMSB mass
	//for (int y0 = r;y0<InputImage->height-r;y0++)//不考虑边界
	parallel_for (int(0), InputImage->height, [&](int y0)
	{
		/*for (int x0 = r;x0<InputImage->width-r;x0++)*/
		parallel_for (int(0), InputImage->width, [&](int x0)
		{

			//计算圆形掩模 相似度
			Calculate_c(x0,y0);

		});
	});
	cvShowImage("m",circularMask_m);
}

void WLD::Calculate_L()//计算线响应
{
	double g;
	double m_max;
	Calculate_WMSBmass(); //计算WMSB mass

	//====================加速方式===========================
	//cvMinMaxLoc(circularMask_m,NULL,&m_max,NULL,NULL,NULL);
	//g = 4*m_max/5;//修正...

	g = 0.7;
	//=======================================================

	for (int y=0;y<circularMask_m->height;y++)
	{
		for (int x=0;x<circularMask_m->width;x++)
		{
			double m = cvGetReal2D(circularMask_m,y,x);
			if (m<g&&g-m>0.1)
			{
				cvSetReal2D(L,y,x,g-m);
			}
			else
			{
				cvSetReal2D(L,y,x,0);
			}
		}
	}
	cvShowImage("l",L);
}

void WLD::Postprocessing()//后处理
{
	//大津阈值，二值化处理
	cvScale(L,OutputImage,255);
	cvThreshold(OutputImage,OutputImage,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
	cvShowImage("OutputImage",OutputImage);

	//区域增长法去除噪声
	ImagePro imagepro;
	imagepro.areaGrow(OutputImage,areaMin);
	cvShowImage("OutputImage1",OutputImage);
}

IplImage* WLD::GetOutputImage()
{
	return OutputImage;
}