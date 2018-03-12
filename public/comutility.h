
#ifndef _COMUTILITY_H_
#define _COMUTILITY_H_
#include "platformdef.h"
#include "comtypes.h"
#ifdef _LINUX
#include "iconv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	CM_INT32 iSec;
	CM_INT32 iUSec;
}tagComTime;

//取当前时间
tagComTime ComGetTime();

//8字节整数大小端转换
CM_INT64 COMHTON64(CM_INT64 iData);

//8字节整数大小端转换
CM_INT64 COMNTOH64(CM_INT64 iData);

// ============================================================================
// 函数功能：计算32位校验和
// 输入参数：32位计算基址 计算长度
// 输出参数：无
// 返回值：  计算后的32位校验和
// ============================================================================
CM_UINT32 Fun_CalSum32( CM_UINT32* pdwMsgBuf, CM_UINT32 dwMsgLen );

// ============================================================================
// 函数功能：计算一段长度双字型CRC校验码函数
// 输入参数：计算基址 计算长度
// 输出参数：无
// 返回值：  计算后的16位CRC校验码
// ============================================================================
CM_UINT32 Fun_CrcStream32( CM_UCHAR *pdwCalBuf, CM_UINT32 dwCalLen );

//计算程序的CRC校验
CM_UINT32 FileCrc32(const CM_CHAR *pFileName);

#ifdef _LINUX 
CM_BOOLEAN FindRunning(const CM_CHAR *pLockFile);

CM_BOOLEAN AlreadyRunning(const CM_CHAR *pLockFile);
//写文件函数
#define UK_SYS_FSYNC()   sync()
#else
#define UK_SYS_FSYNC()

#endif

CM_BOOLEAN Utf82Gb(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen);
CM_BOOLEAN Gb2Utf8(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen);
CM_BOOLEAN Unicode2Gb(CM_CHAR *pIn,CM_CHAR *pOut,CM_INT iDstLen);


unsigned char *MemToHexStr(unsigned char *p, int len,unsigned char *pDst);

#ifdef __cplusplus
}
#endif

#endif //_CYUTILITY_H_