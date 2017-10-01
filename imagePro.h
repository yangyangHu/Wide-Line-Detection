#include "Global.h"

class ImagePro
{
public:
	typedef struct QNode   //���н��
	{
		int x;//ͼ��x��������
		int y;//ͼ��y��������
		struct QNode *next;
	}QNode,*QueuePtr;

	typedef struct
	{
		QNode *front;//����ͷ
		QNode *rear;//����β
	}LinkQueue;

public:
	ImagePro();//���캯��

	~ImagePro();//��������

	void InitQueue(LinkQueue &Q);  //���г�ʼ��

	bool IsEmpty(LinkQueue Q);//�����п�

	void EnQueue(LinkQueue &Q,int x,int y);//���

	bool DeQueue(LinkQueue &Q,int &x,int &y);//����

	//����������,ȥ�����С��area����ͨ��
	void areaGrow(IplImage* image,int area);

};