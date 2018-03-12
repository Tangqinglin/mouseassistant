
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

//ȡ��ǰʱ��
tagComTime ComGetTime();

//8�ֽ�������С��ת��
CM_INT64 COMHTON64(CM_INT64 iData);

//8�ֽ�������С��ת��
CM_INT64 COMNTOH64(CM_INT64 iData);

// ============================================================================
// �������ܣ�����32λУ���
// ���������32λ�����ַ ���㳤��
// �����������
// ����ֵ��  ������32λУ���
// ============================================================================
CM_UINT32 Fun_CalSum32( CM_UINT32* pdwMsgBuf, CM_UINT32 dwMsgLen );

// ============================================================================
// �������ܣ�����һ�γ���˫����CRCУ���뺯��
// ��������������ַ ���㳤��
// �����������
// ����ֵ��  ������16λCRCУ����
// ============================================================================
CM_UINT32 Fun_CrcStream32( CM_UCHAR *pdwCalBuf, CM_UINT32 dwCalLen );

//��������CRCУ��
CM_UINT32 FileCrc32(const CM_CHAR *pFileName);

#ifdef _LINUX 
CM_BOOLEAN FindRunning(const CM_CHAR *pLockFile);

CM_BOOLEAN AlreadyRunning(const CM_CHAR *pLockFile);
//д�ļ�����
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