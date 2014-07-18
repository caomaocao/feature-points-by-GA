#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<vector>
#include<math.h>
#include<time.h>
#include <algorithm>
#include <direct.h>

#define FLOAT2INT(fNum) ((fNum>0)?static_cast<int>(fNum+0.5):static_cast<int>(fNum-0.5))//浮点数转整数
#define SAMPLE_HEIGHT_BIT_COUNT 10
#define	SAMPLE_WIDTH_BIT_COUNT 10
#define SAMPLE_LENGTH 10 //支持最宽为1024或者最长为1024的图片 
#define FEATURE_COUNTS 15
#define POS_FEATURE_COUNTS 15//正向特征点数量
#define NEG_FEATURE_COUNTS 5//反向特征点数量

/*转为全局变量*/
#define POINT_COUNT 30	//种群中个体个数；随机点个数
#define BIG_POINT_COUNT 20   //达到点数
#define MUTATION_PERCENT 0.2//变异率

using namespace std;

typedef struct Point 
{           
	int x;
	int y;
} Point;

unsigned char bmpinfoarray[54];//信息头
unsigned char plltee[1024];//调色板

unsigned char raw_data[500000];//二值图	原始数据
unsigned char compare_data[500000];//掏空图
unsigned char bound_full_data[500000];//边框图，内部为实心
unsigned char bound_empty_data[500000];//边框图，内部为空心

int width,height,scanline;
int rightbound,leftbound,topbound,downbound;

vector<Point> somepoints;// 随机选廓点 包括中间亮斑的轮廓点，工件最小包围框
vector<Point> boundpoints;//轮廓点 包括中间亮斑的轮廓点，不包括工件最小包围框
vector<Point> allboundpoints;//轮

char pos_bound_full_data_name[255];//局部最大圆图
char pos_compare_data_name[255];//特征点图
char pos_inclean_name[255];//镂空图
char pos_in_suffix[20]="-pos_empty";//镂空图后缀
char pos_empty_suffix[20]="-pos_dot";//特征点图后缀
char pos_full_suffix[20]="-pos_circle";//局部最大圆图后缀
char neg_bound_full_data_name[255];
char neg_compare_data_name[255];
char neg_inclean_name[255];
char neg_in_suffix[20]="-neg_empty";
char neg_empty_suffix[20]="-neg_dot";
char neg_full_suffix[20]="-neg_circle";
char bmp[20]=".bmp";//BMP文件后缀

/*声明*/
void Str_process(void);

int Save_Image(char filename[],int choice);

int Load_File(char filename[]);

int Ostu_Process();

void In_Clean_White();

void In_Clean_Black(char filename[255],int choice);

void Carculate_Pic(void);

void Random_In_Numbers();

void Random_Out_Numbers();

int Read_All_Boundpoints(int choice);

float Build_Gnenration(struct Point *p,int i);

float Get_Current_Gene_Adapt_Value(struct Point p);

bool SortBy( const float &v1, const float &v2);

void Select_Generation(struct Point *p,float *adapt_value);

bool Is_NewGeneration_Optimise_OldGeneration(float *adapt_value,float *after_select_adapt_value,int *max_adapt_value_locate,int *after_select_adapt_value_locate);

void Crossover(struct Point *p,float value_adapt[],bool *is_broaden_mutation,int choice);

void BinaryCross(struct Point *p,int x,int y,int choice);

void Trans_To_Binary_Array(int numberarray[],struct Point *p,int flag);

int Binary_To_Decimal(int *binaryarray);

void Mutation(Point pointsarray[],float *p_adapt_value, bool is_broaden_mutation,int choice) ;//变异操作 已经更新适配值

void Mutation_PointArray(Point *p,int choice);   //对该节点进行变异

void Trans_2_Foundation();

void Trans_Back_Foundation(struct Point *p);

void Trans_Foundation(struct Point *p);

float Get_Nearest_Distance(struct Point p);

void Trans_Boundpoints_2_Foundation();

int Write_Points_2_Txt(char txtname[]);

void Draw_Dot_Circle(float radius,int cicle_center_x,int cicle_center_y,int choice);

void Float_qsort(float s[],int l,int r);


/*更新特征点图文件名*/
void Str_process(char* originfile)
{
	strcpy(pos_inclean_name,originfile);
	strcat(pos_inclean_name,pos_in_suffix);
	strcat(pos_inclean_name,bmp);
	strcpy(pos_compare_data_name,originfile);
	strcat(pos_compare_data_name,pos_empty_suffix);
	strcat(pos_compare_data_name,bmp);
	strcpy(pos_bound_full_data_name,originfile);
	strcat(pos_bound_full_data_name,pos_full_suffix);
	strcat(pos_bound_full_data_name,bmp);
	
	strcpy(neg_inclean_name,originfile);
	strcat(neg_inclean_name,neg_in_suffix);
	strcat(neg_inclean_name,bmp);
	strcpy(neg_compare_data_name,originfile);
	strcat(neg_compare_data_name,neg_empty_suffix);
	strcat(neg_compare_data_name,bmp);
	strcpy(neg_bound_full_data_name,originfile);
	strcat(neg_bound_full_data_name,neg_full_suffix);
	strcat(neg_bound_full_data_name,bmp);
}

/*读图像*/
int Load_File(char filename[])
{
	FILE *source;
   	int  x,y;
	int fileSize,dwSize,current;
		
	memset(raw_data,0,500000*sizeof(unsigned char));
	memset(compare_data,0,500000*sizeof(unsigned char));
	memset(bound_full_data,0,500000*sizeof(unsigned char));
	memset(bound_empty_data,0,500000*sizeof(unsigned char));

	if((source=fopen((char *)filename,"rb+"))==NULL)
    	{
        	printf("can't open the source file!\n");
        	return 1;
    	}
	
	/*读取bmp文件头和信息头存到数组里*/
	fseek(source,0,SEEK_SET);
	fread(bmpinfoarray,sizeof(unsigned char),54,source);
	fseek(source,54,SEEK_SET);
	fread(plltee,sizeof(unsigned char),1024,source);
	
	fseek(source,2,0);
	fread(&fileSize,sizeof(long),1,source);//读取bmp数据区大小
	
	fseek(source,14,0);
	fread(&dwSize,sizeof(long),1,source);

    width=bmpinfoarray[18]+bmpinfoarray[19]*256;
    height=bmpinfoarray[22]+bmpinfoarray[23]*256;

    if(width%4)//bmp宽度不为4要补0
		scanline=(width*8+31)/32*4;
    else
		scanline=width;

 	for(x=0;x<height;x++)//读取bmp进数组		
	{	
		for(y=0;y<scanline;y++)
		{
			current=14+dwSize+1024+x*scanline+y;
			fseek(source,current,0); 
			raw_data[(height-1-x)*scanline+y]=fgetc(source);
		}	
	}
	//memcpy(compare_data,raw_data,500000);
	
	fclose(source);
	return 1;
}	

/*大津法二值化*/
int Ostu_Process()
{
int x,y,i,j;
   int thresh_value;
   double piexlsum,greyscale;
   float piexlcount[255]={0};
   float piexlpercent[255]={0};
   float w0,w1,u0tmp,u1tmp,u0,u1,u,deltaTmp,deltaMax;
   
   thresh_value=0;
   greyscale=255;
   deltaMax=0;
   
	piexlsum=width*height;  
   //memset(piexlcount,0,sizeof(piexlcount));
   //memset(piexlpercent,0,sizeof(piexlpercent));

   for (y=0;y<height;y++)  
      for (x=0;x<width;x++)
	  { 
	    piexlcount[(int)raw_data[y*scanline+x]]++;//每级灰度的像素个数
      }
      
   for(i=0;i<greyscale;i++)
   {
      piexlpercent[i]=(1.0*piexlcount[i])/(1.0*piexlsum);//每级灰度的像素占总像素数比率
   }
   
   for(i=0;i<greyscale;i++)//阈值扫描
   {
     w0=w1=u0tmp=u1tmp=u0=u1=u=deltaTmp=0;
     for(j=0;j<greyscale;j++)
     {
		if(j<i)
		{
			w0+=piexlpercent[j];//前景像素占总像素比率
			u0tmp+=j*piexlpercent[j];//前景像素灰度平均值
		}
		else
		{
			w1+=piexlpercent[j];//背景像素占总像素比率
			u1tmp+=j*piexlpercent[j];//背景像素灰度平均值
		}
     }
     u0=u0tmp/w0;//前景像素灰度平均值
     u1=u1tmp/w1;//背景像素灰度平均值
     u=u0tmp+u1tmp;//所有像素灰度平均值
     deltaTmp=w0*pow((u0-u),2)+w1*pow((u1-u),2);//前景背景差距=w0(u0-u)^2+w1(u1-u)^2
     
	 if(deltaTmp>deltaMax)//最大的deltaMax时，前景背景差异最大
     {
		deltaMax=deltaTmp;
		thresh_value=i;
     }
   }  

   if(thresh_value!=0)//计算得到的阈值不为0
   {
	 for (y=0;y<height;y++)  	
		for (x=0;x<width;x++)
		{ 		
			if(raw_data[y*scanline+x]<thresh_value)
				raw_data[y*scanline+x]=255;
			else
				raw_data[y*scanline+x]=0;
        }
   }

	memcpy(compare_data,raw_data,500000);//复制
	char filename[255]="binary_image.bmp";
    Save_Image(filename,0);
	return 1;
}

/*找到框住工件的最小框*/
void Carculate_Pic(void)
{
	int i,j;
	int count;
	
	for(i=0;i<scanline;i++)
	{	
		count=0;
		for(j=0;j<height;j++)
		{
			if(raw_data[j*scanline+i]==255)
				count++;
		}
		if(count>1)
		{
			leftbound=i;
			break;
		}
	}
	for(i=scanline-1;i>0;i--)
	{
		count=0;
		for(j=0;j<height;j++)
		{
			if(raw_data[j*scanline+i]==255)
				count++;
		}
		if(count>1)
		{
			rightbound=i;//LCD下边
			break;
		}
	}
	
	for(j=0;j<height;j++)
	{
		count=0;
		for(i=0;i<scanline;i++)
		{
			
			if(raw_data[j*scanline+i]==255)
				count++;
		}
		if(count>1)
		{
			topbound=j;
			break;
		}
	}
	for(j=height-1;j>0;j--)
	{
		count=0;
		for(i=0;i<scanline;i++)
		{
			if(raw_data[j*scanline+i]==255)
				count++;
		}
		if(count>1)
		{
			downbound=j;
			break;
		}
	}
}

/*画边框*/
void Draw_Bound_Pic()
{
	int x,y;
	char filename[255]="bound_full_data.bmp";
	
	memcpy(bound_full_data,raw_data,500000);//复制

	for(x=topbound;x<downbound;x++)
	{
		bound_full_data[x*scanline+leftbound]=255;
	}
	for(x=topbound;x<downbound;x++)
	{
		bound_full_data[x*scanline+rightbound]=255;
	}
	for(y=leftbound;y<rightbound;y++)
	{		
		bound_full_data[topbound*scanline+y]=255;
	}
	for(y=leftbound;y<rightbound;y++)
	{		
		bound_full_data[downbound*scanline+y]=255;
	}

	Save_Image(filename,1);
}

/*找到p点到边框最短距离*/
float Get_Nearest_Distance(struct Point p)
{
	int i,x,y;
	float value;
	int boundlength=boundpoints.size();
	vector<float> temp_neararray;//边缘点到现在点最短距离
	Point temp;

	x=p.x;
	y=p.y;
	for(i=0;i<boundlength;i++)
	{
		temp=boundpoints[i];
		float tempvalue=(temp.x-p.x)*(temp.x-p.x)+(temp.y-p.y)*(temp.y-p.y) ;				
		temp_neararray.push_back(sqrt(tempvalue));
	}	
	std::sort(temp_neararray.begin(),temp_neararray.end(),SortBy);
	value=temp_neararray[0];
	temp_neararray.clear();

	return value;
}


/*十进制转二进制*/
void Trans_To_Binary_Array(int numberarray[],struct Point *p,int flag)//flag等于0的话取x，1为y
{
	unsigned int sum;
	int i;

	if(flag==0)
		sum=p->x;	
	else
		sum=p->y;	

	for(i=0;i<SAMPLE_LENGTH;i++)//初始化
	{
		numberarray[i]=0;
	}

	i=SAMPLE_LENGTH-1;
	while(sum!=0)
	{
		numberarray[i--]=sum%2;
		sum/=2;
	}
}

/*二进制转十进制*/
int Binary_To_Decimal(int *binaryarray)
{
	int exp=1,sum=0,i;

	for(i=SAMPLE_LENGTH-1;i>=0;i--)
	{
		sum+=binaryarray[i]*exp;
		exp*=2;
	}

	return sum;
}

/*把边界点和形状内点坐标系变换到以最小边框左上角，防止交叉变异得到的坐标出界*/
void Trans_2_Foundation()
{
	int somepoints_length,boundpoints_length;
	int i;

	somepoints_length=somepoints.size();//图形形状内部点
	boundpoints_length=boundpoints.size();//形状的轮廓点
	
	/*坐标系转换，减去最小边框左上角点*/
	for(i=0;i<somepoints_length;i++)
	{
		somepoints[i].x-=topbound;
		somepoints[i].y-=leftbound;
	}
	for(i=0;i<boundpoints_length;i++)
	{
		boundpoints[i].x-=topbound;
		boundpoints[i].y-=leftbound;
	}
}

void Trans_Boundpoints_2_Foundation()
{
	int boundpoints_length;
	int i;

	boundpoints_length=boundpoints.size();
	for(i=0;i<boundpoints_length;i++)
	{
		boundpoints[i].x+=topbound;
		boundpoints[i].y+=leftbound;
	}
}

/*把选出的点坐标系转换为B最小框左上角的坐标系，方便绘制*/
void Trans_Foundation(struct Point *p)
{
	int i;

	for(i=0;i<POINT_COUNT;i++)
	{
		p[i].x-=topbound;
		p[i].y-=leftbound;
	}
}

/*把坐标系转换为Bmp左上角的坐标系，方便绘制*/
void Trans_Back_Foundation(struct Point *p)
{
	int i;

	for(i=0;i<POINT_COUNT;i++)
	{
		p[i].x+=topbound;
		p[i].y+=leftbound;
	}
}

/*保存图片，数组写进bmp文件*/
int Save_Image(char savename[],int choice)
{
	FILE *dst;
	unsigned char temp;
	int x,y;
	float current;

	if((dst=fopen(savename,"wb+"))==NULL)
	{
		printf("file cannot open\n");
		return 0;
	}

	fseek(dst,0,SEEK_SET);
	for(x=0;x<54;x++)//写bmp信息头
	{
		temp=bmpinfoarray[x];
		fwrite(&temp,sizeof(unsigned char),1,dst);	
	}
	fseek(dst,54,SEEK_SET);//写bmp调色板
	for(x=0;x<1024;x++)
	{
		fwrite(&plltee[x],sizeof(unsigned char),1,dst);	
	}

	fseek(dst,1078,SEEK_SET);//存进数据区
	if(choice==0)
	{
		for(x=0;x<height;x++)
		{	
			for(y=0;y<scanline;y++)
			{			
				temp=compare_data[x*scanline+y];
				current=14+40+1024+(height-x-1)*scanline+y;
				fseek(dst,current,SEEK_SET);
				fwrite(&temp,sizeof(unsigned char),1,dst);			
			}
		}
	}
	else if(choice==1)
	{
		for(x=0;x<height;x++)
		{	
			for(y=0;y<scanline;y++)
			{			
				temp=bound_full_data[x*scanline+y];
				current=14+40+1024+(height-x-1)*scanline+y;
				fseek(dst,current,SEEK_SET);
				fwrite(&temp,sizeof(unsigned char),1,dst);			
			}
		}
	}
	else if(choice==2)
	{
		
		for(x=0;x<height;x++)
		{	
			for(y=0;y<scanline;y++)
			{			
				temp=bound_empty_data[x*scanline+y];
				current=14+40+1024+(height-x-1)*scanline+y;
				fseek(dst,current,SEEK_SET);
				fwrite(&temp,sizeof(unsigned char),1,dst);			
			}
		}
	}

	fclose(dst);	
	return 1;
}

/*掏空白色区域内部 */
 void In_Clean_White()
 {
   int x, y;
   int v[9];
 
   for (y=1;y<height-1;y++)  
      for (x=1;x<scanline-1;x++)
	  {
	        if(raw_data[y*scanline+x]==255)
			{
			    v[0] = raw_data[(y-1)*scanline+x-1];
			    v[1] = raw_data[(y-1)*scanline+x];
			    v[2] = raw_data[(y-1)*scanline+x+1];
			    v[3] = raw_data[y*scanline+x-1];
			    v[5] = raw_data[y*scanline+x+1];
			    v[6] = raw_data[(y+1)*scanline+x-1];
			    v[7] = raw_data[(y+1)*scanline+x];
			    v[8] = raw_data[(y+1)*scanline+x+1];
		        if(v[0]+v[1]+v[2]+v[3]+v[5]+v[6]+v[7]+v[8]==(255*8))
				{
					compare_data[y*scanline+x]=0;
	            }
			}
	  }  
	
	for(x=0;x<scanline;x++)
	{  
		compare_data[x] = 255;
		compare_data[(height - 1)*scanline+x] =255;    	
	}
   
	for(y=0;y<height;y++)
	{  
	  compare_data[y] = 255; 
	  compare_data[y*scanline+scanline - 1] = 255;
	}   
 }

 /*掏空黑色内部*/
 void In_Clean_Black(char filename[255],int choice)
{
   int x, y;
   int v[9];
   
   /*掏空*/
   for (y=1;y<height-1;y++)  
      for (x=1;x<scanline-1;x++)
	  {
	        if(raw_data[y*scanline+x]==255)
			{
			    v[0] = raw_data[(y-1)*scanline+x-1];
			    v[1] = raw_data[(y-1)*scanline+x];
			    v[2] = raw_data[(y-1)*scanline+x+1];
			    v[3] = raw_data[y*scanline+x-1];
			    v[5] = raw_data[y*scanline+x+1];
			    v[6] = raw_data[(y+1)*scanline+x-1];
			    v[7] = raw_data[(y+1)*scanline+x];
			    v[8] = raw_data[(y+1)*scanline+x+1];
		        if(v[0]+v[1]+v[2]+v[3]+v[5]+v[6]+v[7]+v[8]==255*8)
				{
					compare_data[y*scanline+x]=0;
	            }
			}
	  }  
	
	for(x=0;x<scanline;x++)
	{  
		compare_data[x] = 0;
		compare_data[(height - 1)*scanline+x] =0;    	
	}
   
	for(y=0;y<height;y++)
	{  
	  compare_data[y] = 0; 
	  compare_data[y*scanline+scanline - 1] = 0;
	}   

	if(choice==1)//如果要找反向点，则需要画边框
	{
		for(x=topbound;x<downbound;x++)
		{
			compare_data[x*scanline+leftbound]=255;
		}
		for(x=topbound;x<downbound;x++)
		{
			compare_data[x*scanline+rightbound]=255;
		}
		for(y=leftbound;y<rightbound;y++)
		{		
			compare_data[topbound*scanline+y]=255;
		}
		for(y=leftbound;y<rightbound;y++)
		{		
			compare_data[downbound*scanline+y]=255;
		}
	}

	Save_Image(filename,0);
	
 }



/*写特征点进txt*/
int Write_Points_2_Txt(char txtname[])
{
	int count=0,length,x;
	int randx,randy;
	FILE *source;
	Point currentPt;
	vector<Point> pointsarray;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;
	
	if((source=fopen(txtname,"wb"))==NULL)
    {
        printf("can't open the source file!\n");
        return 0;
    }
	srand((unsigned)time(NULL)); //产生随机种子	
	while(count<30)
	{
		randx=rand()%tempheight+topbound;//随机点的范围在最小框内
		randy=rand()%tempwidth+leftbound;
		if(compare_data[randx*scanline+randy]==128)//特征点是灰色的
		{
			currentPt.x=randx;
			currentPt.y=randy;
			pointsarray.push_back(currentPt);
			count++;
		}
	}
	while(count<30)
	{
		randx=rand()%tempheight+topbound;//随机点的范围在最小框内
		randy=rand()%tempwidth+leftbound;
		if(compare_data[randx*scanline+randy]==64)//特征点是灰色的
		{
			currentPt.x=randx;
			currentPt.y=randy;
			pointsarray.push_back(currentPt);
			count++;
		}
	}

	length=pointsarray.size();
	for(x=0;x<length/2;x++)//处于一半的点有危险
	{
		fprintf(source,"%d,%d,%d,",pointsarray[x].x,pointsarray[x].y,-1);
	}
	for(x=length/2;x<length;x++)
	{
		fprintf(source,"%d,%d,%d,",pointsarray[x].x,pointsarray[x].y,1);
	}
	pointsarray.clear();
	fclose(source);
	return 1;
}

inline void _draw_circle_8(int xc,int yc,int x,int y,int is_empty_choice,int choice)
{
	if(is_empty_choice==0)
	{
		compare_data[(xc+x)*scanline+yc+y]=255;
		compare_data[(xc-x)*scanline+yc+y]=255;
		compare_data[(xc+x)*scanline+yc-y]=255;
		compare_data[(xc-x)*scanline+yc-y]=255;

		compare_data[(xc+y)*scanline+yc+x]=255;
		compare_data[(xc-y)*scanline+yc+x]=255;
		compare_data[(xc+y)*scanline+yc-x]=255;
		compare_data[(xc-y)*scanline+yc-x]=255;
	}
	else if(is_empty_choice==1)
	{
		if(choice==0)
		{
			bound_full_data[(xc+x)*scanline+yc+y]=0;
			bound_full_data[(xc-x)*scanline+yc+y]=0;
			bound_full_data[(xc+x)*scanline+yc-y]=0;
			bound_full_data[(xc-x)*scanline+yc-y]=0;

			bound_full_data[(xc+y)*scanline+yc+x]=0;
			bound_full_data[(xc-y)*scanline+yc+x]=0;
			bound_full_data[(xc+y)*scanline+yc-x]=0;
			bound_full_data[(xc-y)*scanline+yc-x]=0;
		}
		else if( choice == 1)
		{
			bound_full_data[(xc+x)*scanline+yc+y]=255;
			bound_full_data[(xc-x)*scanline+yc+y]=255;
			bound_full_data[(xc+x)*scanline+yc-y]=255;
			bound_full_data[(xc-x)*scanline+yc-y]=255;

			bound_full_data[(xc+y)*scanline+yc+x]=255;
			bound_full_data[(xc-y)*scanline+yc+x]=255;
			bound_full_data[(xc+y)*scanline+yc-x]=255;
			bound_full_data[(xc-y)*scanline+yc-x]=255;
		}
	}
}

/*画最优点为圆心，到边缘最短距离为半径的圆*/
void Draw_Dot_Circle(float radius,int cicle_center_x,int cicle_center_y,int choice)//Bresenham法画圆
{
	int x=0,y=radius,yi,d;
	d=3-2*radius;

	while(x<=y)//画空心圆
	{
		_draw_circle_8(cicle_center_x,cicle_center_y,x,y,0,choice);
		if(d<0)
			d=d+4*x+6;
		else
		{
			d=d+4*(x-y)+10;
			y--;
		}
		x++;
	}

	x=0;
	y=radius;
	d=3-2*radius;

	while(x<=y)//画实心圆
	{
		if(choice==0)
		{
			for(yi=x;yi<=y;yi++)
				_draw_circle_8(cicle_center_x,cicle_center_y,x,yi,1,choice);
		}
		else if(choice==1)
		{
			for(yi=x;yi<=y;yi++)
				_draw_circle_8(cicle_center_x,cicle_center_y,x,yi,1,choice);
		}

		if(d<0)
			d=d+4*x+6;
		else
		{
			d=d+4*(x-y)+10;
			y--;
		}
		x++;
	}
	compare_data[cicle_center_x*scanline+cicle_center_y]=128;//圆心灰度值为128
}

/*快速排序*/
void Float_qsort(float s[],int l,int r)
{
	int i,j;
	float x;
	if(l<r)
	{
		i=l;
		j=r;
		x=s[i];
		while(i<j)
		{
			while(i<j&&s[j]>x)
				j--;
			if(i<j)
				s[i++]=s[j];
			while(i<j&&s[i]<x)
				i++;
			if(i<j)
				s[j--]=s[i];
		}
		s[i]=x;
		Float_qsort(s,l,i-1);
		Float_qsort(s,i+1,r);
	}
}