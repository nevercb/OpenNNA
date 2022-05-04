/*FPGA��������� PE��ԪHLS����*/
/*����:xddcore*/
/*05/10/2021*/

/*
 * ÿ������ΪUINT8����,UINT8���;
 * �����Ȩ��ΪUint8
 */
#include "process_element.h"
#include <cstring>
data_t L1_output_fmap[L1_output_fmap_channel][L1_output_fmap_size][L1_output_fmap_size] = { 0 };
data_t L1_output_fmap_ReLu[L1_output_fmap_channel][L1_output_fmap_size][L1_output_fmap_size] = { 0 };

data_t L2_output_fmap[L2_output_fmap_channel][L2_output_fmap_size][L2_output_fmap_size] = { 0 };

data_t L3_output_fmap[L3_output_fmap_channel][L3_output_fmap_size][L3_output_fmap_size] = { 0 };
data_t L3_output_fmap_ReLu[L3_output_fmap_channel][L3_output_fmap_size][L3_output_fmap_size] = { 0 };


data_t L4_output_fmap[L4_output_fmap_channel][L4_output_fmap_size][L4_output_fmap_size] = { 0 };
data_t L4_flatten_output_map[L5_input_fmap_size] = { 0 };

data_t L5_output_fmap[L5_output_fmap_size] = {};
data_t L5_output_fmap_ReLu[L5_output_fmap_size] = {};

data_t L6_output_fmap[L6_output_fmap_size] = {};
/*���ĺ���*/
data_t Relu(data_t x)
{
	return (x < 0) ? (data_t)0 : x;
}
void L1_conv2d(
	data_t Kernel[L1_Kernel_number][L1_Kernel_channel][L1_Kernel_size][L1_Kernel_size],
	data_t bias[L1_bias_number],
	data_t input_fmap[L1_input_fmap_channel][L1_input_fmap_size][L1_input_fmap_size],
	data_t output_fmap[L1_output_fmap_channel][L1_output_fmap_size][L1_output_fmap_size]
)
{
	//int fmap_row, fmap_col, fmap_channel; //����ͼɨ�����
	int kernel_row, kernel_col, kernel_channel, kernel_number; //�����ɨ�����
	int shift_col_num;//������� ���ƴ���
	int shift_row_num;//������� ���ƴ���
	int bias_number;//ƫ�ø���,һ��output fmap��Ӧһ��bias
	//�Ծ����Ϊ��λ����ɨ��(ɨ��˳���ɴ���С|����,ͨ��,��,��)
	for (kernel_number = 0; kernel_number < L1_Kernel_number; kernel_number++)
	{
		for (kernel_channel = 0; kernel_channel < L1_Kernel_channel; kernel_channel++)
		{
			for (shift_col_num = 0; shift_col_num < (((L1_input_fmap_size + 2 * L1_conv_padding - L1_Kernel_size) / L1_conv_stride) + 1); shift_col_num++)
			{
				for (shift_row_num = 0; shift_row_num < (((L1_input_fmap_size + 2 * L1_conv_padding - L1_Kernel_size) / L1_conv_stride) + 1); shift_row_num++)
				{
					for (kernel_row = 0; kernel_row < L1_Kernel_size; kernel_row++)
					{
						for (kernel_col = 0; kernel_col < L1_Kernel_size; kernel_col++)
						{
							output_fmap[kernel_number][shift_col_num][shift_row_num] += ((shift_col_num == 0 || shift_row_num == 0) && L1_output_padding != 0) ? (data_t)0 : (data_t)(((Kernel[kernel_number][kernel_channel][kernel_row][kernel_col] * input_fmap[kernel_channel][kernel_row + shift_col_num * L1_conv_stride][kernel_col + shift_row_num * L1_conv_stride])) + (((kernel_row + kernel_col + kernel_channel) == (L1_Kernel_size + L1_Kernel_size + L1_Kernel_channel - 3)) ? bias[kernel_number] : (data_t)0));
						}
					}
				}
			}
		}
	}
}

void L2_maxpool2d(
	data_t input_fmap[L2_input_fmap_channel][L2_input_fmap_size][L2_input_fmap_size],
	data_t output_fmap[L2_output_fmap_channel][L2_output_fmap_size][L2_output_fmap_size]
)
{
	int poolkernel_row, poolkernel_col; //���ػ���ɨ�����
	int shift_col_num;//������� ���ƴ���
	int shift_row_num;//������� ���ƴ���
	int fmap_channel;//����ͼͨ��
	data_t max_value;
	for (fmap_channel = 0; fmap_channel < L2_output_fmap_channel; fmap_channel++)
	{
		for (shift_col_num = 0; shift_col_num < (((L2_input_fmap_size - L2_MaxpoolKernel_size) / L2_MaxpoolKernel_stride) + 1); shift_col_num++)
		{
			for (shift_row_num = 0; shift_row_num < (((L2_input_fmap_size - L2_MaxpoolKernel_size) / L2_MaxpoolKernel_stride) + 1); shift_row_num++)
			{
				for (poolkernel_row = 0; poolkernel_row < L2_MaxpoolKernel_size; poolkernel_row++)
				{
					for (poolkernel_col = 0; poolkernel_col < L2_MaxpoolKernel_size; poolkernel_col++)
					{
						max_value = (poolkernel_row == 0 && poolkernel_col == 0) ? input_fmap[fmap_channel][poolkernel_row + shift_col_num * L2_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L2_MaxpoolKernel_stride] : (input_fmap[fmap_channel][poolkernel_row + shift_col_num * L2_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L2_MaxpoolKernel_stride] > max_value) ? input_fmap[fmap_channel][poolkernel_row + shift_col_num * L2_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L2_MaxpoolKernel_stride] : max_value;
					}
				}
				output_fmap[fmap_channel][shift_col_num][shift_row_num] = max_value;
			}
		}
	}
}

void L3_conv2d(
	data_t Kernel[L3_Kernel_number][L3_Kernel_channel][L3_Kernel_size][L3_Kernel_size],
	data_t bias[L3_bias_number],
	data_t input_fmap[L3_input_fmap_channel][L3_input_fmap_size][L3_input_fmap_size],
	data_t output_fmap[L3_output_fmap_channel][L3_output_fmap_size][L3_output_fmap_size]
)
{
	//int fmap_row, fmap_col, fmap_channel; //����ͼɨ�����
	int kernel_row, kernel_col, kernel_channel, kernel_number; //�����ɨ�����
	int shift_col_num;//������� ���ƴ���
	int shift_row_num;//������� ���ƴ���
	int bias_number;//ƫ�ø���,һ��output fmap��Ӧһ��bias
	//�Ծ����Ϊ��λ����ɨ��(ɨ��˳���ɴ���С|����,ͨ��,��,��)
	for (kernel_number = 0; kernel_number < L3_Kernel_number; kernel_number++)
	{
		for (kernel_channel = 0; kernel_channel < L3_Kernel_channel; kernel_channel++)
		{
			for (shift_col_num = 0; shift_col_num < (((L3_input_fmap_size + 2 * L3_conv_padding - L3_Kernel_size) / L3_conv_stride) + 1); shift_col_num++)
			{
				for (shift_row_num = 0; shift_row_num < (((L3_input_fmap_size + 2 * L3_conv_padding - L3_Kernel_size) / L3_conv_stride) + 1); shift_row_num++)
				{
					for (kernel_row = 0; kernel_row < L3_Kernel_size; kernel_row++)
					{
						for (kernel_col = 0; kernel_col < L3_Kernel_size; kernel_col++)
						{
							output_fmap[kernel_number][shift_col_num][shift_row_num] += ((shift_col_num == 0 || shift_row_num == 0) && L3_output_padding != 0) ? (data_t)0 : (data_t)(((Kernel[kernel_number][kernel_channel][kernel_row][kernel_col] * input_fmap[kernel_channel][kernel_row + shift_col_num * L3_conv_stride][kernel_col + shift_row_num * L3_conv_stride])) + (((kernel_row + kernel_col + kernel_channel) == (L3_Kernel_size + L3_Kernel_size + L3_Kernel_channel - 3)) ? bias[kernel_number] : (data_t)0));
						}
					}
				}
			}
		}
	}
}

void L4_maxpool2d(
	data_t input_fmap[L4_input_fmap_channel][L4_input_fmap_size][L4_input_fmap_size],
	data_t output_fmap[L4_output_fmap_channel][L4_output_fmap_size][L4_output_fmap_size]
)
{
	int poolkernel_row, poolkernel_col; //���ػ���ɨ�����
	int shift_col_num;//������� ���ƴ���
	int shift_row_num;//������� ���ƴ���
	int fmap_channel;//����ͼͨ��
	data_t max_value;
	for (fmap_channel = 0; fmap_channel < L4_output_fmap_channel; fmap_channel++)
	{
		for (shift_col_num = 0; shift_col_num < (((L4_input_fmap_size - L4_MaxpoolKernel_size) / L4_MaxpoolKernel_stride) + 1); shift_col_num++)
		{
			for (shift_row_num = 0; shift_row_num < (((L4_input_fmap_size - L4_MaxpoolKernel_size) / L4_MaxpoolKernel_stride) + 1); shift_row_num++)
			{
				for (poolkernel_row = 0; poolkernel_row < L4_MaxpoolKernel_size; poolkernel_row++)
				{
					for (poolkernel_col = 0; poolkernel_col < L4_MaxpoolKernel_size; poolkernel_col++)
					{
						max_value = (poolkernel_row == 0 && poolkernel_col == 0) ? input_fmap[fmap_channel][poolkernel_row + shift_col_num * L4_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L4_MaxpoolKernel_stride] : (input_fmap[fmap_channel][poolkernel_row + shift_col_num * L4_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L4_MaxpoolKernel_stride] > max_value) ? input_fmap[fmap_channel][poolkernel_row + shift_col_num * L4_MaxpoolKernel_stride][poolkernel_col + shift_row_num * L4_MaxpoolKernel_stride] : max_value;
					}
				}
				output_fmap[fmap_channel][shift_col_num][shift_row_num] = max_value;
			}
		}
	}
}

void L5_dense(
	data_t neure_weights[L5_neure_number][L5_input_fmap_size],
	data_t neure_bias[L5_neure_number],
	data_t input_fmap[L5_input_fmap_size],
	data_t output_fmap[L5_output_fmap_size]
)
{
	int input_fmap_size; //����һά����ͼ��С
	int weights_bias_number;//Ȩ�غ�ƫ�ü���(һ��neura->1 weights + 1 bias)
	for (weights_bias_number = 0; weights_bias_number < L5_neure_number; weights_bias_number++)
	{
		for (input_fmap_size = 0; input_fmap_size < L5_input_fmap_size; input_fmap_size++)
		{
			output_fmap[weights_bias_number] += (input_fmap[input_fmap_size] * neure_weights[weights_bias_number][input_fmap_size]); //+ neure_bias[weights_bias_number];

		}
		output_fmap[weights_bias_number] += neure_bias[weights_bias_number];
	}
}

void L6_dense(
	data_t neure_weights[L6_neure_number][L6_input_fmap_size],
	data_t neure_bias[L6_neure_number],
	data_t input_fmap[L6_input_fmap_size],
	data_t output_fmap[L6_output_fmap_size]
)
{
	int input_fmap_size; //����һά����ͼ��С
	int weights_bias_number;//Ȩ�غ�ƫ�ü���(һ��neura->1 weights + 1 bias)
	for (weights_bias_number = 0; weights_bias_number < L6_neure_number; weights_bias_number++)
	{
		for (input_fmap_size = 0; input_fmap_size < L6_input_fmap_size; input_fmap_size++)
		{
			output_fmap[weights_bias_number] += (input_fmap[input_fmap_size] * neure_weights[weights_bias_number][input_fmap_size]); //+ neure_bias[weights_bias_number];

		}
		output_fmap[weights_bias_number] += neure_bias[weights_bias_number];
	}
}

int process_element(
		data_t *input_fmap[L1_input_fmap_size*L1_input_fmap_size],
		data_t *output_fmap[L6_output_fmap_size]
					)
{
#pragma HLS INTERFACE m_axi port=input_fmap depth=1024 offset=slave bundle=INPUT
#pragma HLS INTERFACE m_axi port=output_fmap depth=10 offset=slave bundle=OUTPUT
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS

	data_t L1_input_fmap[L1_input_fmap_channel][L1_input_fmap_size][L1_input_fmap_size];
	memcpy(L1_input_fmap,(const data_t*)input_fmap,L1_input_fmap_size*L1_input_fmap_size*sizeof(data_t));
	/*����Conv1���*/
	L1_conv2d(L1_Kernel, L1_bias, L1_input_fmap, L1_output_fmap);
	/*��Conv1�����ReLu������������*/
	for (int i = 0; i < L1_output_fmap_channel; i++)
	{
		for (int j = 0; j < L1_output_fmap_size; j++)
		{
			for (int k = 0; k < L1_output_fmap_size; k++)
			{
				L1_output_fmap_ReLu[i][j][k] = Relu(L1_output_fmap[i][j][k]);
			}
		}
	}

	/*�������ػ����*/
	L2_maxpool2d(L1_output_fmap_ReLu, L2_output_fmap);
	/*����Conv3���*/
	L3_conv2d(L3_Kernel, L3_bias, L2_output_fmap, L3_output_fmap);
	/*��Conv3�����ReLu������������*/
	for (int i = 0; i < L3_output_fmap_channel; i++)
	{
		for (int j = 0; j < L3_output_fmap_size; j++)
		{
			for (int k = 0; k < L3_output_fmap_size; k++)
			{
				L3_output_fmap_ReLu[i][j][k] = Relu(L3_output_fmap[i][j][k]);
			}
		}
	}
	/*�������ػ����*/
	L4_maxpool2d(L3_output_fmap_ReLu, L4_output_fmap);
	/*�����ػ��������Flatten*/
	int flatten_i = 0;//flatten ��������
	for (int i = 0; i < L4_output_fmap_size; i++)
	{
		for (int j = 0; j < L4_output_fmap_size; j++)
		{
			for (int k = 0; k < L4_output_fmap_channel; k++)
			{
				L4_flatten_output_map[flatten_i] = L4_output_fmap[k][i][j];//(Ϊ�����denseȨ����״)��ͨ������չ��, ˳���һͨ����һ�е�һ��,�ڶ�ͨ����һ�е�һ��
				flatten_i++;
			}
		}
	}
	/*����Dense5���*/
	L5_dense(L5_neure_weights, L5_neure_bias, L4_flatten_output_map, L5_output_fmap);
	/*��Dense5�����ReLu������������*/
	for (int i = 0; i < L5_output_fmap_size; i++)
	{
		L5_output_fmap_ReLu[i] = Relu(L5_output_fmap[i]);
	}
	/*����Dense6���*/
	L6_dense(L6_neure_weights, L6_neure_bias, L5_output_fmap_ReLu, L6_output_fmap);
	/*���Dense6���*/
	memcpy(output_fmap,L6_output_fmap,L6_output_fmap_size*sizeof(data_t));
	/*����Dense6�������sigmoid����,��Ϊ��Ҫy=exp(x)����,fpga���ü���,���Է���arm�˽���*/

	return 0;

}
