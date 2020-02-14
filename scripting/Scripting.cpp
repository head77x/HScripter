#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <d3d.h>
#include <io.h>
#include <conio.h>
#include <direct.h>

#include "scripting.h"
#include "functions.h"

#define NOFILES		-1

BYTE	buf[1000000];
DWORD	jumpbuf[1000000];
DWORD	labelbuf[1000000];
DWORD	gotobuf[1000000];
char label[100000][FILENAME_MAX], gotos[100000][FILENAME_MAX];
int bufcnt =0, jumpbufcnt = 0, labelcnt = 0, gotocnt = 0, maxjumpbufcnt = 0;
int LineCount = 0;
BOOL event_handler = FALSE;

void SkipRem( FILE *fp );


/*****************************************************************************************************************

	에러 메시지 출력

*****************************************************************************************************************/
void error_process( char *message, FILE *fp )
{
	fclose( fp );

	printf( "ERROR %d LINE: %s \n", LineCount + 1, message );

	_getch();
	while( !_kbhit() );

	exit(0);
}

/*****************************************************************************************************************

	화일에서 한 문자열을 읽어온다.

*****************************************************************************************************************/
int Get_strings( FILE *Fp, char *temp )
{
    char ch;
    int cnt = 0;

again:

    ch = (char)fgetc( Fp );
	if ( ch == EOF )
		return 0;

	// 첫번째 문자가 시작할때까지 반복
    while( ch == ' ' || ch == '\n' || ch == 13 || ch == '\r' || ch == 9 || ch == ';' )
	{
		if ( ch == '\n' )
			LineCount++;

        ch = (char)fgetc( Fp );
	}

	int strmarkcount = 0;

	// 한 문장이 끝날때까지 반복
    while( true )
    {
		if ( ch == '(' || ch == ')' || ch == ' ' || ch == '\n' || ch == 13 || ch == '\r' || ch == 9 || ch == ';' )
			if ( temp[0] != '"' || strmarkcount > 1 )
				break;

		if ( ch == '"' )
			strmarkcount++;


        temp[cnt] = ch;
        ch = (char)fgetc( Fp );
        cnt++;
    }

    fseek( Fp, -1, SEEK_CUR );

    temp[cnt] = 0;

	if ( temp[0] == '/' && temp[1] == '*' )		// 주석이라면.. 뛰어 넘는다.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

/*****************************************************************************************************************

	화일에서 한 문자열을 읽어온다.

*****************************************************************************************************************/
int Get_command( FILE *Fp, char *temp )
{
    char ch;
    int cnt = 0;

again:

    ch = (char)fgetc( Fp );
	if ( ch == EOF )
		return 0;

    while( ch == ' ' || ch == '\n' || ch == 13 || ch == '\r' || ch == 9 || ch == ';')
	{
		if ( ch == '\n' )
			LineCount++;

        ch = (char)fgetc( Fp );
	}

    while( ch != '(' && ch != ')' && ch != ';' && ch != ' ' && ch != '\n' && ch != 13 && ch != '\r' && ch != 9 && ( (ch >= 'a' && ch <= 'z') || (ch>='A' && ch <='Z') || ( ch >= '0' && ch <= '9') || ch == '-' || ch == '_' ) )
    {
        temp[cnt] = ch;
        ch = (char)fgetc( Fp );
        cnt++;
    }

    fseek( Fp, -1, SEEK_CUR );

    temp[cnt] = 0;

	if ( temp[0] == '/' && temp[1] == '*' )		// 주석이라면.. 뛰어 넘는다.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

/*****************************************************************************************************************

	화일에서 한 문자열을 읽어온다.

*****************************************************************************************************************/
int Get_sign( FILE *Fp, char *temp )
{
    char ch;
	int cnt = 0;

again:

    ch = (char)fgetc( Fp );
	if ( ch == EOF )
		return 0;

    while( ch == ' ' || ch == '\n' || ch == 13 || ch == '\r' || ch == 9 )
	{
		if ( ch == '\n' )
			LineCount++;

        ch = (char)fgetc( Fp );
	}

    while( ch != ' ' && ch != '\n' && ch != 13 && ch != '\r' && ch != 9 && ( ch < 'a' || ch > 'z' ) && ( ch < 'A' || ch > 'Z' ) && ( ch < '0' || ch > '9' ) )
    {
        temp[cnt] = ch;
        ch = (char)fgetc( Fp );
        cnt++;
    }

    fseek( Fp, -1, SEEK_CUR );

    temp[cnt] = 0;

	if ( temp[0] == '/' && temp[1] == '*' )		// 주석이라면.. 뛰어 넘는다.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			if 명령을 컴파일 하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void if_process( FILE *fp )
{
	char	temp[1000];
	int		varnum = 0;

	while( 1 )
	{
		buf[bufcnt] = IF_COMMAND;		// IF 커맨드임
		bufcnt++;

		Get_sign( fp, temp );		

		if ( temp[0] != '(' )
			error_process( "if 명령 오류 -> '(' 표시가 없음", fp );					// 변수가 없다면


		// if 뒤의 변수를 읽어들임
		Get_command( fp, temp );		

		if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 지역변수 번호가 이상함", fp );

			buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
		if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 지역변수 번호가 이상함", fp );

			buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
		if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 전역변수 번호가 이상함", fp );

			buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
			error_process( "if 명령 오류 -> 변수명이 잘못되었음( lvar 또는 gvar 로 시작해야함 )", fp );					// 변수가 없다면


		// 변수 뒤의 부호 처리
		Get_sign( fp, temp );	

		if ( temp[0] == '=' )		// = 표시라면, 같다는 표시		
		{
			buf[bufcnt] = EQUAL_SIGN;			// 같다는 표시
			bufcnt++;
		}
		else
		if ( temp[0] == '!' )		// ! 표시라면, 틀리다는 표시		
		{
			buf[bufcnt] = NOTEQUAL_SIGN;			// 틀리다는 표시
			bufcnt++;
		}
		else
		if ( temp[0] == '>'  )		// > 표시라면, 크다는 표시		
		{
			buf[bufcnt] = GREAT_SIGN;			// 크다는 표시
			bufcnt++;
		}
		else
		if ( temp[0] == '<' )		// < 표시라면, 작다는 표시		
		{
			buf[bufcnt] = SMALL_SIGN;			// 작다는 표시
			bufcnt++;
		}
		else
			error_process( "if 명령 오류 -> 부호가 잘못되었음" , fp );

		
		// 부호뒤의 변수/값을 읽어들임
		Get_command( fp, temp );		

		if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 지역변수 번호가 이상함", fp );

			buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
		if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

			buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
		if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if 명령 오류 -> 부호뒤의 전역변수 번호가 이상함", fp );

			buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
			bufcnt++;

			buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
			bufcnt++;
		}
		else
		if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  숫자라면...
		{
			varnum = atoi( temp );

			buf[bufcnt] = VALUE_VAR;		// 수치임
			bufcnt++;

			buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// 수치 넣어줌 상위
			bufcnt++;

			buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// 수치 넣어줌 하위
			bufcnt++;

			buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// 수치 넣어줌 하위
			bufcnt++;

			buf[bufcnt] = (BYTE)(varnum & 0xff);		// 수치 넣어줌 하위
			bufcnt++;
		}
		else
			error_process( "if 명령 오류 -> 부호뒤의 변수명이 잘못되었음( lvar 또는 gvar 로 시작해야함 )", fp );					// 변수가 없다면

		Get_sign( fp, temp );		

		if ( temp[0] != ')' )
			error_process( "if 명령 오류 -> ')' 표시가 없음", fp );					// 변수가 없다면

		Get_sign( fp, temp );

		if ( temp[0] != '{' )
			error_process( "if 명령 오류 -> '{' 표시가 없음", fp );					// 변수가 없다면


		// 조건문이 만족하지 못했을 때 건너뛸 곳
		jumpbuf[jumpbufcnt] = bufcnt;
		jumpbufcnt++;
		bufcnt+=4;				// 레이블 및 점핑 위치는 2Byte

		break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			goto 명령을 컴파일 하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void goto_process( FILE *fp )
{
	char	temp[1000];
	int		varnum = 0, i;

	while( 1 )
	{
		buf[bufcnt] = GOTO_COMMAND;		// goto 커맨드임
		bufcnt++;

		// if 뒤의 변수를 읽어들임
		Get_command( fp, temp );		// 뒤의 레이블명을 읽어 들임

		for( i = 0; i < labelcnt; i++ )
		{
			if ( !stricmp( temp, label[i] ) )		// 레이블이 이미 있다면..
			{
				buf[bufcnt] = (BYTE)(labelbuf[i] >> 24)&0xff;
				buf[bufcnt+1] = (BYTE)(labelbuf[i] >> 16)&0xff;
				buf[bufcnt+2] = (BYTE)(labelbuf[i] >> 8)&0xff;
				buf[bufcnt+3] = (BYTE)(labelbuf[i] & 0xff);
				bufcnt+=4;
				return;
			}
		}
		
		sprintf( gotos[gotocnt], "%s", temp );		// 만약 레이블이 없다면..
		gotobuf[gotocnt] = bufcnt;
		gotocnt++;

		bufcnt+=4;						// 레이블 및 점핑 위치는 2Byte
		
		break;
	}
}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			label 을 컴파일 하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Check_label( char *temp, FILE *fp )
{
	int i, j;

	if ( temp[strlen(temp)-1] != ':' )
		return FALSE;

	temp[strlen(temp)-1] = NULL;
	sprintf( label[labelcnt], "%s", temp );
	labelbuf[labelcnt] = bufcnt;
	labelcnt++;

	for ( j = 0; j < gotocnt; j++ )
	{
		for( i = 0; i < labelcnt; i++ )
		{
			if ( !stricmp( gotos[j], label[i] ) )		// 레이블이 이미 있다면..
			{
				buf[gotobuf[j]] = (BYTE)(labelbuf[i] >> 24)&0xff;
				buf[gotobuf[j]+1] = (BYTE)(labelbuf[i] >> 16)&0xff;
				buf[gotobuf[j]+2] = (BYTE)(labelbuf[i] >> 8)&0xff;
				buf[gotobuf[j]+3] = (BYTE)(labelbuf[i] & 0xff);
			}
		}
	}

	return TRUE;
}
 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		변수명인지 체크하는 부분 ( 대입 또는 연산 처리 )
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Check_var( char *temp, FILE *fp )
{
	int	varnum , i;
	int vartype = 0;

	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		vartype = 1;		// 문자열 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면 대입문
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면 대입문
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
		return FALSE;   // 대입문이 아니라면



	Get_sign( fp, temp );		// 그 다음이 = 이 맞는지 체크

	if ( temp[0] != '=' )
		error_process( "대입 명령 오류 -> '=' 표시가 없음", fp );					// 변수가 없다면

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// 문자열이라면..
	{
		if ( !vartype )
			error_process( "대입 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		buf[bufcnt] = STRING_VALUE;		// 로컬변수임을 넣어줌
		bufcnt++;

		if ( strlen(temp) - 2 > FILENAME_MAX )
			error_process( "대입 명령 오류 -> 문자열이 너무 길다", fp );					

		buf[bufcnt] = strlen(temp) - 2;
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		if ( !vartype )
			error_process( "대입 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  숫자라면...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// 수치임
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// 수치 넣어줌 상위
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// 수치 넣어줌 하위
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// 수치 넣어줌 하위
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// 수치 넣어줌 하위
		bufcnt++;

	}
	else
		error_process( "대입 명령 오류 -> 부호뒤의 변수명또는 수치가 잘못되었음", fp );					// 변수가 없다면


	Get_sign( fp, temp );		// 그 다음이 ;인지 아니면 연산인지 체크

	if ( temp[0] == ';' )
	{
		buf[bufcnt] = END_CAL;
		bufcnt++;
		return TRUE;
	}

	if ( temp[0] == '+' )		// 덧셈
	{
		buf[bufcnt] = PLUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '-' )		// 빼기
	{
		if ( vartype )		// 문자열은 대입 또는 덧셈만 가능
			error_process( "연산 명령 오류 -> 문자열끼리는 연산 불가", fp );

		buf[bufcnt] = MINUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '*' )		// 곱하기
	{
		if ( vartype )		// 문자열은 대입 또는 덧셈만 가능
			error_process( "연산 명령 오류 -> 문자열끼리는 연산 불가", fp );

		buf[bufcnt] = MULTI_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '/' )		// 나누기
	{
		if ( vartype )		// 문자열은 대입 또는 덧셈만 가능
			error_process( "연산 명령 오류 -> 문자열끼리는 연산 불가", fp );

		buf[bufcnt] = DIV_CAL;
		bufcnt++;
	}
	else
		error_process( "연산 명령 오류 -> 연산 기호가 잘못되었음", fp );

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// 문자열이라면..
	{
		if ( !vartype )
			error_process( "연산 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		buf[bufcnt] = STRING_VALUE;		// 로컬변수임을 넣어줌
		bufcnt++;

		if ( strlen(temp) - 2 > FILENAME_MAX )
			error_process( "대입 명령 오류 -> 문자열이 너무 길다", fp );					

		buf[bufcnt] = strlen(temp) - 2;
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		if ( !vartype )
			error_process( "연산 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  숫자라면...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// 수치임
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// 수치 넣어줌 상위
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// 수치 넣어줌 하위
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// 수치 넣어줌 하위
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// 수치 넣어줌 하위
		bufcnt++;
	}
	else
		error_process( "연산 명령 오류 -> 부호뒤의 변수명또는 수치가 잘못되었음", fp );					// 변수가 없다면

	return TRUE;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		함수명인지 체크하는 부분 ( 대입 또는 연산 처리 )
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Check_function( char *temp, FILE *fp )
{
	int	varnum , i;
	int vartype = 0;

	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		vartype = 1;		// 문자열 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면 대입문
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면 대입문
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
		return FALSE;   // 대입문이 아니라면



	Get_sign( fp, temp );		// 그 다음이 = 이 맞는지 체크

	if ( temp[0] != '=' )
		error_process( "대입 명령 오류 -> '=' 표시가 없음", fp );					// 변수가 없다면

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// 문자열이라면..
	{
		if ( !vartype )
			error_process( "대입 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		buf[bufcnt] = STRING_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		if ( !vartype )
			error_process( "대입 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  숫자라면...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// 수치임
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 8);			// 수치 넣어줌 상위
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// 수치 넣어줌 하위
		bufcnt++;
	}
	else
		error_process( "대입 명령 오류 -> 부호뒤의 변수명또는 수치가 잘못되었음", fp );					// 변수가 없다면


	Get_sign( fp, temp );		// 그 다음이 ;인지 아니면 연산인지 체크

	if ( temp[0] == ';' )
		return TRUE;

	if ( temp[0] == '+' )		// 덧셈
	{
		buf[bufcnt] = PLUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '-' )		// 빼기
	{
		buf[bufcnt] = MINUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '*' )		// 곱하기
	{
		buf[bufcnt] = MULTI_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '/' )		// 나누기
	{
		buf[bufcnt] = DIV_CAL;
		bufcnt++;
	}
	else
		error_process( "연산 명령 오류 -> 연산 기호가 잘못되었음", fp );

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// 문자열이라면..
	{
		if ( !vartype )
			error_process( "연산 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		buf[bufcnt] = STRING_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// 스크립트 변수라면
	{
		vartype = 0;		// 보통 변수임
		buf[bufcnt] = MOVE_COMMAND;		// 대입문임을 넣어줌
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = SCRIPT_VAR;		// 스크립트변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		if ( !vartype )
			error_process( "연산 명령 오류 -> 저장할 장소가 문자열 변수가 아님 ( svar 이어야 함 )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = STRING_VAR;		// 문자열변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 로컬 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 지역변수 번호가 이상함", fp );

		buf[bufcnt] = LOCAL_VAR;		// 로컬변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// 글로벌 변수라면
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "대입 명령 오류 -> 부호뒤의 전역변수 번호가 이상함", fp );

		buf[bufcnt] = GLOBAL_VAR;		// 전역변수임을 넣어줌
		bufcnt++;

		buf[bufcnt] = varnum;			// 변수의 번호를 넣어줌
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  숫자라면...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// 수치임
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 8);			// 수치 넣어줌 상위
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// 수치 넣어줌 하위
		bufcnt++;
	}
	else
		error_process( "연산 명령 오류 -> 부호뒤의 변수명또는 수치가 잘못되었음", fp );					// 변수가 없다면

	return TRUE;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		주석 건너쒸는 부분( /*  */ 로 막힌 부분 )
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SkipRem( FILE *fp )
{
	char	temp[1000];

	while( 1 )							
	{
		Get_strings( fp, temp );

		if ( temp[0] == '*' && temp[1] == '/' )
			return;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		이벤트 처리하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	Event_check( char *temp, FILE *fp )
{
	if ( !strcmp( temp, "OnInitial" ) )			// 시작 이벤트 부분이라면
	{
		buf[0] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[1] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[2] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[3] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnClicking" ) )		// 클릭시 이벤트 부분이라면
	{
		buf[4] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[5] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[6] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[7] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnDraging" ) )			// 아이템 드래그시 이벤트 부분이라면
	{
		buf[8] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[9] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[10] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[11] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnHitting" ) )			// 때리는 이벤트 부분이라면
	{
		buf[12] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[13] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[14] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[15] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnKilling" ) )			// 죽이는 이벤트 부분이라면
	{
		buf[16] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[17] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[18] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[19] = (BYTE)(bufcnt & 0xff);
	}
	else
/*	if ( !strcmp( temp, "OnChangeQuest" ) )		// 퀘스트 변경 부분이라면.. ( 주로 퀘스트에 따른 NPC의 위치 )
	{
		buf[20] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[21] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[22] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[23] = (BYTE)(bufcnt & 0xff);
	}
	else
*/
		return FALSE;

	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		스크립트 컴파일 하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	Compile( char *filename, char *savename )
{
	FILE *fp;
	char	temp[1000];

	fp = fopen( filename, "rt" );

	bufcnt = 24;
	LineCount = 0;
	jumpbufcnt = 0;
	gotocnt = 0;
	labelcnt = 0;
	event_handler = FALSE;

	while( 1 )
	{
		Get_strings( fp, temp );

		if ( temp[0] == '/' && temp[1] == '*' )		// 주석이라면.. 뛰어 넘는다.
		{
			SkipRem( fp );
			continue;
		}

		if ( temp[0] == '{' )			// 제일 처음은 무조건 { 로 시작하므로..
			continue;

		if ( temp[0] == '}' )			// 구문이 끝나는 곳이라면
		{
			if ( jumpbufcnt )			// if 문 등의 끝부분이라면, 주소를 입력 시킴
			{
				jumpbufcnt--;			// 바로 전의 구문의 끝이라는 것을 표시
				buf[jumpbuf[jumpbufcnt]] = (BYTE)(bufcnt >> 24)&0xff;
				buf[jumpbuf[jumpbufcnt] + 1] = (BYTE)(bufcnt >> 16)&0xff;
				buf[jumpbuf[jumpbufcnt] + 2] = (BYTE)(bufcnt >> 8)&0xff;
				buf[jumpbuf[jumpbufcnt] + 3] = (BYTE)(bufcnt & 0xff);

				continue;
			}
			else
			{
				buf[bufcnt] = END_COMMAND;		// 완전히 끝난 구문이라면..
				bufcnt++;


				if ( !event_handler )			// 이벤트과 관련된 구문이 아니라면. 마지막으로 끝
					break;
				else
				{
					event_handler = FALSE;		// 이벤트 끝 구문이라면, 다음 이벤트
					continue;
				}
			}
		}

		if ( !strcmp( temp, "if" ) )			// 명령어 처리 부분
		{
			if_process( fp );
			continue;
		}
		else
		if ( !strcmp( temp, "goto" ) )			
		{
			goto_process( fp );
			continue;
		}
		else
			if ( !Check_var( temp, fp ) )		// 대입명령이 아닌지 처리
			{
				if ( !Event_check( temp, fp ) )	// 이벤트 처리부분인지 체크
				{
					if ( !Check_function( temp, fp ) )	// 함수 처리부분 체크
						if ( !Check_label( temp, fp ) )
							error_process( "Command Error -> 잘못된 명령 또는 함수입니다.", fp );
				}
				else
					event_handler = TRUE;
			}
	}

	fclose( fp );

	sprintf( temp, ".\\binary\\%s", savename );

	fp = fopen( temp, "wb" );

	fwrite( &bufcnt, 1, 4, fp );

	fwrite( buf, 1, bufcnt, fp );

	fclose( fp );

	return TRUE;
}


void main( void )
{
  	_finddata_t fileinfo;

	char	start = 0;
	long	handle, TickCount;
	char	Filename[13] = "*.scr";
	int first = 0;
	int	filecnt = 0;
	int cnt = 0;
	int size;
	char	temp[1000];
	char	temp2[1000];

	int i;
	FILE *fp;

	printf( "Script Compiler Version 1.04 ( 2008. 12. 5. 23:10 )\n\n" );
	printf( "Press Any Key !\n" );

	while( !kbhit() ){}; 
	_getch();
 
	if( (handle = _findfirst( Filename, &fileinfo ) ) == NOFILES )
	{
		printf( "------------- Script Source Files Not Found !!! -----------------\n\n");
		while( !_kbhit() );

		exit(0);
	}


	while( 1 )
	{
		printf( "-------- %s file found.....\n\n", fileinfo.name );

		printf( "-------- Now Compiling --------> %s         \n\n", fileinfo.name );


		strncpy( temp2, fileinfo.name, strlen( fileinfo.name ) - strlen( strrchr( fileinfo.name, '.' ) ) );
		temp2[ strlen( fileinfo.name ) - strlen( strrchr( fileinfo.name, '.' ) ) ] = NULL;
		strcat( temp2, ".bin" );

		memset( buf, 0, sizeof( buf ) );

		if ( Compile( fileinfo.name, temp2 ) )
		{
			printf( "--------------------- %s Compile Complete %d bytes %d jumps<-------------     \n\n", fileinfo.name, bufcnt, maxjumpbufcnt );
		}
		else
		{
			printf( "-------------- %s Compile Error !!!! -------------------- \n", fileinfo.name );
			while( !_kbhit() );

			exit( 1 );
		}


		if( (_findnext( handle, &fileinfo ) ) == NOFILES )
			break;
	}

	printf( "No Errors ...................... no errors complete .........................\n" );
	printf( "Press Any Key !\n" );

	while( !kbhit() ){}; 
	_getch();
}





