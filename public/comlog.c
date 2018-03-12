
#include <stdarg.h>
#include <stdio.h>

#include "comlog.h"

// ============================================================================
// 函数功能：以16进制打印内存
//
// 输入参数：p 内存指针，len内存长度
// 输出参数：无
// 返回值：  无
// ============================================================================
void PrintHex(char *p, int len)
{
	int i;
	char *pDst=(char *)malloc(len*3+2);
	char *pBuf=pDst;

	for(i=0;i<len;i++,p++)
	{
		unsigned char cT=*(unsigned char *)p;
		unsigned char cT1=(cT&0xF0);
		cT1=(cT1>>4);
		if( cT1<10)
		{
			*pBuf=(cT1+'0');
		}
		else
		{
			*pBuf=(cT1-10+'A');
		}
		pBuf++;
		cT1=(cT&0x0F);
		if( cT1<10)
		{
			*pBuf=(cT1+'0');
		}
		else
		{
			*pBuf=(cT1-10+'A');
		}
		pBuf++;
		*pBuf=' ';
		pBuf++;

	}
	*pBuf='\n';
	pBuf++;
	*pBuf=0;
	printf((char *)pDst);
	free(pDst);
}

#ifdef _LINUX
// ============================================================================
// 函数功能：写程序崩溃日志
// 输入参数：无
// 输出参数：无
// 返回值：  0
// ============================================================================
static void WriteErrorLog(char *pSig)
{
	struct   tm     *timenow;         //实例化tm结构指针
	struct timeval  tCurTime;
	char sLogFile[64];
	FILE *fp;
	void *array[10];
	size_t size;
	char **strings;
	size_t i;
	size =backtrace(array, 10);
	strings = (char **)backtrace_symbols(array, size);

	gettimeofday(&tCurTime,0);
	timenow   =   localtime(&tCurTime.tv_sec);
	if(timenow==0)
	{
		printf("WriteErrorLog failed ! line=%d \n",__LINE__);
		return ;
	}
	sprintf(sLogFile,"./%s%02d.date",pSig,timenow->tm_sec);
	fp=fopen(sLogFile,"w");
	if(fp==0)
	{
		printf("WriteErrorLog failed ! line=%d \n",__LINE__);
		return ;
	}
	fprintf(fp,"%s%04d%02d%02dH%02d%02d%02d",pSig,timenow->tm_year+1900,timenow->tm_mon+1,timenow->tm_mday ,timenow->tm_hour,timenow->tm_min ,timenow->tm_sec);
	fprintf(fp,"addr2line -C -f -e program address\n");
	for (i = 0; i < size; i++) {
		fprintf(fp, "%d %s \n",i,strings[i]);

	}

	free (strings);

	fclose(fp);
}
// ============================================================================
// 函数功能：处理SIGSEGV信号量
// 输入参数：无
// 输出参数：无
// 返回值：  0
// ============================================================================
void ProcessSIG(int iSig)
{
	switch(iSig)
	{
	case SIGSEGV:
		WriteErrorLog((char *)"segv");
	    exit(1);
		abort();
		break;
	case SIGPIPE:
		printf("Pipe break **************\n");
		break;
	case SIGFPE:
		WriteErrorLog((char *)"fpe");
		exit(1);
		abort();
		break;
	case SIGCHLD:
		{
			int stat;
			int pid;
			pid=waitpid(-1,&stat,WNOHANG);
			if(WIFEXITED(stat)){
				 printf("The child %d exit with code %d\n", pid, WEXITSTATUS(stat));   
			}
		}
		break;
	default:
		break;

	};
}

#endif

