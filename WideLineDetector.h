#include "Global.h"
#include "imagePro.h"

class WLD 
{
	//成员变量
private:
	IplImage* InputImage;//输入的舌灰度图像
	IplImage* circularMask_w;//圆形掩模 权值
	IplImage* circularMask_m;//圆形掩模 WMSB mass
	IplImage* L;//线响应输出
	IplImage* OutputImage;//输出
	int r;//圆形掩模的半径
	int t;//灰度对比阈值

	int areaMin;//裂纹的最小面积（像素点个数），用于区域增长法时，去除干扰噪声

	//成员函数
public:
	WLD(IplImage* in);//构造函数
	~WLD();//析构函数
	void Calculate_t();//计算灰度对比阈值
	void Calculate_w();//计算圆形掩模权值
	void Calculate_Iw();//计算圆形掩模权值(相同的r下，可检测出更宽的暗线,提高检测速度)
	void Calculate_c(int x0,int y0);//计算圆形掩模 相似度
	void Calculate_WMSBmass();//计算WMSB mass
	void Calculate_L();//计算线响应
	void Postprocessing();//后处理
	IplImage* GetOutputImage();
};