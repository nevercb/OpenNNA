#include "PE_Padding.h"

void PE_padding(
	/*PE���ƼĴ���*/
	reg_t* PE_Register,
	/*������������ͼ*/
	data_t* PE_Input_fmap,
	/*PE�������ͼ*/
	data_t* PE_Output_fmap
)
{
#pragma HLS INTERFACE m_axi bundle=INPUT_Reg depth=5 port=PE_Register offset=slave
#pragma HLS INTERFACE m_axi bundle=INPUT_Fmap depth=104857600 port=PE_Input_fmap offset=slave
#pragma HLS INTERFACE m_axi bundle=OUTPUT_Fmap depth=104857600 port=PE_Output_fmap offset=slave
#pragma HLS INTERFACE s_axilite bundle=CTRL_BUS port=return

	for (int i = 0; i < PE_Register[1]; i++)//input_fmap_channel
	{
		for (int j = 0; j < PE_Register[2]; j++)//output_fmap_size ��
		{
			for (int k = 0; k < PE_Register[2]; k++)//output_fmap_size ��
			{
				/*j������,�����͵ײ����*/
				if ( j < PE_Register[4])//����padding
				{
					PE_Output_fmap[(i * PE_Register[2] * PE_Register[2]) + (j * PE_Register[2]) + k] = 0;
				}
				else if (j >= (PE_Register[4] + PE_Register[0]))//�ײ�padding
				{
					PE_Output_fmap[(i * PE_Register[2] * PE_Register[2]) + (j * PE_Register[2]) + k] = 0;
				}
				/*k������,��ߺ��ұ����*/
				if (k < PE_Register[4])//���padding
				{
					PE_Output_fmap[(i * PE_Register[2] * PE_Register[2]) + (j * PE_Register[2]) + k] = 0;
				}
				else if (k >= (PE_Register[4] + PE_Register[0]))//�ұ�padding
				{
					PE_Output_fmap[(i * PE_Register[2] * PE_Register[2]) + (j * PE_Register[2]) + k] = 0;
				}
				//����fmap��䵽�м�
				if(j >= PE_Register[4] && j < (PE_Register[4] + PE_Register[0]) && k >= PE_Register[4] && k < (PE_Register[4] + PE_Register[0]))
					PE_Output_fmap[(i * PE_Register[2] * PE_Register[2]) + (j * PE_Register[2]) + k] = PE_Input_fmap[(i * PE_Register[0] * PE_Register[0]) + ((j-PE_Register[4]) * PE_Register[0]) + (k-PE_Register[4])];
			}
		}
	}
}
