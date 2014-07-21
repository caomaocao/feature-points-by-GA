#include "ga.h"

int main(void)
{
	char originfile[255]="290";//Դͼ���ļ���
	char filename[255];//Դͼ���ļ�
	
	Point g[POINT_COUNT]; //����
	int i,max_iterator=20;//����������
	int crrnt_iterate=1;     //��ǰ��������
	float adapt_value[POINT_COUNT],adapt_value_copy[POINT_COUNT]; //��ǰ���������ֵ
	bool is_broaden_mutation;   //�Ƿ����
	int bound_points_count;//��Ե�������������м����ߵı�Ե����                                                                                                               
	int center_dota_count=0;//��õ����������
	int max_frequency_number=0;//���Ȩֵ��λ��
	int radius,cicle_center_x=0,cicle_center_y=0;//�ֲ����Բ������뾶

	/*�ַ�������*/
	strcpy_s(filename,originfile);
	strcat_s(filename,bmp);
	Str_process(originfile);

	/*ͼ��Ԥ����*/
	Load_File(filename);//��ͼ��
	Ostu_Process();//��򷨶�ֵ��ͼ��
	Carculate_Pic();//������С��

/*------------------------------------------------------------------------------------�����------------------------------------------------------------------------------------*/
	Draw_Bound_Pic();//����С��Χ��
	memcpy(bound_full_data,raw_data,500000);//����
	In_Clean_Black(pos_inclean_name,0);

	while(center_dota_count<POS_FEATURE_COUNTS)
	{
		bound_points_count=Read_All_Boundpoints(0);//��ȡ���б�Ե��,�õ���Ե�����
		Random_In_Numbers();//���������Ե�ڲ�POINT_COUNT���㣬��ɫ���������ڲ���
		Trans_2_Foundation();//���ѡ���ĵ�ͱ�Ե������ϵת�Ƶ� ��С�����Ͻ�����ϵ

		for(i=0;i<POINT_COUNT;i++)//��ʼ����Ӧ��
			adapt_value[i]=0;
		for(i=0;i<POINT_COUNT;i++)  //������һ����Ⱥ �Լ��õ���ǰ��Ⱥ������ֵ	   	
			adapt_value[i]=Build_Gnenration(&g[i],i);
		somepoints.clear();//��ճ�ʼ���ѡ��ĵ�����
		
		crrnt_iterate=1;
		while((crrnt_iterate<max_iterator))//������������
		{	
			Select_Generation(g,adapt_value);//ѡ��
			Crossover(g,adapt_value,&is_broaden_mutation,0);//����
			Mutation(g,adapt_value,is_broaden_mutation,0);//����
			crrnt_iterate++;	//��������
		}

		Trans_Back_Foundation(g);//�����㷵��ԭͼ����ϵ
		Trans_Boundpoints_2_Foundation();//��Ե�㷵��ԭͼ����ϵ
		
		memcpy(adapt_value_copy,adapt_value,POINT_COUNT*sizeof(float));
		Float_qsort(adapt_value_copy,0,sizeof(adapt_value_copy)/sizeof(float)-1);//����
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

		/*��С�뾶Բ��Բ�ĺͰ뾶*/
		if((bound_full_data[cicle_center_x*scanline+cicle_center_y]==255)&&(cicle_center_x!=topbound)&&(cicle_center_x!=downbound)&&(cicle_center_y!=rightbound)&&(cicle_center_y!=leftbound))
		{
			Draw_Dot_Circle(radius,cicle_center_x,cicle_center_y,0);//��Բ��Ե��Բ��
			Save_Image(pos_compare_data_name,0);
			Save_Image(pos_bound_full_data_name,1);
		}

		boundpoints.clear();//����ֽ׶α�Ե��	
		center_dota_count++;//��������������
	}



	/*------------------------------------------------------------------------------------�����------------------------------------------------------------------------------------*/
	center_dota_count=0;//������������0
	Draw_Bound_Pic();
	In_Clean_Black(neg_inclean_name,1);
	while(center_dota_count<NEG_FEATURE_COUNTS)
	{
		bound_points_count=Read_All_Boundpoints(1);//��ȡ���б�Ե��,�õ���Ե�����
		Random_Out_Numbers();//���������Ե�ⲿPOINT_COUNT����
		Trans_2_Foundation();//���ѡ���ĵ�ͱ�Ե������ϵת�Ƶ� ��С�����Ͻ�����ϵ

		for(i=0;i<POINT_COUNT;i++)//��ʼ����Ӧ��
			adapt_value[i]=0;
		for(i=0;i<POINT_COUNT;i++)  //������һ����Ⱥ �Լ��õ���ǰ��Ⱥ������ֵ	        	
			adapt_value[i]=Build_Gnenration(&g[i],i);
		somepoints.clear();//��ճ�ʼ���ѡ��ĵ�����

		crrnt_iterate=1;
		while((crrnt_iterate<max_iterator))//������������
		{	
			Select_Generation(g,adapt_value);//ѡ��
			Crossover(g,adapt_value,&is_broaden_mutation,1);//����
			Mutation(g,adapt_value,is_broaden_mutation,1);//����
			crrnt_iterate++;	//��������
		}

		Trans_Back_Foundation(g);//�����㷵��ԭͼ����ϵ
		Trans_Boundpoints_2_Foundation();//��Ե�㷵��ԭͼ����ϵ
		
		memcpy(adapt_value_copy,adapt_value,POINT_COUNT*sizeof(float));
		Float_qsort(adapt_value_copy,0,sizeof(adapt_value_copy)/sizeof(float)-1);//����
		max_frequency_number=0;
		for(int i=0;i<POINT_COUNT;i++)
			if(adapt_value_copy[POINT_COUNT-1]==adapt_value[i])
			{
				max_frequency_number=i;
				break;
			}			

		/*��С�뾶Բ��Բ�ĺͰ뾶*/
		radius=(int)adapt_value[max_frequency_number];
		cicle_center_x=g[max_frequency_number].x;
		cicle_center_y=g[max_frequency_number].y;
		if((bound_full_data[cicle_center_x*scanline+cicle_center_y]==0))
		{
			Draw_Dot_Circle(radius,cicle_center_x,cicle_center_y,1);//��Բ��Ե��Բ��
			Save_Image(neg_compare_data_name,0);
			Save_Image(neg_bound_full_data_name,1);
		}
		boundpoints.clear();//����ֽ׶α�Ե��	
		center_dota_count++;
	}

	return 1;
}


//������int choice�����ģ���choice==0ʱ������������㣬choice==1ʱ ���÷���㡣

/*��ȡͼ���Ե�㣬��˳��Ҫ��*/
 int Read_All_Boundpoints(int choice)
 {
	 Point currPt;
	 int x,y,count=0;

	 if(choice == 0)
	 {
		/*��С��Χ���ڶ�ȡ��Ե�㣬��Ե��Ϊ��ɫ*/
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
		 for(x=topbound;x<=downbound;x++)//������С��Χ��
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
	return count;//���ر�Ե�����
 }

/*����ҵ���ֵͼ��״�ⲿ�ڵĵ㵱����ʼ��*/
void Random_Out_Numbers()
{
	int randx,randy,count=0,count1=0;
	Point currentPt;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;

	srand((unsigned)time(NULL)); //�����������	

	//���ȷֲ��׵���Ҫ�Ż�
	while(1)
	{
		randx=rand()%tempheight+topbound;//�����ķ�Χ����С����
		randy=rand()%tempwidth+leftbound;//
		if(bound_full_data[randx*scanline+randy]==0)//��ɫ
		{		
			if((randx>topbound+tempheight/3&&randy<topbound+tempheight*2/3)&&(randy>leftbound+tempwidth/3&&randy<leftbound+tempwidth*2/3))
				count1++;//û��
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

/*����ҵ���ֵͼ����״�ڲ��ĵ㵱����ʼ��*/
void Random_In_Numbers()
{
	int randx,randy,count=0,count1=0;
	Point currentPt;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;

	srand((unsigned)time(NULL)); //�����������	

	while(1)
	{
		randx=rand()%tempheight+topbound;//�����ķ�Χ����С����
		randy=rand()%tempwidth+leftbound;//
		if(bound_full_data[randx*scanline+randy]==255)//�׵����ڲ���
		{		
			if((randx>topbound+tempheight/3&&randy<topbound+tempheight*2/3)&&(randy>leftbound+tempwidth/3&&randy<leftbound+tempwidth*2/3))
				count1++;//û��
			else
			{
				currentPt.x=randx;
				currentPt.y=randy;
				somepoints.push_back(currentPt);
				count++;
			}
		}
		if(count==POINT_COUNT)//��ʼ����������������˳�
			break;
	}	
}

/*���ɳ�ʼ��Ⱥ*/
float Build_Gnenration(struct Point *p,int i)
{
	*p=somepoints[i];
	
	return Get_Current_Gene_Adapt_Value(*p);
}

/*��������*/
bool SortBy( const float &v1, const float &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{  
    return v1< v2;//��������  
} 

/*�õ���Ӧ�ȣ������㵽��ǰ�����̾���*/
float Get_Current_Gene_Adapt_Value(struct Point p)
{
	int i;
	int boundlength=boundpoints.size();
	vector<float> neararray;//��Ե�㵽���ڵ���̾���
	Point temp;
	float value=0,tempvalue=0;

	for(i=0;i<boundlength;i++)
	{
		temp=boundpoints[i];//ȡ��������
		tempvalue=(temp.x-p.x)*(temp.x-p.x)+(temp.y-p.y)*(temp.y-p.y);//�������				
		neararray.push_back(sqrt(tempvalue));
	}		
	
	std::sort(neararray.begin(),neararray.end(),SortBy);//�Ե㵽��Ե��ľ������������
	value=neararray[0];//����Ե�����̾���
	neararray.clear();
	
	return value;
}

/*���̶�ѡ��*/
void Select_Generation(struct Point *p,float *adapt_value) 
{   
	float sum_adapt_value=0;
	float gambling[POINT_COUNT];    //���̶�
	float after_select_adapt_value[POINT_COUNT];  //ѡ���Ժ������ֵ
	float probability[POINT_COUNT];
	Point select_point_array[POINT_COUNT];
	int max_adapt_value_locate,after_select_max_adapt_value_locate;//�������ֵ����λ��
	int i,j;
	
	for(i=0;i<POINT_COUNT;i++)//ͳ���ܵ���Ӧ��
	{
		sum_adapt_value+=adapt_value[i];
	}
	for(i=0;i<POINT_COUNT;i++)//���̶ĸ����ۼ�
	{
		probability[i]=adapt_value[i]/sum_adapt_value;
		if(i==0)
			gambling[i]=probability[i];		
		else
			gambling[i]=probability[i]+gambling[i-1];	
	}

	for(i=0;i<POINT_COUNT;i++)//���̶�ѡ��
	{
		float gambling_proba=(rand()%10001)*1.0/10000; //����ѡ�����
		
		for(j=0;j<POINT_COUNT;j++)  
		{
			if(gambling[j]>=gambling_proba)//��j������������̶ĸ���
				break;			
		}
		select_point_array[i]=p[j];
		after_select_adapt_value[i]=adapt_value[j];
	}

	if (!Is_NewGeneration_Optimise_OldGeneration(adapt_value,after_select_adapt_value,&max_adapt_value_locate,&after_select_max_adapt_value_locate))//ѡ������Ⱥ�Ƿ��֮ǰ����Ӧ�ȸ�,�����
	{
		select_point_array[after_select_max_adapt_value_locate]=p[max_adapt_value_locate];
		after_select_adapt_value[after_select_max_adapt_value_locate]=adapt_value[max_adapt_value_locate];
	}

	for(i=0;i<POINT_COUNT;i++)//ѡ������Ⱥ����֮ǰ�ģ�ѡ������Ⱥ��ȥ�������
	{
		p[i]=select_point_array[i];
		adapt_value[i]=after_select_adapt_value[i];
	}
}

/*ѡ������Ⱥ�Ƿ��֮ǰ����Ӧ�ȸ�*/
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
	return max_after_select_adapt_value>max_adapt_value;//ѡ���������Ӧ�����ֵ �� ѡ��ǰ��Ӧ�����ֵ���
}

/*���ѡ�񽻲�*/
void Crossover(struct Point *p,float value_adapt[],bool *is_broaden_mutation,int choice)  //�������������������������һ��������ֵ �Լ��Ƿ�Ҫ��������ʣ�is_broaden_mutation=true��������������
{
	int step;     //�趨����ʱ�Ĳ���(��i λ��i+stepλ����),����ʵ���������
	int i,r=0;
	int current_locate=0;    //��ǰ����λ
	int cross_locate=0;     //�뵱ǰ����λ���н���� ����λ
	
	Point temp_g[POINT_COUNT];      //��ԭ������Ⱥ����һ�ݣ����ڸ��Ƶ���Ⱥ�н��н������
	float temp_adapt_value[POINT_COUNT];
	
	int max_adapt_value_locate,after_crossover_adapt_value_locate;
	Point max_point_array;
	bool is_new_generation_optimise=true;
	float max_adapt_value;
	bool is_over=false;   //������������ʶ
	
	for (i=0;i<POINT_COUNT;i++)//��ԭ������Ⱥ����һ�ݣ�����ʱ��Ⱥ������
	{
		temp_g[i]=p[i];
	}
	
	while ((step=rand()%(POINT_COUNT/2))==0)//�������������������������Ⱥ������һ��
	{
		;
	}

	while(current_locate<POINT_COUNT && !is_over)//�ӵ�һ������ʼ���ѡ���x�����棬��Ⱥ��һ����ɽ���
	{
		for(i=0;i<step;i++)
		{	
			cross_locate=current_locate+step;
			if(cross_locate<POINT_COUNT)//��֤�԰뽻��
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

	/*---------------------------------------------------ȫ�����彻�����--------------------------------------------------------------------------------*/
	
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

/*�����xλ��ʼ����*/
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
	
	srand((unsigned)time(NULL)); //�����������	
	
	Trans_To_Binary_Array(array_x1,(p+x),0);
	Trans_To_Binary_Array(array_x2,(p+y),0);
	Trans_To_Binary_Array(array_y1,(p+x),1);
	Trans_To_Binary_Array(array_y2,(p+y),1);

	//cross_count=rand()%SAMPLE_HEIGHT_BIT_COUNT+(SAMPLE_LENGTH-SAMPLE_HEIGHT_BIT_COUNT);//���������
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

	if(choice == 0)//�����
	{
		if(bound_full_data[(pxx+topbound)*scanline+(pxy+leftbound)]==255)//��֤�������ɫΪ�ڲ�ɫ
		{
			if(pxx>tempheight)//������	
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

		if(bound_full_data[(pyx+topbound)*scanline+(pyy+leftbound)]==255)//��֤�������ɫΪ�ڲ�ɫ
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
		if(bound_full_data[(pxx+topbound)*scanline+(pxy+leftbound)]==0)//��֤�������ɫΪ�ⲿɫ
		{
			if(pxx>tempheight)//������	
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

		if(bound_full_data[(pyx+topbound)*scanline+(pyy+leftbound)]==0)//��֤�������ɫΪ�ⲿɫ
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

/*����*/
void Mutation(Point pointsarray[],float *p_adapt_value, bool is_broaden_mutation,int choice) //������� �Ѿ���������ֵ
{   
	float max_adapt=-1;
	int r=0;    //�����
	float prob;	//��������ı������
	
	r=rand()%1001;
	prob=r*1.0/1000;

	if(MUTATION_PERCENT>prob)//��������ĵط�
	{   
		r=rand()%POINT_COUNT;//����
		if(choice == 0)
			Mutation_PointArray(&pointsarray[r],0);
		else if(choice == 1)
			Mutation_PointArray(&pointsarray[r],1);
		p_adapt_value[r]=Get_Current_Gene_Adapt_Value(pointsarray[r]);
	}
}

/*����*/
void Mutation_PointArray(Point *p,int choice) //�Ըýڵ���б���
{    
	int i;
	int array_x[SAMPLE_LENGTH],array_y[SAMPLE_LENGTH];
	int px=0,py=0;
	int tempheight=downbound-topbound;
	int tempwidth=rightbound-leftbound;
	int temp_px=p->x,temp_py=p->y;//����㱸��
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
		if(bound_full_data[px*scanline+py]==255)//��֤�������ɫΪ��ͼ���ڲ��㣩,���Լ�ѭ�������±���
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
		if(bound_full_data[px*scanline+py]==0)//��֤�������ɫΪ��ͼ���ڲ��㣩,���Լ�ѭ�������±���
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

