#include "imagePro.h"

ImagePro::ImagePro(){}//���캯��

ImagePro::~ImagePro(){}//��������

void ImagePro::InitQueue(LinkQueue &Q)  //���г�ʼ��
{
	Q.front = (QNode *)malloc(sizeof(QNode));
	Q.rear = Q.front;
	Q.front->next = NULL;
}

bool ImagePro::IsEmpty(LinkQueue Q)//�����п�
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

void ImagePro::EnQueue(LinkQueue &Q,int x,int y)//���
{
	QNode *s;
	s = (QNode *)malloc(sizeof(QNode));
	s->x = x;
	s->y = y;
	s->next = NULL;
	Q.rear->next = s;
	Q.rear = s;
}

bool ImagePro::DeQueue(LinkQueue &Q,int &x,int &y)//����
{
	if(Q.front == Q.rear)//�ն�
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
		Q.rear = Q.front;//��ԭ������ֻ��һ����㣬ɾ������
	}
	free(p);
	return true;

}

//����������,ȥ�����С��area����ͨ��
void ImagePro::areaGrow(IplImage* image,int area)
{
	IplImage* flag  = cvCreateImage(cvSize(image->width ,image->height ),IPL_DEPTH_8U,1);
	int T_area = area;//���ֵ���С�����ֵ
	cvZero(flag);

	//ѭ��������������Ѱ�����ӵ� 
	for (int x=0;x<image->width;x++)
		for(int y=0;y<image->height;y++)
		{
			//��ȡ����ֵ
			int pixelValue = cvGetReal2D(image,y,x);
			if((!cvGetReal2D(flag,y,x))&&pixelValue>100)  //�ж��Ƿ�Ϊ���ӵ�
			{
				cvSetReal2D(flag,y,x,1);
				int seed_area_num = 0;		//ͳ�Ƶ�����ͨ�����ڵ����ظ���
				LinkQueue Q,L;
				InitQueue(Q);
				InitQueue(L);
				EnQueue(Q,x,y);
				while(!IsEmpty(Q))//�����п�
				{
					int area_x,area_y;
					if(DeQueue(Q,area_x,area_y))
					{
						EnQueue(L,area_x,area_y);
						seed_area_num++;
						if((area_x-1>=0)&&(area_y>=0)&&(area_x-1<image->width)&&(area_y<image->height)&&!cvGetReal2D(flag,area_y,area_x-1))//��
						{
							//��ȡ����ֵ 
							int value_west = cvGetReal2D(image,area_y,area_x-1);
							if(value_west>100)
							{
								EnQueue(Q,area_x-1,area_y);
								cvSetReal2D(flag,area_y,area_x-1,1);
							}
						}

						if((area_x-1>=0)&&(area_y-1>=0)&&(area_x-1<image->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x-1))//����
						{
							//��ȡ����ֵ 
							int value_west_north = cvGetReal2D(image,area_y-1,area_x-1);
							if(value_west_north>100)
							{
								EnQueue(Q,area_x-1,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x-1,1);
							}
						}

						if((area_x>=0)&&(area_y-1>=0)&&(area_x<image ->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x))//��
						{
							//��ȡ����ֵ 
							int value_north = cvGetReal2D(image,area_y-1,area_x);
							if(value_north>100)
							{
								EnQueue(Q,area_x,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x,1);
							}
						}

						if((area_x+1>=0)&&(area_y-1>=0)&&(area_x+1<image->width)&&(area_y-1<image->height)&&!cvGetReal2D(flag,area_y-1,area_x+1))//����
						{
							//��ȡ����ֵ 
							int value_east_north = cvGetReal2D(image,area_y-1,area_x+1);
							if(value_east_north>100)
							{
								EnQueue(Q,area_x+1,area_y-1);
								cvSetReal2D(flag,area_y-1,area_x+1,1);
							}
						}

						if((area_x+1>=0)&&(area_y>=0)&&(area_x+1<image->width)&&(area_y<image->height)&&!cvGetReal2D(flag,area_y,area_x+1))//��
						{
							//��ȡ����ֵ 
							int value_east = cvGetReal2D(image,area_y,area_x+1);
							if(value_east>100)
							{
								EnQueue(Q,area_x+1,area_y);
								cvSetReal2D(flag,area_y,area_x+1,1);
							}
						}

						if((area_x+1>=0)&&(area_y+1>=0)&&(area_x+1<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x+1))//����
						{
							//��ȡ����ֵ 
							int value_north_south = cvGetReal2D(image,area_y+1,area_x+1);
							if(value_north_south>100)
							{
								EnQueue(Q,area_x+1,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x+1,1);
							}
						}

						if((area_x>=0)&&(area_y+1>=0)&&(area_x<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x))//��
						{
							//��ȡ����ֵ 
							int value_south =cvGetReal2D(image,area_y+1,area_x);
							if(value_south>100)
							{
								EnQueue(Q,area_x,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x,1);
							}
						}

						if((area_x-1>=0)&&(area_y+1>=0)&&(area_x-1<image->width)&&(area_y+1<image->height)&&!cvGetReal2D(flag,area_y+1,area_x-1))//����
						{
							//��ȡ����ֵ 
							int value_west_south = cvGetReal2D(image,area_y+1,area_x-1);
							if(value_west_south>100)
							{
								EnQueue(Q,area_x-1,area_y+1);
								cvSetReal2D(flag,area_y+1,area_x-1,1);
							}
						}


					}//if(DeQueue(Q,area_x,area_y))

				}//while(!IsEmpty(Q))�����п�
				free(Q.front);  //�ͷ�ͷ���
				if(seed_area_num<T_area)
				{
					while(!IsEmpty(L))//�����п�
					{
						int L_x,L_y;
						if(DeQueue(L,L_x,L_y))
						{
							uchar* pixelValue = (uchar*)(image->imageData + L_y*image->widthStep);
							*(pixelValue+L_x) = 0;
						}
					}
					free(L.front);//�ͷ�ͷ���
				}
				else
				{
					while(!IsEmpty(L))//�����п�
					{
						int L_x,L_y;
						DeQueue(L,L_x,L_y);
					}
					free(L.front);//�ͷ�ͷ���
				}
			}//if((!flag[x][y])&&pixelValue<50)  //�ж��Ƿ�Ϊ���ӵ�

		}//for
		cvReleaseImage(&flag);
}