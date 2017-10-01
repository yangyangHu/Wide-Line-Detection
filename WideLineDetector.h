#include "Global.h"
#include "imagePro.h"

class WLD 
{
	//��Ա����
private:
	IplImage* InputImage;//�������Ҷ�ͼ��
	IplImage* circularMask_w;//Բ����ģ Ȩֵ
	IplImage* circularMask_m;//Բ����ģ WMSB mass
	IplImage* L;//����Ӧ���
	IplImage* OutputImage;//���
	int r;//Բ����ģ�İ뾶
	int t;//�ҶȶԱ���ֵ

	int areaMin;//���Ƶ���С��������ص����������������������ʱ��ȥ����������

	//��Ա����
public:
	WLD(IplImage* in);//���캯��
	~WLD();//��������
	void Calculate_t();//����ҶȶԱ���ֵ
	void Calculate_w();//����Բ����ģȨֵ
	void Calculate_Iw();//����Բ����ģȨֵ(��ͬ��r�£��ɼ�������İ���,��߼���ٶ�)
	void Calculate_c(int x0,int y0);//����Բ����ģ ���ƶ�
	void Calculate_WMSBmass();//����WMSB mass
	void Calculate_L();//��������Ӧ
	void Postprocessing();//����
	IplImage* GetOutputImage();
};