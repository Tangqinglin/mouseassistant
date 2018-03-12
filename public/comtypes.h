/************************************************************************/
/* CYSN SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/


#ifndef _COMTYPES_INCLUDE_
#define _COMTYPES_INCLUDE_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/************************************************************************/
/************************************************************************/
/* General purpose defines, same for all platforms			*/

#define CD_TRUE		1
#define CD_FALSE	0		
#define CD_SUCCESS 	0
#define CD_FAILURE 	-1
#define CD_BIG_ENDIAN		0
#define CD_LITTLE_ENDIAN	1

					
#define CD_CONST  const



/************************************************************************/
/************************************************************************/
/* SYSTEM and CPU select defines. These are based on built in compiler	*/
/* defines which allow automatic detection of the compiler.		*/
/************************************************************************/

/************************************************************************/
/* WINDOWS 95/NT							*/
/************************************************************************/
#if defined(_WIN32)				/* VC++, 32-Bit		*/


/* We can tolerate machine-dependent sizes for these types		*/
#define CM_CHAR    char	
#define CM_INT     int		
#define CM_LONG    long int     	
#define CM_UCHAR   unsigned char	
#define CM_UINT    unsigned int		
#define CM_ULONG   unsigned long     	
#define CM_VOID    void      		
#define CM_DOUBLE  double		
#define CM_FLOAT   float		

/* General purpose return code						*/
#define CM_RET  int		

/* We need specific sizes for these types				*/
#define CM_INT8     char     	
#define CM_INT16    short     	
#define CM_INT32    long     	
#define CM_INT64    __int64
#define CM_UINT8    unsigned char     	
#define CM_UINT16   unsigned short    	
#define CM_UINT32   unsigned long    	
#define CM_UINT64   unsigned __int64
#define CM_BOOLEAN  unsigned char		

/* This define shows that we really have support for 64 bit integers	*/
#define INT64_SUPPORT

/* This define shows that we have supplied all required 		*/
#define _CYSRTYPES_DEFINED

#endif


/************************************************************************/
/* VXWORKS - VXWORKS on Motorola 680x0 processor			*/
/************************************************************************/

#if defined(VXWORKS) 

/* We can tolerate machine-dependent sizes for these types		*/
#define CM_CHAR    char	
#define CM_INT     int		
#define CM_LONG    long int     	
#define CM_UCHAR   unsigned char	
#define CM_UINT    unsigned int		
#define CM_ULONG   unsigned long     	
#define CM_VOID    void      		
#define CM_DOUBLE  double		
#define CM_FLOAT   float		

/* General purpose return code						*/
#define CM_RET int		

/* We need specific sizes for these types				*/
#define CM_INT8   char     	
#define CM_INT16  short     	
#define CM_INT32  long     	
#define CM_UINT8  unsigned char     	
#define CM_UINT16 unsigned short    	
#define CM_UINT32 unsigned long    	
#define CM_BOOLEAN  unsigned char		

/* This define shows that we have supplied all required 		*/
#define _CYSRTYPES_DEFINED

#endif

/************************************************************************/
/* LINUX SYSTEM								*/
/* OR LYNXOS SYSTEM (same types)					*/
/************************************************************************/
#if defined(_LINUX)

#include <endian.h>

/* We can tolerate machine-dependent sizes for these types		*/
#define CM_CHAR    char	
#define CM_INT     int		
#define CM_LONG    long int     	
#define CM_UCHAR   unsigned char	
#define CM_UINT    unsigned int		
#define CM_ULONG   unsigned long     	
#define CM_VOID    void      		
#define CM_DOUBLE  double		
#define CM_FLOAT   float		

/* General purpose return code						*/
#define CM_RET int		

/* We need specific sizes for these types				*/
#define CM_INT8   char     	
#define CM_INT16  short     	
#define CM_INT32  long     	
#define CM_INT64  long long
#define CM_UINT8  unsigned char     	
#define CM_UINT16 unsigned short    	
#define CM_UINT32 unsigned int    	
#define CM_UINT64 unsigned long long
#define CM_BOOLEAN  unsigned char		

/* This define shows that we really have support for 64 bit integers	*/
#define INT64_SUPPORT

/* This define shows that we have supplied all required 		*/
#define _CYSRTYPES_DEFINED

#endif	/* linux	*/

/************************************************************************/
/************************************************************************/
/* Make sure that this module has identified the target system 		*/

/************************************************************************/
#ifdef __cplusplus
}
#endif


#endif /* #ifndef _CYSRTYPES_INCLUDE_ */
