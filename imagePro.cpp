#include "imagePro.h"

ImagePro::ImagePro(){}//构造函数

ImagePro::~ImagePro(){}//析构函数

void ImagePro::InitQueue(LinkQueue &Q)  //队列初始化
{
	Q.front = (QNode *)malloc(sizeof(QNode));
	Q.rear = Q.front;
	Q.front->next = NULL;
}

bool ImagePro::IsEmpty(LinkQueue Q)//队列判空
{
	if(Q.front == Q.rear)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ImagePro::EnQueue(LinkQueue &Q,int x,int y)//入队
{
	QNode *s;
	s = (QNode *)malloc(sizeof(QNode));
	s->x = x;
	s->y = y;
	s->next = NULL;
	Q.rear->next = s;
	Q.rear = s;
}

bool ImagePro::DeQueue(LinkQueue &Q,int &x,int &y)//出队
{
	if(Q.front == Q.rear)//空队
	{
		return false;
	}
	QNode *p;
	p = Q.front->next;
	x = p->x;
	y = p->y;
	Q.front->next = p->next;
	if(Q.rear == p)
	{
		Q.rear = Q.front;//若原队列中只有一个结点，删除后变空
	}
	free(p);
	return true;

}

//区域增长法,去除面积小于area的连通域
void ImagePro::areaGrow(IplImage* image,int area)
{
	IplImage* flag  = cvCreateImage(cvSize(image->width ,image->height ),IPL_DEPTH_8U,1);
	int T_area = area;//数字的最小面积阈值
	cvZero(flag);

	//循环遍历所有像素寻找种子点 
	for (int x=0;x<image->width;x++)
		for(int y=0;y<image->height;y++)
		{
			//获取像素值
			int pixelValue = cvGetReal2D(image,y,x);
			if((!cvGetReal2D(flag,y,x))&&pixelValue>100)  //判断是否为种子点
			{
				cvSetReal2D(flag,y,x,1);
				int seed_area_num = 0;		//统计单个连通区域内的像素个数
				LinkQueue Q,L;
				InitQueue(Q);
				InitQueue(L);
				EnQueue(Q,x,y);
				while(!IsEmpty(Q))//队列判空
				{
					int area_x,area_y;
					if(DeQueue(Q,area_x,area_y))
					{
						EnQueue(L,area_x,area_y);
						seed_area_num++;
						if((area_x-1>=0)&&(area_y>=0)&&(area_x-1<image->width)&&(area_y<image->height)&&!cvGetReal2D(flag,area_y,area_x-1))//西
						{
							//获取像素值 
							int value_west = cvGetReal2D(image,area_y,area_x-1);
							if(value_west>100)
							{
								EnQueue(Q,area_x-1,area_y);
								cvSetReal2D(flag,area_y,area_x-1,1);
							}
						}

						if((area_x-1>=0)&&(area_y-1>=0)&&(area_x-1<image->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x-1))//西北
						{
							//获取像素值 
							int value_west_north = cvGetReal2D(image,area_y-1,area_x-1);
							if(value_west_north>100)
							{
								EnQueue(Q,area_x-1,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x-1,1);
							}
						}

						if((area_x>=0)&&(area_y-1>=0)&&(area_x<image ->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x))//北
						{
							//获取像素值 
							int value_north = cvGetReal2D(image,area_y-1,area_x);
							if(value_north>100)
							{
								EnQueue(Q,area_x,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x,1);
							}
						}

						if((area_x+1>=0)&&(area_y-1>=0)&&(area_x+1<image->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x+1))//东北
						{
							//获取像素值 
							int value_east_north = cvGetReal2D(image,area_y-1,area_x+1);
							if(value_east_north>100)
							{
								EnQueue(Q,area_x+1,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x+1,1);
							}
						}

						if((area_x+1>=0)&&(area_y>=0)&&(area_x+1<image->width)&&(area_y<image->height)&&!cvGetReal2D(flag,area_y,area_x+1))//东
						{
							//获取像素值 
							int value_east = cvGetReal2D(image,area_y,area_x+1);
							if(value_east>100)
							{
								EnQueue(Q,area_x+1,area_y);
								cvSetReal2D(flag,area_y,area_x+1,1);
							}
						}

						if((area_x+1>=0)&&(area_y+1>=0)&&(area_x+1<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x+1))//东南
						{
							//获取像素值 
							int value_north_south = cvGetReal2D(image,area_y+1,area_x+1);
							if(value_north_south>100)
							{
								EnQueue(Q,area_x+1,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x+1,1);
							}
						}

						if((area_x>=0)&&(area_y+1>=0)&&(area_x<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x))//南
						{
							//获取像素值 
							int value_south =cvGetReal2D(image,area_y+1,area_x);
							if(value_south>100)
							{
								EnQueue(Q,area_x,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x,1);
							}
						}

						if((area_x-1>=0)&&(area_y+1>=0)&&(area_x-1<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x-1))//西南
						{
							//获取像素值 
							int value_west_south = cvGetReal2D(image,area_y+1,area_x-1);
							if(value_west_south>100)
							{
								EnQueue(Q,area_x-1,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x-1,1);
							}
						}


					}//if(DeQueue(Q,area_x,area_y))

				}//while(!IsEmpty(Q))队列判空
				free(Q.front);  //释放头结点
				if(seed_area_num<T_area)
				{
					while(!IsEmpty(L))//队列判空
					{
						int L_x,L_y;
						if(DeQueue(L,L_x,L_y))
						{
							uchar* pixelValue = (uchar*)(image->imageData + L_y*image->widthStep);
							*(pixelValue+L_x) = 0;
						}
					}
					free(L.front);//释放头结点
				}
				else
				{
					while(!IsEmpty(L))//队列判空
					{
						int L_x,L_y;
						DeQueue(L,L_x,L_y);
					}
					free(L.front);//释放头结点
				}
			}//if((!flag[x][y])&&pixelValue<50)  //判断是否为种子点

		}//for
		cvReleaseImage(&flag);
}