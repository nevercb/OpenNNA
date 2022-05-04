#ifndef _PE_CONV_H_
#define _PE_CONV_H_
#include <ap_int.h>
#include <ap_fixed.h>

typedef ap_fixed<16,8,AP_RND,AP_SAT> data_t;//�����������
typedef short reg_t;//���ƼĴ�������

/*PE�ṹ����*/
#define MAX_PE_Register_NUM 13 //������ƼĴ���Ԫ�ظ���
#define MAX_PE_Kernel_size 3 //����������˳ߴ�
#define MAX_PE_Kernel_channel 1024 //�����������ͨ��
#define MAX_PE_Kernel_number 1024 //���������������
#define MAX_PE_Bias_number 1024 //�����������ƫ��
#define MAX_PE_Input_fmap_channel 1024 //�����������ͼͨ��
#define MAX_PE_Input_fmap_size 320 //�����������ͼ�ߴ�
#define MAX_PE_Outputput_fmap_channel 1024 //����������ͼͨ��
#define MAX_PE_Output_fmap_size 320 //����������ͼ�ߴ�

#define MAX_BUFFER_Input_fmap_size 3 //ÿ���������Bufferһ�����ֻ����MAX_Kernel_size*MAX_Kernel_size*MAX_Input_fmap_channel(Ҳ����һ������Bufferֻ��ȡ����˸��ǵĵط�
#define MAX_BUFFER_Output_fmap_size 1 //�����ÿ����һ������һͨ��fmap�ϵ�һ����
#define MAX_BUFFER_Bias_number 1 //ÿ��bufferֻ��һ�������,һ�������ֻ��һ��bias

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
);

#endif
