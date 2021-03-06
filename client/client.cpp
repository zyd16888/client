// client.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "conio.h"                                                      
#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 100
int Rvalue = 1;             //用于接收函数返回值
int connectReturn = -1;     //判定服务器是否连接成功
SOCKET sock;
char *ipinfo;



//函数前置声明
int cSever();
void talk();
void stop();
void menu();
void receivefile();

int main() {
	
	int n;
	menu();
	scanf("%d", &n);
	while (n)
	{
		switch (n)
		{
		case 1:
			if (!cSever())
				connectReturn = 0;
			//printf("测试用%d\n", connectReturn);      //测试用
			break;
		case 2:
			talk();
			break;
		case 3:
			receivefile();
			break;
		case 4:
			stop();
			break;
		case 5:
			system("cls");
			menu();
			break;
		case 6:
			exit(0);
			break;
		
		default:printf("输入错误！！请重新输入。\n");
		}
		menu();/*执行完功能再次显示菜单界面*/
		scanf("%d", &n);
	}

	return 0;
	
}

//连接服务器
int cSever() 
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//向服务器发起请求
	printf("连接服务器中、、、、、、、、\n\n\n");
	Sleep(2000);         //停顿2秒钟，造成请求服务器假象

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //创建套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sockAddr.sin_port = htons(5001);

	Rvalue = connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//连接服务器，接收返回值
	if (Rvalue != 0)
	{
		printf("连接服务器失败!!\n");
		return -1;
	}
	ipinfo = inet_ntoa(sockAddr.sin_addr);
	printf("成功连接到服务器···\n");
	return 0;
}

//信息交互
void  talk() 
{
	char bufSend[BUF_SIZE];
	char bufRecv[BUF_SIZE];
	int timeOut = 3;                //超时时长
	int strLen;                     //接收缓冲区的值
	if (!connectReturn)
	{
		system("cls");
		printf("开始和服务器对话\n", ipinfo);
		while (1)
		{
			//判断键盘是否有输入，有则进入发送模式
			if (kbhit())
			{

				printf("客户机:");
				gets_s(bufSend);
				send(sock, bufSend, strlen(bufSend) + 1, 0);
				if (!sock || !strcmp(bufSend, "bye") || !strcmp(bufSend, "再见") || !strcmp(bufSend, "quit"))
				{
					system("cls");
					menu();
					stop();
					break;
				}
			}

			//设置recv的超时时间，超时则返回，不等待
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(int));
			strLen = recv(sock, bufRecv, BUF_SIZE, 0);  //接收服务端的数据

			if (strLen > 0)
			{
				bufRecv[strLen] = '\0';
				printf("服务器:%s\n", bufRecv);
				if (!strcmp(bufRecv, "bye") || !strcmp(bufRecv, "再见") || !strcmp(bufRecv, "quit"))
				{
					system("cls");

					menu();
					stop();
					break;
				}
			}

		}

	}else
	{
		printf("还没有连接服务器....\n");
	}

}

//关闭连接
void stop()
{
	if (sock) closesocket(sock);
	::WSACleanup();         //终止 DLL 的使用
	printf("已关闭连接!!!\n");
}
//菜单
void menu()
{
	printf("\n\n");
	printf("------网络通信客户端------\n");
	printf("\t1、连接服务器\n");
	printf("\t2、开始和服务器的对话\n");
	printf("\t3、接收服务器的文件\n");
	printf("\t4、关闭连接\n");
	printf("\t5、返回主菜单\n");
	printf("\t6、退出系统\n");
	
	printf("\t请选择：");

}

//接收服务器的文件
void receivefile() 
{
	if (!connectReturn)
	{

		char filename[100] = { 0 };  //文件名
		printf("请输入要保存的文件名（如：save.txt）: ");
		scanf("%s", &filename);
		FILE *fp = fopen(filename, "wb");  //以二进制方式打开（创建）文件
		if (fp == NULL) {
			printf("文件创建失败，按任意键返回主菜单··\n");
			getch();
		}
		else
		{

			char buffer[BUF_SIZE] = { 0 };  //文件缓冲区
			int nCount;
			//循环接收数据，直到文件传输完毕
			while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0) {
				fwrite(buffer, nCount, 1, fp);
			}
			puts("文件接收成功!");

			fclose(fp);
		}
	}
	else
	{
		printf("尚未连接到服务器···\n");
	}
}
