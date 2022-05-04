#include "PE_Dense.h"


void PE_dense(
	/*PE���ƼĴ���*/
	reg_t* PE_Register,
	/*PEȫ������ԪȨ��*/
	data_t* PE_Neure_weights,
	/*PEȫ������Ԫƫ��*/
	data_t* PE_Neure_bias,
	/*����ȫ����PE������ͼ*/
	data_t* PE_Input_fmap,
	/*ȫ����PE�������ͼ*/
	data_t* PE_Output_fmap
)
{
#pragma HLS INTERFACE m_axi bundle=INPUT_Reg depth=6 port=PE_Register offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Fmap depth=104857600 port=PE_Input_fmap offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Neure_weights depth=307200 port=PE_Neure_weights offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUTNeure_bias depth=1024 port=PE_Neure_bias offset=slave
#pragma HLS INTERFACE m_axi bundle=OUTPUT_Fmap depth=104857600 port=PE_Output_fmap offset=slave
#pragma HLS INTERFACE s_axilite bundle=CTRL_BUS port=return
	//int input_fmap_size; //����һά����ͼ��С
	//int weights_bias_number;//Ȩ�غ�ƫ�ü���(һ��neura->1 weights + 1 bias)
	for (int weights_bias_number = 0; weights_bias_number < PE_Register[4]; weights_bias_number++)//neure_number
	{
		/*+bias*/
		/*�ǳ���Ҫ:��ռ����м��� ����0,0,0λ�õ���Ȩ��,���ڶ�μ�����˵,�������һ�εļ�����,����Ӱ�챾�μ���*/
		PE_Output_fmap[weights_bias_number] = PE_Neure_bias[weights_bias_number];
		/**weights*/
		//����ȫ����������˵,����һά��,����input_fmap_size=input_fmap_size*input_fmap_size*input_fmap_channel
		//����һ��˳�㽫��Ψ����ѹ��һά������
		for (int input_fmap_size = 0; input_fmap_size < (PE_Register[0]*PE_Register[0]* PE_Register[1]); input_fmap_size++)//����ȫ����������˵,����һά��,����input_fmap_size=input_fmap_size*input_fmap_size*input_fmap_channel
		{
			/*+bias*/
			/*�ǳ���Ҫ:��ռ����м��� ����0,0,0λ�õ���Ȩ��,���ڶ�μ�����˵,�������һ�εļ�����,����Ӱ�챾�μ���*/
			//if (input_fmap_size == 0)
			//	PE_Output_fmap[weights_bias_number] = PE_Neure_bias[weights_bias_number];
			PE_Output_fmap[weights_bias_number] += (PE_Input_fmap[input_fmap_size] * PE_Neure_weights[(weights_bias_number*(PE_Register[0] * PE_Register[0] * PE_Register[1]))+input_fmap_size]); //+ neure_bias[weights_bias_number];

		}
	}
}
