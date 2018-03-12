
#include "comutility.h"


// CRC-MODBUS校验码对照表高字节, 校验多项式为G(x)=x^16+x^12+x^6+1
const CM_UINT32 g_crc32_table[256]=
{
	0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
};
// 字节逆转后的CRC32算法，字节序为b1,b2,…,b8  
//#define POLY 0xEDB88320L // CRC32生成多项式
//static unsigned int crc_table[256];
//unsigned int get_sum_poly(unsigned char data)
//{
//	int j;
//	unsigned int sum_poly = data;
//	for(j = 0;j < 8;j++)
//	{
//		int hi = sum_poly&0x01; // 取得reg的最高位
//		sum_poly >>= 1;
//		if(hi) sum_poly = sum_poly^POLY;
//	}
//	return sum_poly;
//}
//void create_crc_table()
//{
//	int i;
//	for( i = 0; i < 256; i++)
//	{
//		crc_table[i] = get_sum_poly(i&0xFF);
//	}
//	
//}
// ============================================================================
// 函数功能：计算一段长度双字型CRC校验码函数
// 输入参数：计算基址 计算长度
// 输出参数：无
// 返回值：  计算后的32位CRC校验码
// ============================================================================
CM_UINT32 Fun_CrcStream32( CM_UCHAR *data, CM_UINT32 len )
{  
    CM_UINT crc = 0xffffffff;    
    CM_UCHAR *p = data;  
    CM_UINT i;  
    for(i=0; i <len; i++)  
        crc =  g_crc32_table[( crc ^( *(p+i)) ) & 0xff] ^ (crc >> 8);  
    return  ~crc ;   
}  

// ============================================================================
// 函数功能：获取系统完整时间，秒，微妙
// 输入参数：
// 输出参数：无
// 返回值：  时间
// ============================================================================
tagComTime ComGetTime()
{
	tagComTime tTimeUTC;
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st); 
	tTimeUTC.iSec=(CM_INT32)time(NULL);
	tTimeUTC.iUSec=st.wMilliseconds*1000;
#else
	struct timeval  tCurTime;
	gettimeofday(&tCurTime,0);
	tTimeUTC.iSec=tCurTime.tv_sec;
	tTimeUTC.iUSec=tCurTime.tv_usec;
#endif
	return tTimeUTC;
}
// ============================================================================
// 函数功能：8字节整数大小端转换
// 输入参数：
// 输出参数：无
// 返回值：  
// ============================================================================
CM_INT64 COMHTON64(CM_INT64 iData)        
{                    
	CM_UCHAR *pInBuf=(CM_UCHAR *)&iData;
	CM_UCHAR aOutBuf[8];
	aOutBuf[0]=pInBuf[CN_1DWB4_HH];        
	aOutBuf[1]=pInBuf[CN_1DWB4_HL];        
	aOutBuf[2]=pInBuf[CN_1DWB4_LH];        
	aOutBuf[3]=pInBuf[CN_1DWB4_LL];        
	aOutBuf[4]=pInBuf[CN_2DWB4_HH];        
	aOutBuf[5]=pInBuf[CN_2DWB4_HL];        
	aOutBuf[6]=pInBuf[CN_2DWB4_LH];        
	aOutBuf[7]=pInBuf[CN_2DWB4_LL];        
	return *(CM_INT64 *)aOutBuf;
}
// ============================================================================
// 函数功能：8字节整数大小端转换
// 输入参数：
// 输出参数：无
// 返回值：  
// ============================================================================
CM_INT64 COMNTOH64(CM_INT64 iData)         
{        
	CM_UCHAR *pInBuf=(CM_UCHAR *)&iData;
	CM_UCHAR aOutBuf[8];
	aOutBuf[CN_1DWB4_HH]=pInBuf[0];         
	aOutBuf[CN_1DWB4_HL]=pInBuf[1];         
	aOutBuf[CN_1DWB4_LH]=pInBuf[2];         
	aOutBuf[CN_1DWB4_LL]=pInBuf[3];         
	aOutBuf[CN_2DWB4_HH]=pInBuf[4];         
	aOutBuf[CN_2DWB4_HL]=pInBuf[5];         
	aOutBuf[CN_2DWB4_LH]=pInBuf[6];         
	aOutBuf[CN_2DWB4_LL]=pInBuf[7];    
	return *(CM_INT64 *)aOutBuf;
}                     

// ============================================================================
// 函数功能：计算32位校验和
// 输入参数：32位计算基址 计算长度
// 输出参数：无
// 返回值：  计算后的32位校验和
// ============================================================================
CM_UINT32 Fun_CalSum32( CM_UINT32* pdwMsgBuf, CM_UINT32 dwMsgLen )
{
    register CM_UINT32  dwSum = 0;              // 计算后的32位校验和
    register CM_UINT32  dwLoop;                 // 临时变量

    for( dwLoop=0; dwLoop<dwMsgLen; dwLoop++ )
    {
        dwSum += pdwMsgBuf[dwLoop];
    }
    return dwSum;
}


#ifdef _LINUX 
CM_BOOLEAN AlreadyRunning(const CM_CHAR *pLockFile)
{
	CM_INT fdLockFile;
	CM_CHAR aPid[32];
	struct flock fl;
	fdLockFile=open(pLockFile,O_RDWR|O_CREAT,666);
	if(fdLockFile<0)
	{
		printf("open %s  failed.\n",pLockFile);
		return 1;
	}
    fl.l_type=F_WRLCK;
	fl.l_whence=SEEK_SET;
	fl.l_start=0;
	fl.l_len=0;
	if(fcntl(fdLockFile,F_SETLK,&fl)<0)
	{
		if(EACCES==errno || EAGAIN ==errno)
		{
		}
		close(fdLockFile);
		return 1;
	}

	ftruncate(fdLockFile,0);
	sprintf(aPid,"%ld",(CM_INT32)getpid());
	write(fdLockFile,aPid,strlen(aPid)+1);

	return 0;

}

// ============================================================================
// 函数功能：检查进程是否在运行，通过文件为锁定认为在运行
// 输入参数：无
// 输出参数：无
// 返回值：  0
// ============================================================================
CM_BOOLEAN FindRunning(const CM_CHAR *pLockFile)
{
	CM_INT fdLockFile;
	struct flock fl;
	fdLockFile=open(pLockFile,O_RDWR,666);
	if(fdLockFile<0)
	{
		return 0;
	}
    fl.l_type=0;
	fl.l_whence=0;
	fl.l_start=0;
	fl.l_len=0;
	if(fcntl(fdLockFile,F_GETLK,&fl)<0)
	{
		close(fdLockFile);
		return 0;
	}
	else
	{
                close(fdLockFile);
		if(fl.l_type==F_WRLCK)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
#endif

#ifdef _LINUX
// 转换输出
int convert(iconv_t cd,char *inbuf,int inlen,char *outbuf,int outlen)
{
	char **pin = &inbuf;
	char **pout = &outbuf;

	memset(outbuf,0,outlen);

	int ret = iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen);
	int errtimes = 0;
	if(ret == -1)		
	{
		printf("iconv error! errno=%d, src=%s, desc=%s\n",errno,inbuf,outbuf);
		errtimes++;
	}

	return ret;
}
#endif
/////////////////////////////////////////////////////////////////////////
// 描    述:
// 作    者:  唐庆林
// 参数说明: 
// 返 回 值:
/////////////////////////////////////////////////////////////////////////
CM_BOOLEAN Gb2Utf8(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen)
{

#ifdef WIN32
	WCHAR *strSrc;
	TCHAR *szRes=pOut;
	
	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_ACP, 0, pIn, -1, NULL, 0);
	strSrc = (WCHAR *)malloc((i+1)<<1);
	MultiByteToWideChar(CP_ACP, 0, pIn, -1, strSrc, i);
	
	//获得临时变量的大小
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	if((i+1)>iDstLen)
		 i=iDstLen-1;
	WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);
	
	free(strSrc);
#else
	iconv_t cd;
	int iInLen = strlen(pIn);
	pOut[0] = '\0';
	
	cd = iconv_open("utf-8//IGNORE","gb18030//IGNORE");
	convert(cd,pIn,iInLen,pOut,iDstLen);
	iconv_close(cd);
#endif
	return CD_TRUE;
}
/////////////////////////////////////////////////////////////////////////
// 描    述:
// 作    者:  唐庆林
// 参数说明: 
// 返 回 值:
/////////////////////////////////////////////////////////////////////////
CM_BOOLEAN Utf82Gb(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen)
{
#ifdef WIN32
     WCHAR *strSrc;// = (TCHAR*)pOut;
     TCHAR *szRes=pOut;

     //获得临时变量的大小
     int i = MultiByteToWideChar(CP_UTF8, 0, pIn, -1, NULL, 0);
     strSrc = (WCHAR *)malloc((i+1)<<1);
     MultiByteToWideChar(CP_UTF8, 0, pIn, -1, strSrc, i);

     //获得临时变量的大小
     i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	 if((i+1)>iDstLen)
		 i=iDstLen-1;
     WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

     free(strSrc);
#else
	iconv_t cd;
	int iInLen = strlen(pIn);
	pOut[0] = '\0';

	cd = iconv_open("gb18030//IGNORE","utf-8//IGNORE");
	convert(cd,pIn,iInLen,pOut,iDstLen);
	iconv_close(cd);

// 	int i=0;
// 	int s=0,ol=0;
// 	for(i=0;i<iInLen;i+=s) //多次转换
// 	{
// 		s=iInLen-i;
// 		if(s > 10240)
// 			s = 10240;
// 
// 		cc.convert(pIn+i,s,pOut+ol,iOutLen-ol);
// 		ol=strlen(pOut);
// 	}

#endif

	return CD_TRUE;
}

/////////////////////////////////////////////////////////////////////////
// 描    述:
// 作    者:  唐庆林
// 参数说明: 
// 返 回 值:
/////////////////////////////////////////////////////////////////////////
CM_BOOLEAN Unicode2Gb(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen)
{
#ifdef WIN32
     TCHAR *szRes=pOut;
     //获得临时变量的大小
     int i = WideCharToMultiByte(CP_ACP, 0, (WCHAR *)pIn, -1, NULL, 0, NULL, NULL);
	 if((i+1)>iDstLen)
		 i=iDstLen-1;
     WideCharToMultiByte(CP_ACP, 0,  (WCHAR *)pIn, -1, szRes, i, NULL, NULL);

#else
	iconv_t cd;
	int iInLen = strlen(pIn);
	pOut[0] = '\0';

	cd = iconv_open("gb18030//IGNORE","unicode//IGNORE");
	convert(cd,pIn,iInLen,pOut,iDstLen);
	iconv_close(cd);
#endif

	return CD_TRUE;
}
// ============================================================================
// 函数功能：内存到16进制字符串转换
//
// 输入参数：p 内存指针，len内存长度
// 输出参数：无
// 返回值：  无
// ============================================================================
unsigned char *MemToHexStr(unsigned char *p, int len,unsigned char *pDst)
{
	int i;
	unsigned char *pBuf=pDst;

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
	*pBuf=0;
	return pDst;
}
// ============================================================================
// 函数功能：计算文件的crc32校验
//
// 输入参数：
// 输出参数：无
// 返回值：  无
// ============================================================================
CM_UINT32 FileCrc32(const CM_CHAR *pFileName)
{
	CM_CHAR   *pDataBuf;
	CM_UINT32 iFileLen;
	CM_UINT32 iCrc32=0;
	FILE *pd=fopen(pFileName,"rb");
	if(pd==0)
	{
		return iCrc32;
	}
	fseek(pd,0,SEEK_END);
	iFileLen=ftell(pd);
	fseek(pd,0,SEEK_SET);
	pDataBuf=(CM_CHAR *)calloc(1,iFileLen);
	iFileLen=fread(pDataBuf,1,iFileLen,pd);
	fclose(pd);
	iCrc32 = Fun_CrcStream32((CM_UCHAR *)pDataBuf,iFileLen);
	free(pDataBuf);

	return iCrc32;
}
