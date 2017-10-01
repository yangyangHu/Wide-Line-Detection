#include "WideLineDetector.h"

WLD::WLD(IplImage* in) //���캯��
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

	r = 10;     ////Բ����ģ�İ뾶

	areaMin = 15;

	circularMask_w = cvCreateImage(cvSize(2*r+1,2*r+1),IPL_DEPTH_32F,1);
	circularMask_m = cvCreateImage(cvGetSize(in),IPL_DEPTH_32F,1);
	L = cvCreateImage(cvGetSize(in),IPL_DEPTH_32F,1);
	OutputImage = cvCreateImage(cvGetSize(in),IPL_DEPTH_8U,1);
}

WLD::~WLD()//��������
{
	cvReleaseImage(&InputImage);
	cvReleaseImage(&circularMask_w);
	cvReleaseImage(&circularMask_m);
	cvReleaseImage(&L);
	cvReleaseImage(&OutputImage);
}

void WLD::Calculate_t() //����ҶȶԱ���ֵ(����������t�����Ӱ��)
{
	//��Ҫ��һ�����ƣ�ͨ��������ɫ����Ӱ�죩
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

void WLD::Calculate_w()//����Բ����ģȨֵ
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
	//��һ��
	cvNormalize(circularMask_w,circularMask_w,1.0,0.0,CV_L1,NULL);
}

void WLD::Calculate_Iw()//����Բ����ģȨֵ(��ͬ��r�£��ɼ�������İ���,��߼���ٶ�)
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
	//��һ��
	cvNormalize(circularMask_w,circularMask_w,1.0,0.0,CV_L1,NULL);
}

void WLD::Calculate_c(int x0,int y0)//����Բ����ģ ���ƶ�(ǰ���������t,circularMask_w,circularMask_pixel)
{
	double m = 0.0;//sum of c in each patch
	double intensityCenter = cvGetReal2D(InputImage,y0,x0); //��ȡ�ο���ĻҶ�ֵ
	for (int y=y0-r;y-y0+r<2*r+1;y++)
	{
		for (int x=x0-r;x-x0+r<2*r+1;x++)
		{
			double dis;

			//ͼ��߽紦��
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

			dis = (pixel-intensityCenter)/t;//��ǰ���t
			double vairant = pow(dis,5);
			double s = ((double)2)/(exp(vairant)+exp(-vairant));

			////======================�򵥿��ټ���s===========================
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
			if (pixel>intensityCenter)//�������淴��Ӱ��
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

void WLD::Calculate_WMSBmass()//����WMSB mass
{
	Calculate_t(); //����ҶȶԱ���ֵt
	Calculate_Iw(); //����Բ����ģȨֵcircularMask_w
	//��WMSB mass
	//for (int y0 = r;y0<InputImage->height-r;y0++)//�����Ǳ߽�
	parallel_for (int(0), InputImage->height, [&](int y0)
	{
		/*for (int x0 = r;x0<InputImage->width-r;x0++)*/
		parallel_for (int(0), InputImage->width, [&](int x0)
		{

			//����Բ����ģ ���ƶ�
			Calculate_c(x0,y0);

		});
	});
	cvShowImage("m",circularMask_m);
}

void WLD::Calculate_L()//��������Ӧ
{
	double g;
	double m_max;
	Calculate_WMSBmass(); //����WMSB mass

	//====================���ٷ�ʽ===========================
	//cvMinMaxLoc(circularMask_m,NULL,&m_max,NULL,NULL,NULL);
	//g = 4*m_max/5;//����...

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

void WLD::Postprocessing()//����
{
	//�����ֵ����ֵ������
	cvScale(L,OutputImage,255);
	cvThreshold(OutputImage,OutputImage,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
	cvShowImage("OutputImage",OutputImage);

	//����������ȥ������
	ImagePro imagepro;
	imagepro.areaGrow(OutputImage,areaMin);
	cvShowImage("OutputImage1",OutputImage);
}

IplImage* WLD::GetOutputImage()
{
	return OutputImage;
}