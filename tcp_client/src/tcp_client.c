#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "tcp_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/
static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err);

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/***
 * �������� : Tcp_Client_Init();
 *
 * �������� : TCP��������ʼ��;
 *
 * ����ֵ	  : ��;
 *
 * ����ֵ   : ��;
 *
 **/
void Tcp_Client_Init(void)
{
		struct tcp_pcb *tcp_client_pcb;
		struct ip_addr ipaddr;
	
		/* ��Ŀ���������IPд��һ���ṹ�� */
		IP4_ADDR(&ipaddr, 192, 168, 0, 1);
	
		/* Ϊtcp�ͻ��˷���һ��tcp_pcb�ṹ��	*/
		tcp_client_pcb = tcp_new();
	
		/* �󶨱��ض˺ź�IP��ַ */
		tcp_bind(tcp_client_pcb, IP_ADDR_ANY, 80);
		
		if (tcp_client_pcb != NULL)
		{
				/* ��Ŀ��������������ӣ�����������Ŀ��˿ں�Ŀ��IP */
				tcp_connect(tcp_client_pcb, &ipaddr, 80, tcp_client_connected);
		}
}

/***
 * �������� : tcp_client_connected();
 *
 * �������� : lwip���ݽ��ջص�������������tcp���ӵ�ȷ�ϣ����ջص�����������;
 *
 * ����ֵ	  : *arg, *pcb, err ;
 *
 * ����ֵ   : ERR_OK �޴���;
 *
 **/
static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
		/* ȷ�ϼ��������� */
		tcp_arg(pcb, mem_calloc(sizeof(struct name), 1));
	
		/* ����һ���������ӵ��ʺ��ַ���*/
		tcp_write(pcb, "hello \n", strlen("hello \n"), 0);
	
		/* ���ý��ջص����� */
		tcp_recv(pcb, tcp_client_recv);
	
		return ERR_OK;
}

/***
 * �������� : tcp_client_recv();
 *
 * �������� : ���ܵ����ݺ󣬽����ݿ���ת����ȥ;
 *
 * ����ֵ	  : *arg, *pcb, *tcp_recv_pbuf, err;
 *
 * ����ֵ   : ERR_ARG �Ƿ��߼���ERR_OK�޴���;
 *
 **/
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
		struct pbuf *tcp_send_pbuf;
		struct name *name = (struct name *)arg;
	
		if (tcp_recv_pbuf != NULL)
		{
				/* �����շ����ݵĴ��� */
				tcp_recved(pcb, tcp_recv_pbuf->tot_len);
			
				if (!name)
				{
						pbuf_free(tcp_recv_pbuf);
						return ERR_ARG;
				}
				
				/* �����յ����ݿ��������ͽṹ�� */
				tcp_send_pbuf = tcp_recv_pbuf;
				
				/* �����յ���������ת����ȥ */
				tcp_write(pcb, tcp_send_pbuf->payload, tcp_send_pbuf->len, 1);
				/* ���� */
				tcp_write(pcb, "\r\n", strlen("\r\n"), 1);
				
				pbuf_free(tcp_recv_pbuf);
		}
		else if (err == ERR_OK)
		{
				/* �ͷ��ڴ� */
				mem_free(name);
				return tcp_close(pcb);
		}
		
		return ERR_OK;
}

