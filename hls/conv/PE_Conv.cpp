#include "PE_Conv.h"

/*Weights Buffer�Ծ����Ϊ��Ԫ*/
/*��ÿ������˵ļ�����п���,����������Ⱥ�����*/
/*Fmap Buffer��һ������������һ��fmap������Ϊ��Ԫ*/
/*Register Define*/
//0:Kernel_size
//1:Kernel_channel
//2:Kernel_number
//3:Kernel_bias_number
//4:conv_stride
//5:conv_padding
//6:input_fmap_size
//7:input_fmap_channel
//8:output_fmap_size
//9:output_fmap_channel
//10:Calculate Mode (����ģʽ:��ͨ���,��Ⱦ��,�����,MobileNET V2���

void PE_conv2d_cal(
	reg_t Register[MAX_PE_Register_NUM],
	data_t BUFFER_Kernel[MAX_PE_Kernel_channel][MAX_PE_Kernel_size][MAX_PE_Kernel_size],
	data_t BUFFER_Bias[MAX_BUFFER_Bias_number],
	data_t BUFFER_Input_fmap[MAX_PE_Input_fmap_channel][MAX_BUFFER_Input_fmap_size][MAX_BUFFER_Input_fmap_size],
	data_t BUFFER_Output_fmap[MAX_BUFFER_Output_fmap_size]
)
{
	int kernel_row, kernel_col, kernel_channel, kernel_number; //�����ɨ�����
	int shift_col_num;//������� ���ƴ���
	int shift_row_num;//������� ���ƴ���
	int bias_number;//ƫ�ø���,һ��output fmap��Ӧһ��bias
	//�Ծ����Ϊ��λ����ɨ��(ɨ��˳���ɴ���С|����,ͨ��,��,��)
	LOOP_cal_Data:
	for (kernel_channel = 0; kernel_channel < ((Register[10]==0)?Register[1]: 1); kernel_channel++)//0:��ͨ���Kernel_channel=Kernel_channel(���ͨ��) 1:��ͨ�����Kernel_channel=1 1�������ֻ�е�һ��ͨ�������һ��ͨ����fmap//Kernel_number(���������)
	{
		for (kernel_row = 0; kernel_row < Register[0]; kernel_row++)//Kernel_size
		{
			for (kernel_col = 0; kernel_col < Register[0]; kernel_col++)//Kernel_size
			{
				/*+bias*/
				/*�ǳ���Ҫ:��ռ����м��� ����0,0,0λ�õ���Ȩ��,���ڶ�μ�����˵,�������һ�εļ�����,����Ӱ�챾�μ���*/
				if ((kernel_row + kernel_col + kernel_channel) == 0)
				{
					BUFFER_Output_fmap[MAX_BUFFER_Output_fmap_size-1] = BUFFER_Bias[MAX_BUFFER_Bias_number-1];
				}
				/**weight*/
				BUFFER_Output_fmap[MAX_BUFFER_Output_fmap_size-1] += BUFFER_Kernel[((Register[10] == 0) ? kernel_channel : 0)][kernel_row][kernel_col] * BUFFER_Input_fmap[kernel_channel][kernel_row][kernel_col];
			}
		}
	}
}
/*��ȡһ��BUFFER����������ͼ��1��Kernel������*/
void PE_load_Data_Weights_Buffer(
	/*��DDR����ȡ��һά����*/
	reg_t PE_Register[MAX_PE_Register_NUM],
	reg_t Reg_Inside_Kernel_number,//����ľ�������
	reg_t shift_row,//input fmap���Ʒ������� Ҳ����������
	reg_t shift_col,//input fmap���Ʒ������� Ҳ����������
	data_t PE_Kernel[MAX_PE_Kernel_number * MAX_PE_Kernel_channel * MAX_PE_Kernel_size * MAX_PE_Kernel_size],
	data_t PE_Bias[MAX_PE_Bias_number],
	data_t PE_Input_fmap[MAX_PE_Input_fmap_channel * MAX_PE_Input_fmap_size * MAX_PE_Input_fmap_size],
	/*���ɵ�����ά��Buffer����*/
	data_t BUFFER_Kernel[MAX_PE_Kernel_channel][MAX_PE_Kernel_size][MAX_PE_Kernel_size],
	data_t BUFFER_Bias[MAX_BUFFER_Bias_number],
	data_t BUFFER_Input_fmap[MAX_PE_Input_fmap_channel][MAX_BUFFER_Input_fmap_size][MAX_BUFFER_Input_fmap_size]
)
{
	LOOP_Load_Data:
	for (int i = 0; i < PE_Register[1]; i++)//Kernel_channel
	{
		for (int j = 0; j < PE_Register[0]; j++)//Kernel_size
		{
			for (int k = 0; k < PE_Register[0]; k++)//Kernel_size
			{
				BUFFER_Bias[MAX_BUFFER_Bias_number - 1] = PE_Bias[Reg_Inside_Kernel_number];
				BUFFER_Kernel[i][j][k] = PE_Kernel[(Reg_Inside_Kernel_number*PE_Register[1]*PE_Register[0]*PE_Register[0]) + (i*PE_Register[0]*PE_Register[0]) + (j*PE_Register[0]) + (k)];
				BUFFER_Input_fmap[i][j][k] = PE_Input_fmap[(shift_col * PE_Register[4]) + (shift_row * PE_Register[4] * PE_Register[6]) + (i*PE_Register[6]*PE_Register[6]) + (j*PE_Register[6]) + (k)];
			}
		}
	}
}
/*�洢��������PE����ķ�fmap��(�������洢λ����shift_col��shift_row���*/
void PE_store_Output_fmap(
	/*PE���ƼĴ���*/
	reg_t PE_Register[MAX_PE_Register_NUM],
	reg_t Reg_Inside_Kernel_number,//�������Դ(��������)-<������ŵ�PE�������ͼ��ͨ��>
	reg_t shift_row,//input fmap���Ʒ������� Ҳ����������
	reg_t shift_col,//input fmap���Ʒ������� Ҳ����������
	/*Buffer�����������(һ����)*/
	data_t BUFFER_Output_fmap[MAX_BUFFER_Output_fmap_size],
	/*PE����ļ�����*/
	data_t PE_Output_fmap[MAX_PE_Outputput_fmap_channel * MAX_PE_Output_fmap_size * MAX_PE_Output_fmap_size]
)
{
	PE_Output_fmap[(Reg_Inside_Kernel_number*PE_Register[8]*PE_Register[8]) + (shift_row*PE_Register[8])+ (shift_col)] = BUFFER_Output_fmap[MAX_BUFFER_Output_fmap_size - 1];
}
void PE_conv2d(
	/*PE���ƼĴ���*/
	 reg_t *PE_Register,
	/*��������Ȩ��*/
	 data_t *PE_Kernel,
	/*��������ƫ��*/
	 data_t *PE_Bias,
	/*������������ͼ*/
	 data_t *PE_Input_fmap,
	/*PE�������ͼ*/
	 data_t *PE_Output_fmap
)
{
#pragma HLS INTERFACE m_axi bundle=INPUT_Reg depth=13 port=PE_Register offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Kernel depth=9216 port=PE_Kernel offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Bias depth=1024 port=PE_Bias offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Fmap depth=104857600 port=PE_Input_fmap offset=slave
#pragma HLS INTERFACE m_axi bundle=OUTPUT_Fmap depth=104857600 port=PE_Output_fmap offset=slave
#pragma HLS INTERFACE s_axilite bundle=CTRL_BUS port=return

	/*˫����*/
	/*Buffer1*/
	data_t BUFFER1_Kernel[MAX_PE_Kernel_channel][MAX_PE_Kernel_size][MAX_PE_Kernel_size];
	data_t BUFFER1_Bias[MAX_BUFFER_Bias_number];
	data_t BUFFER1_Input_fmap[MAX_PE_Input_fmap_channel][MAX_BUFFER_Input_fmap_size][MAX_BUFFER_Input_fmap_size];
	data_t BUFFER1_Output_fmap[MAX_BUFFER_Output_fmap_size];
	/*Buffer2*/
	data_t BUFFER2_Kernel[MAX_PE_Kernel_channel][MAX_PE_Kernel_size][MAX_PE_Kernel_size];
	data_t BUFFER2_Bias[MAX_BUFFER_Bias_number];
	data_t BUFFER2_Input_fmap[MAX_PE_Input_fmap_channel][MAX_BUFFER_Input_fmap_size][MAX_BUFFER_Input_fmap_size];
	data_t BUFFER2_Output_fmap[MAX_BUFFER_Output_fmap_size];
	//(�ȼ���i)reg_t Reg_Inside_Kernel_number = 0;//�������Դ(��������)-<������ŵ�PE�������ͼ��ͨ��>
	//(�ȼ���j)reg_t shift_row = 0; //input fmap��������
	//(�ȼ���k)reg_t shift_col = 0; //input fmap��������

	LOOP_Kernel_number:
	for (reg_t Reg_Inside_Kernel_number = 0; Reg_Inside_Kernel_number < PE_Register[2]; Reg_Inside_Kernel_number++)//Kernel_number
	{
		/*�������λ*/
		bool BUFFER_CONTROL = true;
		bool BUFFER_START = true;//ÿ������˿�ʼʱ���BUFFER
		if (BUFFER_START == true)
		{
			/*��DDR����ȡһ��Buffer1������*/
			PE_load_Data_Weights_Buffer(
				/*��DDR����ȡ��һά����*/
				PE_Register,
				Reg_Inside_Kernel_number,//����ľ�������
				0,//input fmap���Ʒ������� Ҳ����������
				0,//input fmap���Ʒ������� Ҳ����������
				PE_Kernel,
				PE_Bias,
				PE_Input_fmap,
				/*���ɵ�����ά��Buffer����*/
				BUFFER1_Kernel,
				BUFFER1_Bias,
				BUFFER1_Input_fmap
			);
		}
		LOOP_Shift_row:
		for (reg_t shift_row = 0; shift_row < PE_Register[8]; shift_row++)//output_fmap_size
		{
			LOOP_Shift_col:
			for (reg_t shift_col = 0; shift_col < PE_Register[8]; shift_col++)//output_fmap_size
			{
				/*�л���������*/
				/*index +1�������Խ���ķ���,��Ϊ���һ��bufferֻ�Ǳ�Խ�����,��û�б�Խ�����洢*/
				/*����Ϊ�˱���,��ֹFPGAӲ����·Խ�����BRAM��ɲ�ȷ������,��Ҫ���if����ֹԽ����ʵĲ���*/
				if (BUFFER_CONTROL)
				{
					/*��DDR����ȡһ��Buffer2������*/
					if((shift_col + 1) != PE_Register[8])//��ֹԽ�����
					PE_load_Data_Weights_Buffer(
						/*��DDR����ȡ��һά����*/
						PE_Register,
						Reg_Inside_Kernel_number,//����ľ�������
						shift_row,//input fmap���Ʒ������� Ҳ����������
						shift_col + 1,//input fmap���Ʒ������� Ҳ����������
						PE_Kernel,
						PE_Bias,
						PE_Input_fmap,
						/*���ɵ�����ά��Buffer����*/
						BUFFER2_Kernel,
						BUFFER2_Bias,
						BUFFER2_Input_fmap
					);
				}
				else
				{
					/*��DDR����ȡһ��Buffer1������*/
					if ((shift_col + 1) != PE_Register[8])//��ֹԽ�����
					PE_load_Data_Weights_Buffer(
						/*��DDR����ȡ��һά����*/
						PE_Register,
						Reg_Inside_Kernel_number,//����ľ�������
						shift_row,//input fmap���Ʒ������� Ҳ����������
						shift_col + 1,//input fmap���Ʒ������� Ҳ����������
						PE_Kernel,
						PE_Bias,
						PE_Input_fmap,
						/*���ɵ�����ά��Buffer����*/
						BUFFER1_Kernel,
						BUFFER1_Bias,
						BUFFER1_Input_fmap
					);
				}

				if (BUFFER_CONTROL)
				{
					/*����һ��Buffer1������*/
					PE_conv2d_cal(PE_Register, BUFFER1_Kernel, BUFFER1_Bias, BUFFER1_Input_fmap, BUFFER1_Output_fmap);
					/*�洢һ��Buffer1�����ݻ�DDR*/
					PE_store_Output_fmap(
						PE_Register,
						Reg_Inside_Kernel_number,//�������Դ(��������)-<������ŵ�PE�������ͼ��ͨ��>
						shift_row,//input fmap��������
						shift_col,//input fmap��������
						/*Buffer�����������(һ����)*/
						BUFFER1_Output_fmap,
						/*PE����ļ�����*/
						PE_Output_fmap
					);
					BUFFER_CONTROL = false;
				}
				else
				{
					/*����һ��Buffer2������*/
					PE_conv2d_cal(PE_Register, BUFFER2_Kernel, BUFFER2_Bias, BUFFER2_Input_fmap, BUFFER2_Output_fmap);
					/*�洢һ��Buffer2�����ݻ�DDR*/
					PE_store_Output_fmap(
						PE_Register,
						Reg_Inside_Kernel_number,//�������Դ(��������)-<������ŵ�PE�������ͼ��ͨ��>
						shift_row,//input fmap��������
						shift_col,//input fmap��������
						/*Buffer�����������(һ����)*/
						BUFFER2_Output_fmap,
						/*PE����ļ�����*/
						PE_Output_fmap
					);
					BUFFER_CONTROL = true;
				}
			}
		}
	}
}
