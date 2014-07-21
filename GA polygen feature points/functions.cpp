#include "ga.h"

int main(void)
{
	char originfile[255]="290";//源图像文件名
	char filename[255];//源图像文件
	
	Point g[POINT_COUNT]; //序列
	int i,max_iterator=20;//最大迭代次数
	int crrnt_iterate=1;     //当前迭代次数
	float adapt_value[POINT_COUNT],adapt_value_copy[POINT_COUNT]; //当前个体的适配值
	bool is_broaden_mutation;   //是否变异
	int bound_points_count;//边缘点数量，包括中间亮斑的边缘数量                                                                                                               
	int center_dota_count=0;//获得的特征点个数
	int max_frequency_number=0;//最大权值的位置
	int radius,cicle_center_x=0,cicle_center_y=0;//局部最大圆坐标与半径

	/*字符串连接*/
	strcpy_s(filename,originfile);
	strcat_s(filename,bmp);
	Str_process(originfile);

	/*图像预处理*/
	Load_File(filename);//读图像
	Ostu_Process();//大津法二值化图像
	Carculate_Pic();//计算最小框

/*------------------------------------------------------------------------------------正向点------------------------------------------------------------------------------------*/
	Draw_Bound_Pic();//画最小包围框
	memcpy(bound_full_data,raw_data,500000);//复制
	In_Clean_Black(pos_inclean_name,0);

	while(center_dota_count<POS_FEATURE_COUNTS)
	{
		bound_points_count=Read_All_Boundpoints(0);//读取所有边缘点,得到边缘点个数
		Random_In_Numbers();//随机产生边缘内部POINT_COUNT个点，黑色点机多边形内部点
		Trans_2_Foundation();//随机选出的点和边缘点坐标系转移到 最小框左上角坐标系

		for(i=0;i<POINT_COUNT;i++)//初始化适应度
			adapt_value[i]=0;
		for(i=0;i<POINT_COUNT;i++)  //产生第一代种群 以及得到当前种群的适配值	   	
			adapt_value[i]=Build_Gnenration(&g[i],i);
		somepoints.clear();//清空初始随机选择的点向量
		
		crrnt_iterate=1;
		while((crrnt_iterate<max_iterator))//不超过最大代数
		{	
			Select_Generation(g,adapt_value);//选择
			Crossover(g,adapt_value,&is_broaden_mutation,0);//交叉
			Mutation(g,adapt_value,is_broaden_mutation,0);//变异
			crrnt_iterate++;	//代数增加
		}

		Trans_Back_Foundation(g);//特征点返回原图坐标系
		Trans_Boundpoints_2_Foundation();//边缘点返回原图坐标系
		
		memcpy(adapt_value_copy,adapt_value,POINT_COUNT*sizeof(float));
		Float_qsort(adapt_value_copy,0,sizeof(adapt_value_copy)/sizeof(float)-1);//快排
		max_frequency_number=0;
		for(int i=0;i<POINT_COUNT;i++)
			if(adapt_value_copy[POINT_COUNT-1]==adapt_value[i])
			{
				max_frequency_number=i;
				break;
			}			
		
		radius=(int)adapt_value[max_frequency_number];
		cicle_center_x=g[max_frequency_number].x;
		cicle_center_y=g[max_frequency_number].y;

		/*最小半径圆的圆心和半径*/
		if((bound_full_data[cicle_center_x*scanline+cicle_center_y]==255)&&(cicle_center_x!=topbound)&&(cicle_center_x!=downbound)&&(cicle_center_y!=rightbound)&&(cicle_center_y!=leftbound))
		{
			Draw_Dot_Circle(radius,cicle_center_x,cicle_center_y,0);//画圆边缘和圆心
			Save_Image(pos_compare_data_name,0);
			Save_Image(pos_bound_full_data_name,1);
		}

		boundpoints.clear();//清除现阶段边缘点	
		center_dota_count++;//特征点数量增加
	}



	/*------------------------------------------------------------------------------------反向点------------------------------------------------------------------------------------*/
	center_dota_count=0;//特征点数量清0
	Draw_Bound_Pic();
	In_Clean_Black(neg_inclean_name,1);
	while(center_dota_count<NEG_FEATURE_COUNTS)
	{
		bound_points_count=Read_All_Boundpoints(1);//读取所有边缘点,得到边缘点个数
		Random_Out_Numbers();//随机产生边缘外部POINT_COUNT个点
		Trans_2_Foundation();//随机选出的点和边缘点坐标系转移到 最小框左上角坐标系

		for(i=0;i<POINT_COUNT;i++)//初始化适应度
			adapt_value[i]=0;
		for(i=0;i<POINT_COUNT;i++)  //产生第一代种群 以及得到当前种群的适配值	        	
			adapt_value[i]=Build_Gnenration(&g[i],i);
		somepoints.clear();//清空初始随机选择的点向量

		crrnt_iterate=1;
		while((crrnt_iterate<max_iterator))//不超过最大代数
		{	
			Select_Generation(g,adapt_value);//选择
			Crossover(g,adapt_value,&is_broaden_mutation,1);//交叉
			Mutation(g,adapt_value,is_broaden_mutation,1);//变异
			crrnt_iterate++;	//代数增加
		}

		Trans_Back_Foundation(g);//特征点返回原图坐标系
		Trans_Boundpoints_2_Foundation();//边缘点返回原图坐标系
		
		memcpy(adapt_value_copy,adapt_value,POINT_COUNT*sizeof(float));
		Float_qsort(adapt_value_copy,0,sizeof(adapt_value_copy)/sizeof(float)-1);//快排
		max_frequency_number=0;
		for(int i=0;i<POINT_COUNT;i++)
			if(adapt_value_copy[POINT_COUNT-1]==adapt_value[i])
			{
				max_frequency_number=i;
				break;
			}			

		/*最小半径圆的圆心和半径*/
		radius=(int)adapt_value[max_frequency_number];
		cicle_center_x=g[max_frequency_number].x;
		cicle_center_y=g[max_frequency_number].y;
		if((bound_full_data[cicle_center_x*scanline+cicle_center_y]==0))
		{
			Draw_Dot_Circle(radius,cicle_center_x,cicle_center_y,1);//画圆边缘和圆心
			Save_Image(neg_compare_data_name,0);
			Save_Image(neg_bound_full_data_name,1);
		}
		boundpoints.clear();//清除现阶段边缘点	
		center_dota_count++;
	}

	return 1;
}


//函数有int choice参数的，均choice==0时，适用求正向点，choice==1时 适用反向点。

/*读取图像边缘点，无顺序要求*/
 int Read_All_Boundpoints(int choice)
 {
	 Point currPt;
	 int x,y,count=0;

	 if(choice == 0)
	 {
		/*最小包围框内读取边缘点，边缘点为黑色*/
		for(x=topbound;x<downbound;x++)
			for(y=leftbound;y<rightbound;y++)
			{
				if((compare_data[x*scanline+y]==255))
				{
					currPt.x=x;
					currPt.y=y;
					boundpoints.push_back(currPt);
					count++;
				}
			}
	 }
	 else if(choice == 1)
	 {
		 for(x=topbound;x<=downbound;x++)//包括最小包围框
		for(y=leftbound;y<=rightbound;y++)
		{
			if((compare_data[x*scanline+y]==255))
			{
				currPt.x=x;
				currPt.y=y;
				boundpoints.push_back(currPt);
				count++;
			}
		}
	 }
	return count;//返回边缘点个数
 }

/*随机找到二值图形状外部内的点当作初始点*/
void Random_Out_Numbers()
{
	int randx,randy,count=0,count1=0;
	Point currentPt;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;

	srand((unsigned)time(NULL)); //产生随机种子	

	//均匀分布白点需要优化
	while(1)
	{
		randx=rand()%tempheight+topbound;//随机点的范围在最小框内
		randy=rand()%tempwidth+leftbound;//
		if(bound_full_data[randx*scanline+randy]==0)//黑色
		{		
			if((randx>topbound+tempheight/3&&randy<topbound+tempheight*2/3)&&(randy>leftbound+tempwidth/3&&randy<leftbound+tempwidth*2/3))
				count1++;//没用
			else
			{
				currentPt.x=randx;
				currentPt.y=randy;
				somepoints.push_back(currentPt);
				count++;
			}
		}
		if(count==POINT_COUNT)
			break;
	}	
}

/*随机找到二值图里形状内部的点当作初始点*/
void Random_In_Numbers()
{
	int randx,randy,count=0,count1=0;
	Point currentPt;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;

	srand((unsigned)time(NULL)); //产生随机种子	

	while(1)
	{
		randx=rand()%tempheight+topbound;//随机点的范围在最小框内
		randy=rand()%tempwidth+leftbound;//
		if(bound_full_data[randx*scanline+randy]==255)//白点是内部点
		{		
			if((randx>topbound+tempheight/3&&randy<topbound+tempheight*2/3)&&(randy>leftbound+tempwidth/3&&randy<leftbound+tempwidth*2/3))
				count1++;//没用
			else
			{
				currentPt.x=randx;
				currentPt.y=randy;
				somepoints.push_back(currentPt);
				count++;
			}
		}
		if(count==POINT_COUNT)//初始点个数满足条件就退出
			break;
	}	
}

/*生成初始种群*/
float Build_Gnenration(struct Point *p,int i)
{
	*p=somepoints[i];
	
	return Get_Current_Gene_Adapt_Value(*p);
}

/*升序排列*/
bool SortBy( const float &v1, const float &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{  
    return v1< v2;//升序排列  
} 

/*得到适应度：轮廓点到当前点的最短距离*/
float Get_Current_Gene_Adapt_Value(struct Point p)
{
	int i;
	int boundlength=boundpoints.size();
	vector<float> neararray;//边缘点到现在点最短距离
	Point temp;
	float value=0,tempvalue=0;

	for(i=0;i<boundlength;i++)
	{
		temp=boundpoints[i];//取出轮廓点
		tempvalue=(temp.x-p.x)*(temp.x-p.x)+(temp.y-p.y)*(temp.y-p.y);//计算距离				
		neararray.push_back(sqrt(tempvalue));
	}		
	
	std::sort(neararray.begin(),neararray.end(),SortBy);//以点到边缘点的距离的升序排序
	value=neararray[0];//到边缘点的最短距离
	neararray.clear();
	
	return value;
}

/*轮盘赌选择*/
void Select_Generation(struct Point *p,float *adapt_value) 
{   
	float sum_adapt_value=0;
	float gambling[POINT_COUNT];    //轮盘赌
	float after_select_adapt_value[POINT_COUNT];  //选择以后的适配值
	float probability[POINT_COUNT];
	Point select_point_array[POINT_COUNT];
	int max_adapt_value_locate,after_select_max_adapt_value_locate;//最大适配值所在位置
	int i,j;
	
	for(i=0;i<POINT_COUNT;i++)//统计总的适应度
	{
		sum_adapt_value+=adapt_value[i];
	}
	for(i=0;i<POINT_COUNT;i++)//轮盘赌概率累加
	{
		probability[i]=adapt_value[i]/sum_adapt_value;
		if(i==0)
			gambling[i]=probability[i];		
		else
			gambling[i]=probability[i]+gambling[i-1];	
	}

	for(i=0;i<POINT_COUNT;i++)//轮盘赌选择
	{
		float gambling_proba=(rand()%10001)*1.0/10000; //赌轮选择概率
		
		for(j=0;j<POINT_COUNT;j++)  
		{
			if(gambling[j]>=gambling_proba)//第j个超过随机轮盘赌概率
				break;			
		}
		select_point_array[i]=p[j];
		after_select_adapt_value[i]=adapt_value[j];
	}

	if (!Is_NewGeneration_Optimise_OldGeneration(adapt_value,after_select_adapt_value,&max_adapt_value_locate,&after_select_max_adapt_value_locate))//选择后的种群是否比之前的适应度高,如果否，
	{
		select_point_array[after_select_max_adapt_value_locate]=p[max_adapt_value_locate];
		after_select_adapt_value[after_select_max_adapt_value_locate]=adapt_value[max_adapt_value_locate];
	}

	for(i=0;i<POINT_COUNT;i++)//选择后的种群优于之前的，选择后的种群拿去交叉变异
	{
		p[i]=select_point_array[i];
		adapt_value[i]=after_select_adapt_value[i];
	}
}

/*选择后的种群是否比之前的适应度高*/
bool Is_NewGeneration_Optimise_OldGeneration(float *adapt_value,float *after_select_adapt_value,int *max_adapt_value_locate,int *after_select_adapt_value_locate)
{
	int i,max_adapt_value_flag=-1,max_after_select_adapt_value_flag=-1;
	float max_adapt_value=0,max_after_select_adapt_value=0;
	
	for (i=0;i<POINT_COUNT;i++)
	{
		if (max_adapt_value<adapt_value[i])
		{
			max_adapt_value=adapt_value[i];
			max_adapt_value_flag=i;
		}
		if (max_after_select_adapt_value<after_select_adapt_value[i])
		{
			max_after_select_adapt_value=after_select_adapt_value[i];
			max_after_select_adapt_value_flag=i;
		}
	}
	*max_adapt_value_locate=max_adapt_value_flag;
	*after_select_adapt_value_locate=max_after_select_adapt_value_flag;
	return max_after_select_adapt_value>max_adapt_value;//选择操作后适应度最大值 与 选择前适应度最大值相比
}

/*随机选择交叉*/
void Crossover(struct Point *p,float value_adapt[],bool *is_broaden_mutation,int choice)  //交叉操作，包含交叉后产生的下一代的适配值 以及是否要扩大变异率，is_broaden_mutation=true，则扩大变异概率
{
	int step;     //设定交叉时的步长(第i 位与i+step位交叉),真正实现随机交叉
	int i,r=0;
	int current_locate=0;    //当前交叉位
	int cross_locate=0;     //与当前交叉位进行交叉的 交叉位
	
	Point temp_g[POINT_COUNT];      //将原来的种群复制一份，并在复制的种群中进行交叉操作
	float temp_adapt_value[POINT_COUNT];
	
	int max_adapt_value_locate,after_crossover_adapt_value_locate;
	Point max_point_array;
	bool is_new_generation_optimise=true;
	float max_adapt_value;
	bool is_over=false;   //随机交叉结束标识
	
	for (i=0;i<POINT_COUNT;i++)//将原来的种群复制一份，对临时种群做变异
	{
		temp_g[i]=p[i];
	}
	
	while ((step=rand()%(POINT_COUNT/2))==0)//产生随机步进，步长不超过种群数量的一半
	{
		;
	}

	while(current_locate<POINT_COUNT && !is_over)//从第一个基因开始随机选择第x个交叉，种群的一半完成交叉
	{
		for(i=0;i<step;i++)
		{	
			cross_locate=current_locate+step;
			if(cross_locate<POINT_COUNT)//保证对半交叉
			{	
				if(choice == 0)
				{
					BinaryCross(temp_g,current_locate,cross_locate,0);
					current_locate++;
				}
				else if(choice == 1)
				{
					BinaryCross(temp_g,current_locate,cross_locate,1);
					current_locate++;
				}
			}
			else
			{
				is_over=true;
				break;
			}
		}
		if(!is_over)
		{
			current_locate+=step;
		}
	}

	if (is_over && current_locate<POINT_COUNT)
	{
		i=current_locate;
		while (i<POINT_COUNT)
		{
			if (i+1<POINT_COUNT)
			{
				if(choice==0)
				{
					BinaryCross(temp_g,i,i+1,0);
					i+=2;
				}
				else
				{
					BinaryCross(temp_g,i,i+1,1);
					i+=2;				
				}
			}
			else
			{
				break;
			}
		}
	}

	/*---------------------------------------------------全部个体交叉结束--------------------------------------------------------------------------------*/
	
	for(i=0;i<POINT_COUNT;i++)
	{
		temp_adapt_value[i]=Get_Current_Gene_Adapt_Value(temp_g[i]);
	}

	if (!Is_NewGeneration_Optimise_OldGeneration(value_adapt,temp_adapt_value,&max_adapt_value_locate,&after_crossover_adapt_value_locate))
	{
		max_point_array=p[max_adapt_value_locate];
		max_adapt_value=value_adapt[max_adapt_value_locate];
		is_new_generation_optimise=false;
	}
	for (i=0;i<POINT_COUNT;i++)
	{
		p[i]=temp_g[i];
		value_adapt[i]=temp_adapt_value[i];
	}
	if (!is_new_generation_optimise)
	{
		p[after_crossover_adapt_value_locate]=max_point_array;
		value_adapt[after_crossover_adapt_value_locate]=max_adapt_value;
	}
	*is_broaden_mutation=!is_new_generation_optimise;

}

/*随机第x位开始交叉*/
void BinaryCross(struct Point *p,int x,int y,int choice)
{
	int array_x1[SAMPLE_LENGTH],array_x2[SAMPLE_LENGTH],array_y1[SAMPLE_LENGTH],array_y2[SAMPLE_LENGTH];
	int pxx=0,pxy=0,pyx=0,pyy=0;
	int i,cross_count;
	int temp;
	int new_x=0,new_y=0;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;
	int temp_pxx,temp_pyx,temp_pxy,temp_pyy;
	
	temp_pxx=(p+x)->x;
	temp_pyx=(p+y)->x;
	temp_pxy=(p+x)->y;
	temp_pyy=(p+y)->y;
	
	srand((unsigned)time(NULL)); //产生随机种子	
	
	Trans_To_Binary_Array(array_x1,(p+x),0);
	Trans_To_Binary_Array(array_x2,(p+y),0);
	Trans_To_Binary_Array(array_y1,(p+x),1);
	Trans_To_Binary_Array(array_y2,(p+y),1);

	//cross_count=rand()%SAMPLE_HEIGHT_BIT_COUNT+(SAMPLE_LENGTH-SAMPLE_HEIGHT_BIT_COUNT);//区间随机数
	cross_count=rand()%SAMPLE_LENGTH;
	for(i=cross_count;i<SAMPLE_LENGTH;i++)
	{
		temp=array_x1[i];
		array_x1[i]=array_x2[i];
		array_x2[i]=temp;
	}
	
	//cross_count=rand()%SAMPLE_WIDTH_BIT_COUNT+(SAMPLE_LENGTH-SAMPLE_WIDTH_BIT_COUNT);
	cross_count=rand()%SAMPLE_LENGTH;
	for(i=cross_count;i<SAMPLE_LENGTH;i++)
	{
		temp=array_y1[i];
		array_y1[i]=array_y2[i];
		array_y2[i]=temp;
	}

	pxx=Binary_To_Decimal(array_x1);
	pyx=Binary_To_Decimal(array_x2);
	pxy=Binary_To_Decimal(array_y1);
	pyy=Binary_To_Decimal(array_y2);

	if(choice == 0)//正向点
	{
		if(bound_full_data[(pxx+topbound)*scanline+(pxy+leftbound)]==255)//保证交叉点颜色为内部色
		{
			if(pxx>tempheight)//待精简	
				p[x].x=temp_pxx;	
			else
				p[x].x=pxx;
			if(pxy>tempwidth)
				p[x].y=temp_pxy;
			else
				p[x].y=pxy;
		}
		else
		{
			p[x].x=temp_pxx;
			p[x].y=temp_pxy;
		}

		if(bound_full_data[(pyx+topbound)*scanline+(pyy+leftbound)]==255)//保证交叉点颜色为内部色
		{
			if(pyx>tempheight)
				p[y].x=temp_pyx;
			else
				p[y].x=pyx;	
			if(pyy>tempwidth)
				p[y].y=temp_pyy;
			else
				p[y].y=pyy;	
		}
		else
		{
			p[y].x=temp_pyx;
			p[y].y=temp_pyy;
		}
	}
	else if(choice == 1)
	{
		if(bound_full_data[(pxx+topbound)*scanline+(pxy+leftbound)]==0)//保证交叉点颜色为外部色
		{
			if(pxx>tempheight)//待精简	
				p[x].x=temp_pxx;	
			else
				p[x].x=pxx;
			if(pxy>tempwidth)
				p[x].y=temp_pxy;
			else
				p[x].y=pxy;
		}
		else
		{
			p[x].x=temp_pxx;
			p[x].y=temp_pxy;
		}

		if(bound_full_data[(pyx+topbound)*scanline+(pyy+leftbound)]==0)//保证交叉点颜色为外部色
		{
			if(pyx>tempheight)
				p[y].x=temp_pyx;
			else
				p[y].x=pyx;	
			if(pyy>tempwidth)
				p[y].y=temp_pyy;
			else
				p[y].y=pyy;	
		}
		else
		{
			p[y].x=temp_pyx;
			p[y].y=temp_pyy;
		}
	}
 }

/*变异*/
void Mutation(Point pointsarray[],float *p_adapt_value, bool is_broaden_mutation,int choice) //变异操作 已经更新适配值
{   
	float max_adapt=-1;
	int r=0;    //随机数
	float prob;	//随机产生的变异概率
	
	r=rand()%1001;
	prob=r*1.0/1000;

	if(MUTATION_PERCENT>prob)//真正变异的地方
	{   
		r=rand()%POINT_COUNT;//疑问
		if(choice == 0)
			Mutation_PointArray(&pointsarray[r],0);
		else if(choice == 1)
			Mutation_PointArray(&pointsarray[r],1);
		p_adapt_value[r]=Get_Current_Gene_Adapt_Value(pointsarray[r]);
	}
}

/*变异*/
void Mutation_PointArray(Point *p,int choice) //对该节点进行变异
{    
	int i;
	int array_x[SAMPLE_LENGTH],array_y[SAMPLE_LENGTH];
	int px=0,py=0;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;
	int temp_px=p->x,temp_py=p->y;//变异点备份
	//int r_width=rand()%SAMPLE_WIDTH_BIT_COUNT+(9-SAMPLE_WIDTH_BIT_COUNT);//9
	//int r_height=rand()%SAMPLE_HEIGHT_BIT_COUNT+(9-SAMPLE_HEIGHT_BIT_COUNT);//8
	int r_width=rand()%SAMPLE_LENGTH;
	int r_height=rand()%SAMPLE_LENGTH;

	for(i=0;i<SAMPLE_LENGTH;i++)
	{
		array_x[i]=array_y[i]=0;
	}

	Trans_To_Binary_Array(array_x,p,0);
	Trans_To_Binary_Array(array_y,p,1);

	if(array_x[r_height]==1)
		array_x[r_height]=0;
	else
		array_x[r_height]=1;

	if(array_y[r_width]==1)
		array_y[r_width]=0;
	else
		array_y[r_width]=1;

	px=Binary_To_Decimal(array_x);
	py=Binary_To_Decimal(array_y);
	
	if(choice == 0)
	{
		if(bound_full_data[px*scanline+py]==255)//保证交叉点颜色为（图像内部点）,可以加循环，重新变异
		{
			if(px>tempheight)
				p->x=temp_px;
			else
				p->x=px;

			if(py>tempwidth)
				p->y=temp_py;
			else
				p->y=py;
		}
	}
	else if(choice == 1)
	{
		if(bound_full_data[px*scanline+py]==0)//保证交叉点颜色为（图像内部点）,可以加循环，重新变异
		{
			if(px>tempheight)
				p->x=temp_px;
			else
				p->x=px;

			if(py>tempwidth)
				p->y=temp_py;
			else
				p->y=py;
		}
	}
}

