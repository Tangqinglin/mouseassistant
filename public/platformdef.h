
#ifndef _PLATFORMDEF_H_
#define _PLATFORMDEF_H_


#ifdef _LINUX
#include <sys/time.h>
#include <unistd.h>	/* SHOULD be before any other include files 	*/
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>		/* open, O_RDONLY, O_WRONLY, etc.	*/
#include <ctype.h>
#include <limits.h>
#include <signal.h>		/* for "kill", etc.		*/
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>		/* gethostbyname, etc.			*/
#include <netinet/in.h>		/* IPPROTO_*, etc.			*/
#include <sys/un.h>		/* for sockaddr_un			*/
#include <arpa/inet.h>		/* inet_addr, etc.			*/
#include <netinet/tcp.h>	/* TCP_NODELAY, etc.			*/
#include <pthread.h>
#include <dirent.h>
#include <math.h>
#include <execinfo.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h> 
#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <linux/netlink.h>

#ifndef INVALID_SOCKET
#define SOCKET int
#define INVALID_SOCKET -1
#endif

#define CM_MSSLEEP(t)  usleep(t*1000)

#elif _WIN32

#pragma warning(disable: 4996)

#include <io.h>
#include <WinSock2.h>
#include <windows.h>
#include <process.h>		/* for _beginthread, _endthread	*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <conio.h>
#include <fcntl.h>
#include <math.h>
#include <direct.h>
#include <signal.h>

 #if !defined(__cplusplus)
#define inline __inline
#endif

#define CM_MSSLEEP(t) Sleep(t) 
#else

#endif

#ifndef cmmin
#define ukmin(a,b)  a<b?a:b
#endif

#ifndef cmmax
#define ukmax(a,b)  a>b?a:b
#endif

// 大小端工作模式
#define CN_BIG_ENDIAN           ( 1 )                       // 1:大端模式   0:小端模式
#define CN_LIT_ENDIAN           ( 0 )

#define CN_BYTE_ORDER           ( CN_LIT_ENDIAN )           // 定义为大端模式

#if( CN_BYTE_ORDER==CN_BIG_ENDIAN )                         // 大端模式
    #define  CN_BYTE_LO         ( 1 )                       // 低字节
    #define  CN_BYTE_HI         ( 0 )                       // 高字节

    #define  CN_WORD_LO         ( 1 )                       // 低字
    #define  CN_WORD_HI         ( 0 )                       // 高字

    #define  CN_2DWB4_LL        ( 7 )                       // 最低字节
    #define  CN_2DWB4_LH        ( 6 )                       // 次低字节
    #define  CN_2DWB4_HL        ( 5 )                       // 次高字节
    #define  CN_2DWB4_HH        ( 4 )                       // 最高字节
    #define  CN_1DWB4_LL        ( 3 )                       // 最低字节
    #define  CN_1DWB4_LH        ( 2 )                       // 次低字节
    #define  CN_1DWB4_HL        ( 1 )                       // 次高字节
    #define  CN_1DWB4_HH        ( 0 )                       // 最高字节

    #define  CN_DWB4_LL         ( 3 )                       // 最低字节
    #define  CN_DWB4_LH         ( 2 )                       // 次低字节
    #define  CN_DWB4_HL         ( 1 )                       // 次高字节
    #define  CN_DWB4_HH         ( 0 )                       // 最高字节

#else                                                       // 缺省为小端模式
    #define  CN_BYTE_LO         ( 0 )                       // 低字节
    #define  CN_BYTE_HI         ( 1 )                       // 高字节

    #define  CN_WORD_LO         ( 0 )                       // 低字
    #define  CN_WORD_HI         ( 1 )                       // 高字

    #define  CN_DWB4_LL         ( 0 )                       // 最低字节
    #define  CN_DWB4_LH         ( 1 )                       // 次低字节
    #define  CN_DWB4_HL         ( 2 )                       // 次高字节
    #define  CN_DWB4_HH         ( 3 )                       // 最高字节

    #define  CN_2DWB4_LL        ( 0 )                       // 最低字节
    #define  CN_2DWB4_LH        ( 1 )                       // 次低字节
    #define  CN_2DWB4_HL        ( 2 )                       // 次高字节
    #define  CN_2DWB4_HH        ( 3 )                       // 最高字节
    #define  CN_1DWB4_LL        ( 4 )                       // 最低字节
    #define  CN_1DWB4_LH        ( 5 )                       // 次低字节
    #define  CN_1DWB4_HL        ( 6 )                       // 次高字节
    #define  CN_1DWB4_HH        ( 7 )                       // 最高字节
#endif

#endif //_PLATFORMDEF_H_