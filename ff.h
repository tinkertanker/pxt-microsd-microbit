#ifndef _FATFS
#define _FATFS	64180	
#ifdef __cplusplus
extern "C" {
#endif
#include "integer.h"	
#include "ffconf.h"		
#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif
#if _MULTI_PARTITION		
typedef struct {
	BYTE pd;	
	BYTE pt;	
} PARTITION;
extern PARTITION VolToPart[];	
#define LD2PD(vol) (VolToPart[vol].pd)	
#define LD2PT(vol) (VolToPart[vol].pt)	
#else							
#define LD2PD(vol) (BYTE)(vol)	
#define LD2PT(vol) 0			
#endif
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
typedef struct {
	BYTE	fs_type;		
	BYTE	drv;			
	BYTE	csize;			
	BYTE	n_fats;			
	BYTE	wflag;			
	BYTE	fsi_flag;		
	WORD	id;				
	WORD	n_rootdir;
	DWORD	last_clust;		
	DWORD	free_clust;
	DWORD	n_fatent;		
	DWORD	fsize;			
	DWORD	volbase;		
	DWORD	fatbase;		
	DWORD	dirbase;		
	DWORD	database;		
	DWORD	winsect;		
	BYTE	win[_MAX_SS];	
} FATFS;
typedef struct {
	FATFS*	fs;				
	WORD	id;				
	BYTE	flag;			
	BYTE	err;			
	DWORD	fptr;			
	DWORD	fsize;			
	DWORD	sclust;			
	DWORD	clust;			
	DWORD	dsect;	
	DWORD	dir_sect;		
	BYTE*	dir_ptr;
} FIL;
typedef struct {
	FATFS*	fs;				
	WORD	id;				
	WORD	index;			
	DWORD	sclust;			
	DWORD	clust;			
	DWORD	sect;			
	BYTE*	dir;			
	BYTE*	fn;	
	WCHAR*	lfn;			
	WORD	lfn_idx;
} FATFS_DIR;
typedef struct {
	DWORD	fsize;			
	WORD	fdate;			
	WORD	ftime;			
	BYTE	fattrib;		
	char	fname[13];
	char*	lfname;			
	UINT 	lfsize;
} FILINFO;
typedef enum {
	FR_OK = 0,				
	FR_DISK_ERR,			
	FR_INT_ERR,				
	FR_NOT_READY,			
	FR_NO_FILE,				
	FR_NO_PATH,				
	FR_INVALID_NAME,		
	FR_DENIED,				
	FR_EXIST,				
	FR_INVALID_OBJECT,		
	FR_WRITE_PROTECTED,		
	FR_INVALID_DRIVE,		
	FR_NOT_ENABLED,			
	FR_NO_FILESYSTEM,		
	FR_MKFS_ABORTED,		
	FR_TIMEOUT,				
	FR_LOCKED,				
	FR_NOT_ENOUGH_CORE,		
	FR_TOO_MANY_OPEN_FILES,	
	FR_INVALID_PARAMETER	
} FRESULT;
FRESULT f_open (FIL* fp, const char* path, BYTE mode);				
FRESULT f_close (FIL* fp);											
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	
FRESULT f_lseek (FIL* fp, DWORD ofs);								
FRESULT f_truncate (FIL* fp);										
FRESULT f_sync (FIL* fp);											
FRESULT f_opendir (FATFS_DIR* dp, const char* path);						
FRESULT f_closedir (FATFS_DIR* dp);										
FRESULT f_readdir (FATFS_DIR* dp, FILINFO* fno);							
FRESULT f_findfirst (FATFS_DIR* dp, FILINFO* fno, const char* path, const char* pattern);	
FRESULT f_findnext (FATFS_DIR* dp, FILINFO* fno);							
FRESULT f_mkdir (const char* path);								
FRESULT f_unlink (const char* path);								
FRESULT f_rename (const char* path_old, const char* path_new);	
FRESULT f_stat (const char* path, FILINFO* fno);					
FRESULT f_chmod (const char* path, BYTE attr, BYTE mask);			
FRESULT f_utime (const char* path, const FILINFO* fno);			
FRESULT f_chdir (const char* path);								
FRESULT f_chdrive (const char* path);								
FRESULT f_getcwd (char* buff, UINT len);							
FRESULT f_getfree (const char* path, DWORD* nclst, FATFS** fatfs);	
FRESULT f_getlabel (const char* path, char* label, DWORD* vsn);	
FRESULT f_setlabel (const char* label);							
FRESULT f_mount (FATFS* fs, const char* path, BYTE opt);			
FRESULT f_mkfs (const char* path, BYTE sfd, UINT au);				
FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);			
int f_putc (char c, FIL* fp);										
int f_puts (const char* str, FIL* cp);								
int f_printf (FIL* fp, const char* str, ...);						
char* f_gets (char* buff, int len, FIL* fp);						
#define f_eof(fp) ((int)((fp)->fptr == (fp)->fsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->fsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#ifndef EOF
#define EOF (-1)
#endif
#if !_FS_READONLY && !_FS_NORTC
DWORD get_fattime (void);
#endif
#if _USE_LFN							
WCHAR ff_convert (WCHAR chr, UINT dir);	
WCHAR ff_wtoupper (WCHAR chr);			
#if _USE_LFN == 3						
void* ff_memalloc (UINT msize);			
void ff_memfree (void* mblock);			
#endif
#endif
#if _FS_REENTRANT
int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj);	
int ff_req_grant (_SYNC_t sobj);				
void ff_rel_grant (_SYNC_t sobj);				
int ff_del_syncobj (_SYNC_t sobj);				
#endif
#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define	AM_RDO	0x01	
#define	AM_HID	0x02	
#define	AM_SYS	0x04	
#define	AM_VOL	0x08	
#define AM_LFN	0x0F	
#define AM_DIR	0x10	
#define AM_ARC	0x20	
#define AM_MASK	0x3F	
#define CREATE_LINKMAP	0xFFFFFFFF				
#define	LD_WORD(ptr)		(WORD)(((WORD)*((BYTE*)(ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(DWORD)(((DWORD)*((BYTE*)(ptr)+3)<<24)|((DWORD)*((BYTE*)(ptr)+2)<<16)|((WORD)*((BYTE*)(ptr)+1)<<8)|*(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8)
#define	ST_DWORD(ptr,val)	*(BYTE*)(ptr)=(BYTE)(val); *((BYTE*)(ptr)+1)=(BYTE)((WORD)(val)>>8); *((BYTE*)(ptr)+2)=(BYTE)((DWORD)(val)>>16); *((BYTE*)(ptr)+3)=(BYTE)((DWORD)(val)>>24)
#ifdef __cplusplus
}
#endif
#endif 
