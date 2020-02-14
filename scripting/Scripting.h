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


#define LOCAL_VAR	0x50		// 지역변수
#define GLOBAL_VAR	0x51		// 전역변수
#define VALUE_VAR	0x52		// 수치
#define STRING_VAR	0x53		// 문자열변수
#define SCRIPT_VAR	0x54		// 스크립트 변수
#define STRING_VALUE	0x55	// 문자열

#define PLUS_CAL	0x60		// 더하기
#define MINUS_CAL	0x61		// 빼기
#define MULTI_CAL	0x62		// 곱하기
#define DIV_CAL		0x63		// 나누기
#define END_CAL		0x67		// 대입만 함

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