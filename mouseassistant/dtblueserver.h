#pragma once

#include "comthread.h"
#include <ws2bth.h>
#include "rawclient.h"
#include <list>
#include <string>

#pragma pack(4)

typedef struct
{
	char aCompany[2]; //公司标识
	short iLenght; //数据长度
	short iType;    //类型
	short iReserved;
}DTBlueHeader;

typedef struct
{
	char left;
	char mid;
	char right;
	char resv1;
	long x;
	long y;
	long wheel;
	long resv2;
	long resv3;
}DTMouseData;

typedef struct
{
	unsigned char  aESA[16];
	unsigned char  aVersion[4];
	unsigned char  aReserved[12];
}DTBlueVerifer;

#pragma pack()

#define   CN_BLUE_MAXLEN   512    //允许最长蓝牙数据包
typedef struct
{
	short iMode;    //接收阶段，0=接收头，=1接送数据
	short iRemain;  // 未接收数据长度
	int   iRecvStat; //统计计数
	DTBlueHeader tHdr; //接收头缓存
	char  *revChar;  //接收数据缓存
}DTBlueReceive;

class dtblueserver
{
public:
	dtblueserver();
	~dtblueserver();

	bool Start();

	int RunAccept();
	int RunWork(SOCKET ClientSocket, SOCKADDR_BTH *pLocalAddr, SOCKADDR_BTH *pPeerAddr);

	int ReceiveBlueData(SOCKET hSkt);
	int CodeVerifer(SOCKADDR_BTH *pLocalAddr, unsigned char output[16]);
	int DecodeVerifer(SOCKADDR_BTH *pPeerAddr, unsigned char *input);
	int Stop();

	static CM_THREAD_RET CM_THREAD_CALL_CONV threadMain(CM_THREAD_ARG pArg);
private:
	bool m_bRuning;

	CM_THREAD_HANDLE m_threadHandle;
	CM_THREAD_ID     m_threadId;

	DTBlueReceive    m_BlueMng;
	SOCKET           m_LocalSocket;

	HANDLE     m_hfRaw;
	

	static bool      m_bIniWsa;

public:
	//调试日志，临时用
	int        m_logFlag;
	DWORD      m_iOpenFileCode;
};
