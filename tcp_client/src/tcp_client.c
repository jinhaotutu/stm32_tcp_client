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
 * 函数名称 : Tcp_Client_Init();
 *
 * 函数描述 : TCP服务器初始化;
 *
 * 传递值	  : 无;
 *
 * 返回值   : 无;
 *
 **/
void Tcp_Client_Init(void)
{
		struct tcp_pcb *tcp_client_pcb;
		struct ip_addr ipaddr;
	
		/* 将目标服务器的IP写入一个结构体 */
		IP4_ADDR(&ipaddr, 192, 168, 0, 1);
	
		/* 为tcp客户端分配一个tcp_pcb结构体	*/
		tcp_client_pcb = tcp_new();
	
		/* 绑定本地端号和IP地址 */
		tcp_bind(tcp_client_pcb, IP_ADDR_ANY, 80);
		
		if (tcp_client_pcb != NULL)
		{
				/* 与目标服务器进行连接，参数包括了目标端口和目标IP */
				tcp_connect(tcp_client_pcb, &ipaddr, 80, tcp_client_connected);
		}
}

/***
 * 函数名称 : tcp_client_connected();
 *
 * 函数描述 : lwip数据接收回调函数，包含对tcp连接的确认，接收回调函数的配置;
 *
 * 传递值	  : *arg, *pcb, err ;
 *
 * 返回值   : ERR_OK 无错误;
 *
 **/
static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
		/* 确认监听与连接 */
		tcp_arg(pcb, mem_calloc(sizeof(struct name), 1));
	
		/* 发送一个建立连接的问候字符串*/
		tcp_write(pcb, "hello \n", strlen("hello \n"), 0);
	
		/* 配置接收回调函数 */
		tcp_recv(pcb, tcp_client_recv);
	
		return ERR_OK;
}

/***
 * 函数名称 : tcp_client_recv();
 *
 * 函数描述 : 接受到数据后，将数据拷贝转发出去;
 *
 * 传递值	  : *arg, *pcb, *tcp_recv_pbuf, err;
 *
 * 返回值   : ERR_ARG 非法逻辑，ERR_OK无错误;
 *
 **/
static err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
{
		struct pbuf *tcp_send_pbuf;
		struct name *name = (struct name *)arg;
	
		if (tcp_recv_pbuf != NULL)
		{
				/* 扩大收发数据的窗口 */
				tcp_recved(pcb, tcp_recv_pbuf->tot_len);
			
				if (!name)
				{
						pbuf_free(tcp_recv_pbuf);
						return ERR_ARG;
				}
				
				/* 将接收的数据拷贝给发送结构体 */
				tcp_send_pbuf = tcp_recv_pbuf;
				
				/* 将接收到的数据再转发出去 */
				tcp_write(pcb, tcp_send_pbuf->payload, tcp_send_pbuf->len, 1);
				/* 换行 */
				tcp_write(pcb, "\r\n", strlen("\r\n"), 1);
				
				pbuf_free(tcp_recv_pbuf);
		}
		else if (err == ERR_OK)
		{
				/* 释放内存 */
				mem_free(name);
				return tcp_close(pcb);
		}
		
		return ERR_OK;
}

