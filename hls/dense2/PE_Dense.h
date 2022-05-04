#ifndef _PE_DENSE_H_
#define _PE_DENSE_H_
#include <ap_int.h>
#include <ap_fixed.h>

typedef ap_fixed<16,8,AP_RND,AP_SAT> data_t;//�����������
typedef short reg_t;//���ƼĴ�������

/*PE�ṹ����*/
#define MAX_PE_Register_NUM 6 //������ƼĴ���Ԫ�ظ���
#define MAX_PE_Kernel_size 3 //����������˳ߴ�
#define MAX_PE_Kernel_channel 1024 //�����������ͨ��
#define MAX_PE_Kernel_number 1024 //���������������
#define MAX_PE_Bias_number 1024 //�����������ƫ��
#define MAX_PE_Input_fmap_channel 1024 //�����������ͼͨ��
#define MAX_PE_Input_fmap_size 320 //�����������ͼ�ߴ�
#define MAX_PE_Outputput_fmap_channel 1024 //����������ͼͨ��
#define MAX_PE_Output_fmap_size 320 //����������ͼ�ߴ�

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
);

#endif
