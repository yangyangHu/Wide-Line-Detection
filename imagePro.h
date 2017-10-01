#include "Global.h"

class ImagePro
{
public:
	typedef struct QNode   //队列结点
	{
		int x;//图像x方向索引
		int y;//图像y方向索引
		struct QNode *next;
	}QNode,*QueuePtr;

	typedef struct
	{
		QNode *front;//队列头
		QNode *rear;//队列尾
	}LinkQueue;

public:
	ImagePro();//构造函数

	~ImagePro();//析构函数

	void InitQueue(LinkQueue &Q);  //队列初始化

	bool IsEmpty(LinkQueue Q);//队列判空

	void EnQueue(LinkQueue &Q,int x,int y);//入队

	bool DeQueue(LinkQueue &Q,int &x,int &y);//出队

	//区域增长法,去除面积小于area的连通域
	void areaGrow(IplImage* image,int area);

};