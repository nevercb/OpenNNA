#ifndef _PROCESS_ELEMENT_H_
#define _PROCESS_ELEMENT_H_

#include <ap_int.h>
#include <ap_fixed.h>

typedef ap_fixed<16,8,AP_RND,AP_SAT> data_t;

/*L1 Conv2d*/
#define L1_Kernel_size 3 //����˴�С
#define L1_Kernel_channel 1 //�����ͨ��
#define L1_Kernel_number 16 //���������
#define L1_bias_number 16  //ƫ������
#define L1_input_fmap_size 32 //��������ͼ��С
#define L1_input_fmap_channel 1 //��������ͼͨ��
#define L1_output_fmap_size (((L1_input_fmap_size+2*L1_conv_padding-L1_Kernel_size)/ L1_conv_stride)+1)+L1_output_padding //�������ͼ����+���Padding
#define L1_output_fmap_channel 16 //�������ͼͨ��
#define L1_conv_padding 0//���ǰ���,�ڱ�Ե�����nȦ0(δʵ��)
#define L1_conv_stride 1//����˲���
#define L1_output_padding 0//���padding //����Ҫ�޸ľ���˵�һ����Ŀ���������

/*L2 Maxpool2D*/
#define L2_MaxpoolKernel_size 2 //�ػ��˴�С
#define L2_MaxpoolKernel_stride 2//�ػ��˲���
#define L2_input_fmap_size 30 //��������ͼ��С
#define L2_input_fmap_channel 16 //��������ͼͨ��
#define L2_output_fmap_size (((L2_input_fmap_size-L2_MaxpoolKernel_size)/ L2_MaxpoolKernel_stride)+1) //�������ͼ����
#define L2_output_fmap_channel 16 //�������ͼͨ��

/*L3 Conv2D*/
#define L3_Kernel_size 3 //����˴�С
#define L3_Kernel_channel 16 //�����ͨ��
#define L3_Kernel_number 32 //���������
#define L3_bias_number 32  //ƫ������
#define L3_input_fmap_size 15 //��������ͼ��С
#define L3_input_fmap_channel 16 //��������ͼͨ��
#define L3_output_fmap_size (((L3_input_fmap_size+2*L3_conv_padding-L3_Kernel_size)/ L3_conv_stride)+1)+L3_output_padding //�������ͼ����+���Padding
#define L3_output_fmap_channel 32 //�������ͼͨ��
#define L3_conv_padding 0//���ǰ���,�ڱ�Ե�����nȦ0(δʵ��)
#define L3_conv_stride 1//����˲���
#define L3_output_padding 0//���padding //����Ҫ�޸ľ���˵�һ����Ŀ���������

/*L4 Maxpool2D*/
#define L4_MaxpoolKernel_size 2 //�ػ��˴�С
#define L4_MaxpoolKernel_stride 2//�ػ��˲���
#define L4_input_fmap_size 13 //��������ͼ��С
#define L4_input_fmap_channel 32 //��������ͼͨ��
#define L4_output_fmap_size (((L4_input_fmap_size-L4_MaxpoolKernel_size)/ L4_MaxpoolKernel_stride)+1) //�������ͼ����
#define L4_output_fmap_channel 32 //�������ͼͨ��

/*L5 Dense*/
#define L5_input_fmap_size 1152 //ȫ���Ӳ���������ͼ��С
#define L5_neure_number 64 //��Ԫ����
#define L5_output_fmap_size 64 //�������ͼ��С

/*L6 Dense*/
#define L6_input_fmap_size  64 //ȫ���Ӳ���������ͼ��С
#define L6_neure_number  10 //��Ԫ����
#define L6_output_fmap_size  10 //�������ͼ��С

#include "conv1_weights_bias.h"
#include "conv3_weights_bias.h"
#include "dense5_weights_bias.h"
#include "dense6_weights_bias.h"
int process_element(
		data_t *input_fmap[L1_input_fmap_size*L1_input_fmap_size],
		data_t *output_fmap[L6_output_fmap_size]
					);

#endif
