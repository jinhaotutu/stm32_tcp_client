#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "tcp_server.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/
static err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/***
 * 函数名称 : Tcp_Server_Init();
 *
 * 函数描述 : TCP服务器初始化;
 *
 * 传递值	  : 无;
 *
 * 返回值   : 无;
 *
 **/
void Tcp_Server_Init(void)
{
		struct tcp_pcb *tcp_server_pcb;
		
		/* 为tcp服务器分配一个tcp_pcb结构体	*/
		tcp_server_pcb = tcp_new();
	
		/* 绑定本地端号和IP地址 */
		tcp_bind(tcp_server_pcb, IP_ADDR_ANY, 80);
	
		/* 监听之前创建的结构体tcp_server_pcb */
		tcp_server_pcb = tcp_listen(tcp_server_pcb);
	
		/* 初始化结构体接收回调函数 */
		tcp_accept(tcp_server_pcb, tcp_server_accept);
}

/***
 * 函数名称 : tcp_server_accept();
 *
 * 函数描述 : lwip数据接收回调函数，包含对tcp连接的确认，接收回调函数的配置;
 *
 * 传递值	  : *arg, *pcb, err ;
 *
 * 返回值   : ERR_OK 无错误;
 *
 **/
static err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
		/* 确认监听与连接 */
	  tcp_arg(pcb, mem_calloc(sizeof(struct name), 1));
	
		/* 发送一个建立连接的字符串 */
		tcp_write(pcb, "hello my dream \n\r",strlen("hello my dream \n\r  "), 1);
	
		/* 配置接收回调函数 */
		tcp_recv(pcb, tcp_server_recv);
	
		return ERR_OK;
}

/***
 * 函数名称 : tcp_server_recv();
 *
 * 函数描述 : 接受到数据后，将数据拷贝转发出去;
 *
 * 传递值	  : *arg, *pcb, *tcp_recv_pbuf, err;
 *
 * 返回值   : ERR_ARG 非法逻辑，ERR_OK无错误;
 *
 **/
static err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *tcp_recv_pbuf, err_t err)
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
				
				/* 换行 */
				tcp_write(pcb, "\r\n", strlen("\r\n"), 1);
				/* 将接收到的数据再转发出去 */
				tcp_write(pcb, tcp_send_pbuf->payload, tcp_send_pbuf->len, 1);
				
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
