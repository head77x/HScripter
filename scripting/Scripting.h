#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <d3d.h>
#include <io.h>
#include <conio.h>
#include <direct.h>

#ifndef __SCRIPTER__
#define __SCRIPTER__

#define IF_COMMAND	0x30		
#define GOTO_COMMAND	0x31
#define MOVE_COMMAND	0x32
#define END_COMMAND	0x33
#define FUNCTION_COMMAND	0x34


#define LOCAL_VAR	0x50		// ��������
#define GLOBAL_VAR	0x51		// ��������
#define VALUE_VAR	0x52		// ��ġ
#define STRING_VAR	0x53		// ���ڿ�����
#define SCRIPT_VAR	0x54		// ��ũ��Ʈ ����
#define STRING_VALUE	0x55	// ���ڿ�

#define PLUS_CAL	0x60		// ���ϱ�
#define MINUS_CAL	0x61		// ����
#define MULTI_CAL	0x62		// ���ϱ�
#define DIV_CAL		0x63		// ������
#define END_CAL		0x67		// ���Ը� ��

#define EQUAL_SIGN	0x80		// '='
#define GREAT_SIGN  0x81		// '>'
#define SMALL_SIGN	0x82		// '<'
#define NOTEQUAL_SIGN	0x83	// '!='

extern BYTE	buf[1000000];
extern DWORD	jumpbuf[1000000];
extern DWORD	labelbuf[1000000];
extern char label[100000][FILENAME_MAX];
extern int bufcnt, jumpbufcnt, labelcnt;
extern int LineCount;
extern BOOL event_handler;

extern void error_process( char *message, FILE *fp );
extern int Get_strings( FILE *Fp, char *temp );
extern int Get_command( FILE *Fp, char *temp );
extern int Get_sign( FILE *Fp, char *temp );

#endif