#include "WideLineDetector.h"

int main()
{
	double  time;   
	clock_t Start,Finish;
	Start=clock( );

	IplImage* in;
	IplImage* gray;
	in = cvLoadImage("E:\\上海中医药大学合作项目\\projects\\舌像特征提取\\裂纹\\data\\22.jpg",CV_LOAD_IMAGE_COLOR);
	gray = cvCreateImage(cvGetSize(in),8,1);
	cvCvtColor(in,gray,CV_BGR2GRAY);
	cvShowImage("in",gray);

	WLD wld(gray);

	wld.Calculate_L();
	wld.Postprocessing();

	for (int y=0;y<in->height;y++)
	{
		for (int x=0;x<in->width;x++)
		{
			if (cvGetReal2D(wld.GetOutputImage(),y,x)>100)
			{
				cvSet2D(in,y,x,CV_RGB(0,255,0));
			}
		}
	}

	cvShowImage("result",in);

	Finish=clock( ); 
	time = (double)(Finish-Start)*1000/CLOCKS_PER_SEC;
	std::cout<<"time = "<<time<<"ms"<<std::endl;

	cvWaitKey(0);
	cvReleaseImage(&in);
	cvReleaseImage(&gray);

	return 0;
}