#include "dtblueserver.h"
#include <qdebug.h>

#include <initguid.h>
#include <winsock2.h>

#include <strsafe.h>
#include <intsafe.h>
#include "comsm4.h"

// {B62C4E8D-62CC-404b-BBBF-BF3E3BBB1374}
DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);
#define CXN_INSTANCE_STRING "DT MOUSE SERVER"


#ifdef CXN_INSTANCE_STRING
//#ifdef DEBUG
#include "atlbase.h"
#include "atlstr.h"
void OutputDebugPrintf(const char* strOutputString, ...)
{
	char strBuffer[4096] = { 0 };
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	OutputDebugString((strBuffer));
}
#else
#define OutputDebugPrintf
#endif

bool dtblueserver::m_bIniWsa = false;
dtblueserver::dtblueserver()
{
	m_threadHandle=NULL;
	m_threadId = NULL;

	m_bRuning = true;
	m_LocalSocket = INVALID_SOCKET;

	m_BlueMng.iMode = 0;
	m_BlueMng.iRecvStat = 0;
	m_BlueMng.iRemain = sizeof(DTBlueHeader);
	m_BlueMng.revChar = new char[CN_BLUE_MAXLEN];

	m_iOpenFileCode = 0;
	m_hfRaw = 0;

	
}


dtblueserver::~dtblueserver()
{
	delete[]m_BlueMng.revChar;
}

// ============================================================================
// 函数功能：启动蓝牙服务线程
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
bool dtblueserver::Start()
{
	OutputDebugPrintf("long=%d int=%d DTBlueHeader=%d,DTMouseData=%d, DTBlueVerifer=%d \n", sizeof(long), sizeof(int)
		, sizeof(DTBlueHeader), sizeof(DTMouseData), sizeof(DTBlueVerifer));
	m_logFlag = 0;

	if (!m_bIniWsa)
	{ //初始化window wsa 
		WSADATA     tWSAData = { 0 };
		int ulRetCode = WSAStartup(MAKEWORD(2, 2), &tWSAData);
		if (CD_SUCCESS != ulRetCode) {
			m_bIniWsa = false;
			return false;
		}
		else
		{
			m_bIniWsa = true;
		}
	}
	//打开驱动
	PSP_DEVICE_INTERFACE_DETAIL_DATA    IfDetailData;
	//IfDetailData = SearchRawPath((LPGUID)&GUID_DEVINTERFACE_MOUSE);
	IfDetailData = SearchRawPath((LPGUID)&GUID_DEVINTERFACE_DTMOUSE);
	if (IfDetailData)
	{
		m_hfRaw = CreateFile(IfDetailData->DevicePath,
			GENERIC_READ | GENERIC_WRITE, 0, NULL, // no SECURITY_ATTRIBUTES structure
			OPEN_EXISTING, // No special create flags
			0, // No special attributes
			NULL);

		m_logFlag |= 0x0001;

		if (INVALID_HANDLE_VALUE == m_hfRaw) {
			m_iOpenFileCode = GetLastError();
		}

		free(IfDetailData);  //一定释放
	}


	//启动蓝牙服务线程
	m_bRuning = true;
	Thread_Start(dtblueserver::threadMain,
		this,&m_threadHandle,&m_threadId,204800,
		"dtblueserver::threadMain");
	
	return 0;
}
// ============================================================================
// 函数功能：结束当前服务
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
int dtblueserver::Stop()
{
	m_bRuning = false;

	if (INVALID_SOCKET != m_LocalSocket) {
		closesocket(m_LocalSocket);
		m_LocalSocket = INVALID_SOCKET;
	}

	if (m_hfRaw)
	{
		CloseHandle(m_hfRaw);

		m_iOpenFileCode = 0;
		m_hfRaw = 0;
	}
	return 0;
}

// ============================================================================
// 函数功能：服务线程
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
CM_THREAD_RET CM_THREAD_CALL_CONV dtblueserver::threadMain(CM_THREAD_ARG pArg)
{
	dtblueserver *pServer = (dtblueserver *)pArg;
	pServer->RunAccept();

	return CM_THREAD_RET_VAL;
}
// ============================================================================
// 函数功能：初始化socket，侦听连接
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
int dtblueserver::RunAccept()
{
	ULONG           ulRetCode = CD_SUCCESS;
	int             iAddrLen = sizeof(SOCKADDR_BTH);
	int             iCxnCount = 0;
	size_t          cbInstanceNameSize = 0;
	char *       pszInstanceName = NULL;
	char         szThisComputerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD           dwLenComputerName = MAX_COMPUTERNAME_LENGTH + 1;
	SOCKET          LocalSocket = INVALID_SOCKET;
	SOCKET          ClientSocket = INVALID_SOCKET;
	WSAQUERYSET     wsaQuerySet = { 0 };
	SOCKADDR_BTH    SockAddrBthLocal = { 0 };
	LPCSADDR_INFO   lpCSAddrInfo = NULL;
	HRESULT         res;

	//
	// This fixed-size allocation can be on the stack assuming the
	// total doesn't cause a stack overflow (depends on your compiler settings)
	// However, they are shown here as dynamic to allow for easier expansion
	//
	lpCSAddrInfo = (LPCSADDR_INFO)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY, sizeof(CSADDR_INFO));
	if (NULL == lpCSAddrInfo) {
		OutputDebugPrintf("!ERROR! | Unable to allocate memory for CSADDR_INFO\n");
		ulRetCode = CD_FAILURE;
	}

	if (CD_SUCCESS == ulRetCode) {

		if (!GetComputerName(szThisComputerName, &dwLenComputerName)) {
			OutputDebugPrintf("=CRITICAL= | GetComputerName() call failed. WSAGetLastError=[%d]\n", WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	//
	// Open a bluetooth socket using RFCOMM protocol
	//
	if (CD_SUCCESS == ulRetCode) {
		LocalSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (INVALID_SOCKET == LocalSocket) {
			OutputDebugPrintf("=CRITICAL= | socket() call failed. WSAGetLastError = [%d]\n", WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {

		//
		// Setting address family to AF_BTH indicates winsock2 to use Bluetooth port
		//
		SockAddrBthLocal.addressFamily = AF_BTH;
		SockAddrBthLocal.port = BT_PORT_ANY;
		SockAddrBthLocal.serviceClassId = g_guidServiceClass;
		//
		// bind() associates a local address and port combination
		// with the socket just created. This is most useful when
		// the application is a server that has a well-known port
		// that clients know about in advance.
		//
		if (SOCKET_ERROR == bind(LocalSocket,(struct sockaddr *) &SockAddrBthLocal,sizeof(SOCKADDR_BTH))) {
			OutputDebugPrintf("=CRITICAL= | bind() call failed w/socket = [0x%I64X]. WSAGetLastError=[%d]\n", (ULONG64)LocalSocket, WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {
		ulRetCode = getsockname(LocalSocket,(struct sockaddr *)&SockAddrBthLocal,&iAddrLen);
		if (SOCKET_ERROR == ulRetCode) {
			OutputDebugPrintf("=CRITICAL= | getsockname() call failed w/socket = [0x%I64X]. WSAGetLastError=[%d]\n", (ULONG64)LocalSocket, WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {
		// CSADDR_INFO
		lpCSAddrInfo[0].LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
		lpCSAddrInfo[0].LocalAddr.lpSockaddr = (LPSOCKADDR)&SockAddrBthLocal;
		lpCSAddrInfo[0].RemoteAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
		lpCSAddrInfo[0].RemoteAddr.lpSockaddr = (LPSOCKADDR)&SockAddrBthLocal;
		lpCSAddrInfo[0].iSocketType = SOCK_STREAM;
		lpCSAddrInfo[0].iProtocol = BTHPROTO_RFCOMM;

		//
		// If we got an address, go ahead and advertise it.
		//
		ZeroMemory(&wsaQuerySet, sizeof(WSAQUERYSET));
		wsaQuerySet.dwSize = sizeof(WSAQUERYSET);
		wsaQuerySet.lpServiceClassId = (LPGUID)&g_guidServiceClass;

		//
		// Adding a byte to the size to account for the space in the
		// format string in the swprintf call. This will have to change if converted
		// to UNICODE
		//
		res = StringCchLength(szThisComputerName, sizeof(szThisComputerName), &cbInstanceNameSize);
		if (FAILED(res)) {
			OutputDebugPrintf("-FATAL- | ComputerName specified is too large\n");
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {
		cbInstanceNameSize += sizeof(CXN_INSTANCE_STRING) + 1;
		pszInstanceName = (LPSTR)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,cbInstanceNameSize);
		if (NULL == pszInstanceName) {
			OutputDebugPrintf("-FATAL- | HeapAlloc failed | out of memory | gle = [%d] \n", GetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {
		StringCbPrintf(pszInstanceName, cbInstanceNameSize, "%s %s", szThisComputerName, CXN_INSTANCE_STRING);
		wsaQuerySet.lpszServiceInstanceName = pszInstanceName;
		wsaQuerySet.lpszComment = "DT Mouse Service";
		wsaQuerySet.dwNameSpace = NS_BTH;
		wsaQuerySet.dwNumberOfCsAddrs = 1;      // Must be 1.
		wsaQuerySet.lpcsaBuffer = lpCSAddrInfo; // Req'd.

		// As long as we use a blocking accept(), we will have a race
		// between advertising the service and actually being ready to
		// accept connections.  If we use non-blocking accept, advertise
		// the service after accept has been called.
		//
		if (SOCKET_ERROR == WSASetService(&wsaQuerySet, RNRSERVICE_REGISTER, 0)) {
			OutputDebugPrintf("=CRITICAL= | WSASetService() call failed. WSAGetLastError=[%d]\n", WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	//
	// listen() call indicates winsock2 to listen on a given socket for any incoming connection.
	//
	if (CD_SUCCESS == ulRetCode) {
		if (SOCKET_ERROR == listen(LocalSocket, 1)) {
			OutputDebugPrintf("=CRITICAL= | listen() call failed w/socket = [0x%I64X]. WSAGetLastError=[%d]\n", (ULONG64)LocalSocket, WSAGetLastError());
			ulRetCode = CD_FAILURE;
		}
	}

	if (CD_SUCCESS == ulRetCode) {
		m_LocalSocket = LocalSocket;
		while (m_bRuning)
		{
			//
			// accept() call indicates winsock2 to wait for any
			// incoming connection request from a remote socket.
			// If there are already some connection requests on the queue,
			// then accept() extracts the first request and creates a new socket and
			// returns the handle to this newly created socket. This newly created
			// socket represents the actual connection that connects the two sockets.
			//
			SOCKADDR_BTH    SockAddrBthPeer = { 0 };
			ClientSocket = accept(LocalSocket, (struct sockaddr *)&SockAddrBthPeer, &iAddrLen);
			if (INVALID_SOCKET == ClientSocket) {
				ulRetCode=WSAGetLastError();
				OutputDebugPrintf("=CRITICAL= | accept() call failed. WSAGetLastError=[%d]\n", WSAGetLastError());
				ulRetCode = CD_FAILURE;
				break; // Break out of the for loop
			}
			RunWork(ClientSocket,&SockAddrBthLocal, &SockAddrBthPeer);

			if (INVALID_SOCKET != ClientSocket) {
				closesocket(ClientSocket);
				ClientSocket = INVALID_SOCKET;
			}
		}
	}

	if (NULL != lpCSAddrInfo) {
		HeapFree(GetProcessHeap(), 0, lpCSAddrInfo);
		lpCSAddrInfo = NULL;
	}
	if (NULL != pszInstanceName) {
		HeapFree(GetProcessHeap(), 0, pszInstanceName);
		pszInstanceName = NULL;
	}

	return ulRetCode;
}

// ============================================================================
// 函数功能：启动蓝牙服务线程
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
int dtblueserver::RunWork(SOCKET ClientSocket, SOCKADDR_BTH *pLocalAddr, SOCKADDR_BTH *pPeerAddr)
{
	ULONG           ulRetCode = CD_SUCCESS;
	char *          pszDataBuffer = NULL;
	char *          pszDataBufferIndex = NULL;
	UINT            iLengthSended = 0;
	UINT            uiTotalLengthReceived;
	char            aSendBuf[128];
	ULONG           iSockoptOn = 1;
	ULONG           iStage = 0;

	//发送验证信息
	DTBlueHeader *pHeadr = (DTBlueHeader *)aSendBuf;
	pHeadr->aCompany[0] = 'D';
	pHeadr->aCompany[1] = 'T';
	pHeadr->iType = 0;
	pHeadr->iLenght = sizeof(DTBlueVerifer);
	DTBlueVerifer *pVerfier = (DTBlueVerifer *)(aSendBuf+sizeof(DTBlueHeader));
	CodeVerifer(pLocalAddr, pVerfier->aESA);
	pVerfier->aVersion[0] = 0;
	pVerfier->aVersion[1] = 0;
	pVerfier->aVersion[0] = 0;
	pVerfier->aVersion[0] = 0;
	uiTotalLengthReceived = sizeof(DTBlueHeader) + sizeof(DTBlueVerifer);
	iLengthSended =send(ClientSocket, aSendBuf, uiTotalLengthReceived, 0);
	if(uiTotalLengthReceived != iLengthSended){
		return -1;
	}

	m_logFlag |= 0x0002;

	//设置非阻塞模式
	//ioctlsocket(ClientSocket, FIONBIO, &(iSockoptOn));
	while (m_bRuning) 
	{
		//Sleep(1000);
		//continue;
		int iRecvRet = ReceiveBlueData(ClientSocket);
		if (iRecvRet == 1)
		{
			if (iStage == 0) //验证阶段
			{
				if (m_BlueMng.tHdr.aCompany[0] != 'D' || m_BlueMng.tHdr.aCompany[1] != 'T'
					|| m_BlueMng.tHdr.iType != 0 || m_BlueMng.tHdr.iLenght != sizeof(DTBlueVerifer))
					break;
				//if (DecodeVerifer(pPeerAddr,(unsigned char *)m_BlueMng.revChar) < 0)
				//	break;
				iStage = 1;

				m_logFlag |= 0x0004;
			}
			else
			{
				switch (m_BlueMng.tHdr.iType)
				{
				case 1: // 鼠标数据
					if (m_BlueMng.tHdr.iLenght == sizeof(DTMouseData))
					{
						if (m_hfRaw != NULL)
						{
							DTMouseData *pRcvData = (DTMouseData *)m_BlueMng.revChar;
							USHORT  btnFlags = MOUSE_WHEEL;
							ULONG   requiredLength = 0;
							MOUSE_INPUT_DATA msData;
							msData.UnitId = 0;
							msData.Flags = MOUSE_MOVE_RELATIVE;

							if (pRcvData->left == 1)
								btnFlags |= MOUSE_LEFT_BUTTON_DOWN;
							else if(pRcvData->left==2)
								btnFlags |= MOUSE_LEFT_BUTTON_UP;
							else
							{
							}
							if (pRcvData->right == 1)
								btnFlags |= MOUSE_RIGHT_BUTTON_DOWN;
							else if (pRcvData->right == 2)
								btnFlags |= MOUSE_RIGHT_BUTTON_UP;
							else
							{
							}
							//OutputDebugPrintf(("....... btnFlags = %02X wl=%d \n"), btnFlags,pRcvData->wheel);
							m_logFlag |= 0x0100;

							msData.ButtonFlags = btnFlags;
							msData.ButtonData = pRcvData->wheel;
							msData.RawButtons = 0;
							msData.LastY = pRcvData->y;
							msData.LastX = pRcvData->x;
							msData.ExtraInformation = 6677;
							
							if (!DeviceIoControl(m_hfRaw, IOCTL_DT_SET_MOUSE_DATA,
								&msData, sizeof(MOUSE_INPUT_DATA), NULL, 0,
								&requiredLength, NULL))
							{
								m_logFlag |= 0x0200;
								m_iOpenFileCode = GetLastError();
								//OutputDebugPrintf(("Open file failed , error code = %d\n"), m_iOpenFileCode);
							}
						}
					}
					else
					{
						OutputDebugPrintf("error world \n");
					}
					break;
				default:
					OutputDebugPrintf("......... undefined ......for future \n");
					break;
				}
			}
		}
		else if (iRecvRet == 0)
		{
			continue; //继续接收
		}
		else
		{
			break; //接收错误
		}
	}
	return(ulRetCode);
}

// ============================================================================
// 函数功能：从SOCKET读数据。
// 输入参数：连接对象
// 输出参数：无
// 返回值：  =-1失败，0=未完成，= 1 完成 
// ============================================================================
int dtblueserver::ReceiveBlueData(SOCKET hSkt)
{
	int iRet = CD_SUCCESS;
	DTBlueReceive *pCnntMng = &m_BlueMng;
	int iMode = pCnntMng->iMode;
	int iRemain = pCnntMng->iRemain;
	int iLength;

	fd_set fdReadSet;
	struct timeval tTimeout;
	FD_ZERO(&fdReadSet);
	FD_SET(hSkt, &fdReadSet);
	tTimeout.tv_sec = 0;
	tTimeout.tv_usec = 500000; //500ms
	CM_INT iFds = select(hSkt + 1, &fdReadSet, 0, 0, &tTimeout);
	if (iFds <= 0) //无数据返回
	{
		return iRet;
	}

	//以下接收数据
	if (iMode == 0) //接收头数据
	{
		char *pRevHdr=(char *)&pCnntMng->tHdr;
		iLength = recv(hSkt, pRevHdr+sizeof(DTBlueHeader)-iRemain, iRemain, 0);
		if (iLength <= 0 || iLength>iRemain)//当有东西可读时，一定要检查recv()或 read()的返回值，返回-1和0 时要作出关断本地Socket的处理
		{
			OutputDebugPrintf("### Receive data length error,will close socket.iLength=%d,iRemain=%d \n", iLength, iRemain);
			iMode = 0;
			iRemain = sizeof(DTBlueHeader);
			iRet = CD_FAILURE;
			pCnntMng->iRecvStat++;
		}
		else if (iLength == iRemain)
		{
			char *pStead =(pCnntMng->tHdr.aCompany);
			if (pStead[0] != 'D' || pStead[1] != 'T' 
				|| pCnntMng->tHdr.iLenght <= 0 || pCnntMng->tHdr.iLenght>CN_BLUE_MAXLEN)
			{
				OutputDebugPrintf("### Receive data format error, pStead->len=%d \n", pCnntMng->tHdr.iLenght);
				iMode = 0;
				iRemain = sizeof(DTBlueHeader);
				iRet = CD_FAILURE;
				pCnntMng->iRecvStat++;
			}
			else
			{
				iMode = 1;
				iRemain = pCnntMng->tHdr.iLenght;
			}
			//alllen++;
			//if(!(alllen%100))
			//	printf("pStead->num=%d, iDatalen=%d,pStead->len=%d \n",pStead->num, iDatalen,pStead->len);
		}
		else
		{
			iRemain = iRemain - iLength;
		}
	}
	else if (iMode == 1)
	{
		char *revChar = pCnntMng->revChar;
		int iDatalen = pCnntMng->tHdr.iLenght;
		iLength = recv(hSkt, revChar + iDatalen - iRemain, iRemain, 0);
		if (iLength <= 0 || iLength>iRemain)
		{
			OutputDebugPrintf("### Receive data length error,will close socket.iLength=%d,iRemain=%d \n", iLength, iRemain);
			iMode = 0;
			iRemain = sizeof(DTBlueHeader);
			iRet = CD_FAILURE;
			pCnntMng->iRecvStat++;
		}
		else if (iRemain == iLength)
		{
			iRet = 1;
			iMode = 0;
			iDatalen = 0;
			iRemain = sizeof(DTBlueHeader);
		}
		else
		{
			iRemain -= iLength;
		}
	}
	else
	{
		OutputDebugPrintf("#######  imode error =%d   \n", iMode);
		iMode = 0;
		iRemain = sizeof(DTBlueHeader);
		iRet = CD_FAILURE;
	}

	pCnntMng->iMode  = iMode;
	pCnntMng->iRemain = iRemain;
	return iRet;
}

// ============================================================================
// 函数功能：编码认证信息
// 输入参数：pLocalAddr 本地地址
//           output 加密输出
// 输出参数：无
// 返回值：  =-1失败，0=完成
// ============================================================================
int dtblueserver::CodeVerifer(SOCKADDR_BTH *pLocalAddr, unsigned char output[16])
{
	static unsigned char key[16] = { 'D','I','N','G','T','A','N','G','T','E','C','H','.','C','O','M' };
	unsigned char input[20] = {0};
	sm4_context ctx;
	//unsigned long i;
	unsigned long len = 16;

	unsigned char *pAddr = (unsigned char *)&(pLocalAddr->btAddr);
	sprintf((char *)input, "%02X:%02X:%02X:%02X:%02X:%02X", pAddr[5], pAddr[4], pAddr[3], pAddr[2], pAddr[1], pAddr[0]);

	//sm4 ecb encrypt testing   
	sm4_setkey_enc(&ctx, key);
	sm4_crypt_ecb(&ctx, 1, len, input, output);

	//printf("%-15s", "ECB cipher:");
	//for (i = 0; i<len; i++)
	//	printf("%02x ", output[i]);
	//printf("\n");
	return 0;
}

// ============================================================================
// 函数功能：解码认证信息
// 输入参数：pPeerAddr 对端地址
//           input  接收信息
// 输出参数：无
// 返回值：  =-1失败，0=成功
// ============================================================================
int dtblueserver::DecodeVerifer(SOCKADDR_BTH *pPeerAddr, unsigned char *input)
{
	static unsigned char key[16] = { 'D','I','N','G','T','A','N','G','T','E','C','H','.','C','O','M' };
	unsigned char output[16];
	char fmtput[20];
	sm4_context ctx;
	//unsigned long i;
	unsigned long len = 16;

	//sm4 ecb decrypt testing  
	sm4_setkey_dec(&ctx, key);
	sm4_crypt_ecb(&ctx, 0, len, input, output);

	unsigned char *pAddr = (unsigned char *)&(pPeerAddr->btAddr);
	sprintf(fmtput,"%02X:%02X:%02X:%02X:%02X:%02X", pAddr[5], pAddr[4], pAddr[3], pAddr[2], pAddr[1], pAddr[0]);

	if (memcmp(output, fmtput, 16)!=0)
		return -1;

	return 0;
}



