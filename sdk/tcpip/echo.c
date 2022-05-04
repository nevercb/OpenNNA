/*
 * Copyright (C) 2009 - 2018 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

/****************������������******************/
#include "xprocess_element_hw.h"
#include "xprocess_element.h"
#include "xil_cache.h"
#include "xtime_l.h"
typedef short data_t;//ip������λ��
typedef float data_packet;//���ݰ�����
#define L1_input_fmap_size 32 //��������ͼ��С
#define L1_input_fmap_channel 1 //��������ͼͨ��
#define L6_output_fmap_size  10 //�������ͼ��С

#define Receive_total_bytes_num 4096//�������ܽ���
#define Receive_packet_bytes_num 1024//ÿ�����ݰ�������
#define Packet_num Receive_total_bytes_num/Receive_packet_bytes_num //���ݰ�����=�ܽ����ֽ���/ÿ�����ݰ����ֽ���
#define Receive_data_num Receive_total_bytes_num/sizeof(data_packet) //������(Ҳ�����ж��ٸ�data_packet���͵�����)= �������ܽ���(һ��ͼƬ�����ֽ���)/ÿ��data_packet���͵��ֽ���
#define fixed_point 256//��������8λС��
int packet_num = 0;//���ݰ����� float 4bytes ������4bytes���ݰ�
int packet_receive_sucess_flag = 0;//�ɹ�����4bytes���ݰ���־
struct tcp_pcb *c_pcb;
char rxbuffer[Receive_total_bytes_num] ={0};//Rxbuffer����Ϊȫ�ֱ���,��Ϊÿ��tcp����1kb���ݰ� ����Ҫ������һ�����ݰ�������
int transfer_data() {
	return 0;
}

void print_app_header()
{
#if (LWIP_IPV6==0)
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
#else
	xil_printf("\n\r\n\r-----lwIPv6 TCP echo server ------\n\r");
#endif
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}
//���������
int neural_network_calculate(data_packet input_fmap[Receive_data_num])
{
	float max_value;
	int max_value_sn;
	Xil_DCacheDisable();
	data_t *src=(data_t*)malloc(L1_input_fmap_channel*L1_input_fmap_size*L1_input_fmap_size*sizeof(data_t));
	data_t *dest=(data_t*)malloc(L6_output_fmap_size*sizeof(data_t));
	XProcess_element  HlsXProcess_element;
	XProcess_element_Config  *ExamplePtr;
	ExamplePtr = XProcess_element_LookupConfig(XPAR_PROCESS_ELEMENT_0_DEVICE_ID);
	if (!ExamplePtr)
	{
		xil_printf("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	//xil_printf("Initialize the Device\n");
	long status = XProcess_element_CfgInitialize(&HlsXProcess_element, ExamplePtr);
	if (status != XST_SUCCESS)
	{
		xil_printf("ERROR: Could not initialize accelerator.\n\r");
		//return(-1);
	}
	//XTime_GetTime(&tCur);
	for (int i = 0; i < Receive_data_num; i++)
	{
		src[i] = (data_t)(input_fmap[i]*fixed_point);
		//xil_printf("{%d}-src:%d\n",i,src[i]);
	}
	//xil_printf("\n***********************************\n");
	/*cache����Ҫ���� A9 32bit ����Ҫ4�ֽڶ���*/
	//Xil_DCacheFlushRange((u32)src,L1_input_fmap_channel*L1_input_fmap_size*L1_input_fmap_size*sizeof(data_t));
	XProcess_element_Set_input_fmap(&HlsXProcess_element,(u32)src);
	XProcess_element_Set_output_fmap(&HlsXProcess_element,(u32)dest);
	XProcess_element_Start(&HlsXProcess_element);
	while((XProcess_element_IsDone(&HlsXProcess_element)) == 0);
	/*���10��short 20�ֽ�,��Ҫ��12�ֽ�,����32�ֽ�*/
	//Xil_DCacheInvalidateRange((u32)dest,sizeof(data_t)*L6_output_fmap_size+12);
	//XTime_GetTime(&tEnd);
	//tUsed=((tEnd-tCur)*1000000)/(COUNTS_PER_SECOND);
	//printf("\ntime used is %d us|FPS(%d)\n Result:\n",(int)tUsed,(int)1000000/tUsed);
	max_value = (float)dest[0]/fixed_point;
	max_value_sn = 0; //�ӵ�0�����ݿ�ʼ
	//xil_printf("%d,",max_value);
	for(int i=0;i<10;i++){
		if((float)dest[i]/fixed_point>max_value)
		{
			max_value = (float)dest[i]/fixed_point;
			max_value_sn = i;
		}
		//xil_printf("[%d]%d,",i,dest[i]);//������
		//printf("[%d]%f,",i,(float)dest[i]/256);
	}
	//xil_printf("max_value_sn:%d",max_value_sn);
	free(src);
	free(dest);
	return max_value_sn;
}
//��ͻ��˻�����Ϣ
void sent_msg(const char *msg)
{
    err_t err;
    tcp_nagle_disable(c_pcb);
    if (tcp_sndbuf(c_pcb) > strlen(msg)) {
        err = tcp_write(c_pcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK)
            xil_printf("tcp_server: Error on tcp_write: %d\r\n", err);
        err = tcp_output(c_pcb);
        if (err != ERR_OK)
            xil_printf("tcp_server: Error on tcp_output: %d\r\n", err);
    } else
        xil_printf("no space in tcp_sndbuf\r\n");
}
err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{
	int result;
	char result_bytes;
	char receive_char  = 's';
	struct pbuf *q;
	data_packet array_data[Receive_data_num] ={0};//�ֲ�����
	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		xil_printf("tcp connection closed\r\n");
		return ERR_OK;
	}
	q = p;
	//xil_printf("%d\n\r",q->tot_len);//���յ��ܳ���
	//xil_printf("%d\n\r",q->len);//���ݰ�����
	if (q->tot_len == Receive_packet_bytes_num) {
	memcpy(&rxbuffer[packet_num*Receive_packet_bytes_num], q->payload, q->len);
	packet_num++;//�Ѿ����յ������ݰ�+1
	if(packet_num == Packet_num)//���յ������е����ݰ�
	{
		packet_num = 0;//Clear
		packet_receive_sucess_flag = 1;//�ɹ�����4 bytes���ݰ�
	}
	//total_bytes += q->len;
	//q = q->next;
	}
	if(packet_receive_sucess_flag == 0)//���ݰ����ջ�δ���
	{
		//����Ӧ��
		tcp_nagle_disable(tpcb);
		tcp_write(tpcb,&receive_char,1,TCP_WRITE_FLAG_COPY);
		err = tcp_output(tpcb);//����
		tcp_recved(tpcb, p->tot_len);//����Ϊ�ѽ���,�������ܴ���,ӭ����һ�ν���
	}
	else//�������
	{
		packet_receive_sucess_flag = 0;//��־clear
		/*4bytes��ԭ��float32*/
		//for(int i = 0;i<Receive_total_bytes_num;i=i+1)
		//{
		//	xil_printf("{%d}-buffer:%d\n",i,rxbuffer[i]);
		//}
		/*4bytes��ԭ��float32*/
		for(int i = 0;i<Receive_total_bytes_num;i=i+4)
		{
			memcpy(&array_data[i/4], &rxbuffer[i], 4);
		}
		result = neural_network_calculate(array_data);
		//xil_printf("---|||Result:%d|||---\n",result);
		result_bytes = (char)result;
		tcp_nagle_disable(tpcb);
		tcp_write(tpcb,&result_bytes,1,TCP_WRITE_FLAG_COPY);
		err = tcp_output(tpcb);//����
		tcp_recved(tpcb, p->tot_len);//����Ϊ�ѽ���,�������ܴ���,ӭ����һ�ν���
	}
	/*4bytes��ԭ��float*/
	/*��ӡfloat32��ԭ���*/
	//for(int i = 0;i<4;i=i+1)
	//{
		//memcpy(&array_data[i/4], &rxbuffer[i], 4);
	//	printf("[%d]",rxbuffer[i]);
	//}
	//xil_printf("{{{{{%x\n\r",array_data[0]);
	//while(1);
	//for(int i = 0;i<Receive_data_num;i=i+1)
	//{
		//xil_printf("%f\n\r",array_data[i]);//���ݰ�����
	//}
	//xil_printf("{%d}-buffer:%d\n",i,rxbuffer[i]);
	//result = neural_network_calculate(array_data);
	//xil_printf("---|||Result:%d|||---\n",result);
	//result_bytes = (char)result;
	//tcp_nagle_disable(tpcb);
	//tcp_write(tpcb,&result_bytes,1,TCP_WRITE_FLAG_COPY);
	//err = tcp_output(tpcb);//����
	//tcp_recved(tpcb, p->tot_len);//����Ϊ�ѽ���,�������ܴ���,ӭ����һ�ν���
	pbuf_free(p);
	pbuf_free(q);
	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;



	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_application()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}
