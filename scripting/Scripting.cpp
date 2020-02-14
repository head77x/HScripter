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

	���� �޽��� ���

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

	ȭ�Ͽ��� �� ���ڿ��� �о�´�.

*****************************************************************************************************************/
int Get_strings( FILE *Fp, char *temp )
{
    char ch;
    int cnt = 0;

again:

    ch = (char)fgetc( Fp );
	if ( ch == EOF )
		return 0;

	// ù��° ���ڰ� �����Ҷ����� �ݺ�
    while( ch == ' ' || ch == '\n' || ch == 13 || ch == '\r' || ch == 9 || ch == ';' )
	{
		if ( ch == '\n' )
			LineCount++;

        ch = (char)fgetc( Fp );
	}

	int strmarkcount = 0;

	// �� ������ ���������� �ݺ�
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

	if ( temp[0] == '/' && temp[1] == '*' )		// �ּ��̶��.. �پ� �Ѵ´�.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

/*****************************************************************************************************************

	ȭ�Ͽ��� �� ���ڿ��� �о�´�.

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

	if ( temp[0] == '/' && temp[1] == '*' )		// �ּ��̶��.. �پ� �Ѵ´�.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

/*****************************************************************************************************************

	ȭ�Ͽ��� �� ���ڿ��� �о�´�.

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

	if ( temp[0] == '/' && temp[1] == '*' )		// �ּ��̶��.. �پ� �Ѵ´�.
	{
		SkipRem( Fp );
		cnt = 0;
		goto again;
	}

    return( cnt );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			if ����� ������ �ϴ� �κ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void if_process( FILE *fp )
{
	char	temp[1000];
	int		varnum = 0;

	while( 1 )
	{
		buf[bufcnt] = IF_COMMAND;		// IF Ŀ�ǵ���
		bufcnt++;

		Get_sign( fp, temp );		

		if ( temp[0] != '(' )
			error_process( "if ��� ���� -> '(' ǥ�ð� ����", fp );					// ������ ���ٸ�


		// if ���� ������ �о����
		Get_command( fp, temp );		

		if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
		if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
		if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
			error_process( "if ��� ���� -> �������� �߸��Ǿ���( lvar �Ǵ� gvar �� �����ؾ��� )", fp );					// ������ ���ٸ�


		// ���� ���� ��ȣ ó��
		Get_sign( fp, temp );	

		if ( temp[0] == '=' )		// = ǥ�ö��, ���ٴ� ǥ��		
		{
			buf[bufcnt] = EQUAL_SIGN;			// ���ٴ� ǥ��
			bufcnt++;
		}
		else
		if ( temp[0] == '!' )		// ! ǥ�ö��, Ʋ���ٴ� ǥ��		
		{
			buf[bufcnt] = NOTEQUAL_SIGN;			// Ʋ���ٴ� ǥ��
			bufcnt++;
		}
		else
		if ( temp[0] == '>'  )		// > ǥ�ö��, ũ�ٴ� ǥ��		
		{
			buf[bufcnt] = GREAT_SIGN;			// ũ�ٴ� ǥ��
			bufcnt++;
		}
		else
		if ( temp[0] == '<' )		// < ǥ�ö��, �۴ٴ� ǥ��		
		{
			buf[bufcnt] = SMALL_SIGN;			// �۴ٴ� ǥ��
			bufcnt++;
		}
		else
			error_process( "if ��� ���� -> ��ȣ�� �߸��Ǿ���" , fp );

		
		// ��ȣ���� ����/���� �о����
		Get_command( fp, temp );		

		if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
		if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
		if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
		{
			varnum = atoi( &temp[4] );

			if ( varnum < 1 )
				error_process( "if ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

			buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
			bufcnt++;

			buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
			bufcnt++;
		}
		else
		if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  ���ڶ��...
		{
			varnum = atoi( temp );

			buf[bufcnt] = VALUE_VAR;		// ��ġ��
			bufcnt++;

			buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// ��ġ �־��� ����
			bufcnt++;

			buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// ��ġ �־��� ����
			bufcnt++;

			buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// ��ġ �־��� ����
			bufcnt++;

			buf[bufcnt] = (BYTE)(varnum & 0xff);		// ��ġ �־��� ����
			bufcnt++;
		}
		else
			error_process( "if ��� ���� -> ��ȣ���� �������� �߸��Ǿ���( lvar �Ǵ� gvar �� �����ؾ��� )", fp );					// ������ ���ٸ�

		Get_sign( fp, temp );		

		if ( temp[0] != ')' )
			error_process( "if ��� ���� -> ')' ǥ�ð� ����", fp );					// ������ ���ٸ�

		Get_sign( fp, temp );

		if ( temp[0] != '{' )
			error_process( "if ��� ���� -> '{' ǥ�ð� ����", fp );					// ������ ���ٸ�


		// ���ǹ��� �������� ������ �� �ǳʶ� ��
		jumpbuf[jumpbufcnt] = bufcnt;
		jumpbufcnt++;
		bufcnt+=4;				// ���̺� �� ���� ��ġ�� 2Byte

		break;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			goto ����� ������ �ϴ� �κ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void goto_process( FILE *fp )
{
	char	temp[1000];
	int		varnum = 0, i;

	while( 1 )
	{
		buf[bufcnt] = GOTO_COMMAND;		// goto Ŀ�ǵ���
		bufcnt++;

		// if ���� ������ �о����
		Get_command( fp, temp );		// ���� ���̺���� �о� ����

		for( i = 0; i < labelcnt; i++ )
		{
			if ( !stricmp( temp, label[i] ) )		// ���̺��� �̹� �ִٸ�..
			{
				buf[bufcnt] = (BYTE)(labelbuf[i] >> 24)&0xff;
				buf[bufcnt+1] = (BYTE)(labelbuf[i] >> 16)&0xff;
				buf[bufcnt+2] = (BYTE)(labelbuf[i] >> 8)&0xff;
				buf[bufcnt+3] = (BYTE)(labelbuf[i] & 0xff);
				bufcnt+=4;
				return;
			}
		}
		
		sprintf( gotos[gotocnt], "%s", temp );		// ���� ���̺��� ���ٸ�..
		gotobuf[gotocnt] = bufcnt;
		gotocnt++;

		bufcnt+=4;						// ���̺� �� ���� ��ġ�� 2Byte
		
		break;
	}
}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//			label �� ������ �ϴ� �κ�
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
			if ( !stricmp( gotos[j], label[i] ) )		// ���̺��� �̹� �ִٸ�..
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
//		���������� üũ�ϴ� �κ� ( ���� �Ǵ� ���� ó�� )
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Check_var( char *temp, FILE *fp )
{
	int	varnum , i;
	int vartype = 0;

	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		vartype = 1;		// ���ڿ� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� ������� ���Թ�
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� ������� ���Թ�
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
		return FALSE;   // ���Թ��� �ƴ϶��



	Get_sign( fp, temp );		// �� ������ = �� �´��� üũ

	if ( temp[0] != '=' )
		error_process( "���� ��� ���� -> '=' ǥ�ð� ����", fp );					// ������ ���ٸ�

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// ���ڿ��̶��..
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		buf[bufcnt] = STRING_VALUE;		// ���ú������� �־���
		bufcnt++;

		if ( strlen(temp) - 2 > FILENAME_MAX )
			error_process( "���� ��� ���� -> ���ڿ��� �ʹ� ���", fp );					

		buf[bufcnt] = strlen(temp) - 2;
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  ���ڶ��...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// ��ġ��
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// ��ġ �־��� ����
		bufcnt++;

	}
	else
		error_process( "���� ��� ���� -> ��ȣ���� ������Ǵ� ��ġ�� �߸��Ǿ���", fp );					// ������ ���ٸ�


	Get_sign( fp, temp );		// �� ������ ;���� �ƴϸ� �������� üũ

	if ( temp[0] == ';' )
	{
		buf[bufcnt] = END_CAL;
		bufcnt++;
		return TRUE;
	}

	if ( temp[0] == '+' )		// ����
	{
		buf[bufcnt] = PLUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '-' )		// ����
	{
		if ( vartype )		// ���ڿ��� ���� �Ǵ� ������ ����
			error_process( "���� ��� ���� -> ���ڿ������� ���� �Ұ�", fp );

		buf[bufcnt] = MINUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '*' )		// ���ϱ�
	{
		if ( vartype )		// ���ڿ��� ���� �Ǵ� ������ ����
			error_process( "���� ��� ���� -> ���ڿ������� ���� �Ұ�", fp );

		buf[bufcnt] = MULTI_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '/' )		// ������
	{
		if ( vartype )		// ���ڿ��� ���� �Ǵ� ������ ����
			error_process( "���� ��� ���� -> ���ڿ������� ���� �Ұ�", fp );

		buf[bufcnt] = DIV_CAL;
		bufcnt++;
	}
	else
		error_process( "���� ��� ���� -> ���� ��ȣ�� �߸��Ǿ���", fp );

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// ���ڿ��̶��..
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		buf[bufcnt] = STRING_VALUE;		// ���ú������� �־���
		bufcnt++;

		if ( strlen(temp) - 2 > FILENAME_MAX )
			error_process( "���� ��� ���� -> ���ڿ��� �ʹ� ���", fp );					

		buf[bufcnt] = strlen(temp) - 2;
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 || varnum > 255 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  ���ڶ��...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// ��ġ��
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 24) & 0xff;			// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 16) & 0xff);		// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)((varnum >> 8) & 0xff);		// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// ��ġ �־��� ����
		bufcnt++;
	}
	else
		error_process( "���� ��� ���� -> ��ȣ���� ������Ǵ� ��ġ�� �߸��Ǿ���", fp );					// ������ ���ٸ�

	return TRUE;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		�Լ������� üũ�ϴ� �κ� ( ���� �Ǵ� ���� ó�� )
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Check_function( char *temp, FILE *fp )
{
	int	varnum , i;
	int vartype = 0;

	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		vartype = 1;		// ���ڿ� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� ������� ���Թ�
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� ������� ���Թ�
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
		return FALSE;   // ���Թ��� �ƴ϶��



	Get_sign( fp, temp );		// �� ������ = �� �´��� üũ

	if ( temp[0] != '=' )
		error_process( "���� ��� ���� -> '=' ǥ�ð� ����", fp );					// ������ ���ٸ�

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// ���ڿ��̶��..
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		buf[bufcnt] = STRING_VAR;		// ���ú������� �־���
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  ���ڶ��...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// ��ġ��
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 8);			// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// ��ġ �־��� ����
		bufcnt++;
	}
	else
		error_process( "���� ��� ���� -> ��ȣ���� ������Ǵ� ��ġ�� �߸��Ǿ���", fp );					// ������ ���ٸ�


	Get_sign( fp, temp );		// �� ������ ;���� �ƴϸ� �������� üũ

	if ( temp[0] == ';' )
		return TRUE;

	if ( temp[0] == '+' )		// ����
	{
		buf[bufcnt] = PLUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '-' )		// ����
	{
		buf[bufcnt] = MINUS_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '*' )		// ���ϱ�
	{
		buf[bufcnt] = MULTI_CAL;
		bufcnt++;
	}
	else
	if ( temp[0] == '/' )		// ������
	{
		buf[bufcnt] = DIV_CAL;
		bufcnt++;
	}
	else
		error_process( "���� ��� ���� -> ���� ��ȣ�� �߸��Ǿ���", fp );

	Get_strings( fp, temp );

	if ( temp[0] == '"' )		// ���ڿ��̶��..
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		buf[bufcnt] = STRING_VAR;		// ���ú������� �־���
		bufcnt++;

		for ( i = 0; i < strlen(temp) - 2; i++ )
		{
			buf[bufcnt] = temp[i+1] ^ 0x3c;
			bufcnt++;
		}
	}
	else
	if ( temp[0] == 's' && temp[1] == 'c' && temp[2] == 'r' && temp[3] == 'i' )		// ��ũ��Ʈ �������
	{
		vartype = 0;		// ���� ������
		buf[bufcnt] = MOVE_COMMAND;		// ���Թ����� �־���
		bufcnt++;

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = SCRIPT_VAR;		// ��ũ��Ʈ�������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 's' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		if ( !vartype )
			error_process( "���� ��� ���� -> ������ ��Ұ� ���ڿ� ������ �ƴ� ( svar �̾�� �� )", fp );

		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = STRING_VAR;		// ���ڿ��������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'l' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// ���� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = LOCAL_VAR;		// ���ú������� �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( temp[0] == 'g' && temp[1] == 'v' && temp[2] == 'a' && temp[3] == 'r' )		// �۷ι� �������
	{
		varnum = atoi( &temp[4] );

		if ( varnum < 1 )
			error_process( "���� ��� ���� -> ��ȣ���� �������� ��ȣ�� �̻���", fp );

		buf[bufcnt] = GLOBAL_VAR;		// ������������ �־���
		bufcnt++;

		buf[bufcnt] = varnum;			// ������ ��ȣ�� �־���
		bufcnt++;
	}
	else
	if ( (temp[0] >= '0' && temp[0] <= '9') || temp[0] == '-' )		//  ���ڶ��...
	{
		varnum = atoi( temp );

		buf[bufcnt] = VALUE_VAR;		// ��ġ��
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum >> 8);			// ��ġ �־��� ����
		bufcnt++;

		buf[bufcnt] = (BYTE)(varnum & 0xff);		// ��ġ �־��� ����
		bufcnt++;
	}
	else
		error_process( "���� ��� ���� -> ��ȣ���� ������Ǵ� ��ġ�� �߸��Ǿ���", fp );					// ������ ���ٸ�

	return TRUE;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		�ּ� �ǳʾ��� �κ�( /*  */ �� ���� �κ� )
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
//		�̺�Ʈ ó���ϴ� �κ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	Event_check( char *temp, FILE *fp )
{
	if ( !strcmp( temp, "OnInitial" ) )			// ���� �̺�Ʈ �κ��̶��
	{
		buf[0] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[1] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[2] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[3] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnClicking" ) )		// Ŭ���� �̺�Ʈ �κ��̶��
	{
		buf[4] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[5] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[6] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[7] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnDraging" ) )			// ������ �巡�׽� �̺�Ʈ �κ��̶��
	{
		buf[8] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[9] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[10] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[11] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnHitting" ) )			// ������ �̺�Ʈ �κ��̶��
	{
		buf[12] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[13] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[14] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[15] = (BYTE)(bufcnt & 0xff);
	}
	else
	if ( !strcmp( temp, "OnKilling" ) )			// ���̴� �̺�Ʈ �κ��̶��
	{
		buf[16] = (BYTE)((bufcnt >> 24) & 0xff);
		buf[17] = (BYTE)((bufcnt >> 16) & 0xff);
		buf[18] = (BYTE)((bufcnt >> 8) & 0xff);
		buf[19] = (BYTE)(bufcnt & 0xff);
	}
	else
/*	if ( !strcmp( temp, "OnChangeQuest" ) )		// ����Ʈ ���� �κ��̶��.. ( �ַ� ����Ʈ�� ���� NPC�� ��ġ )
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
//		��ũ��Ʈ ������ �ϴ� �κ�
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

		if ( temp[0] == '/' && temp[1] == '*' )		// �ּ��̶��.. �پ� �Ѵ´�.
		{
			SkipRem( fp );
			continue;
		}

		if ( temp[0] == '{' )			// ���� ó���� ������ { �� �����ϹǷ�..
			continue;

		if ( temp[0] == '}' )			// ������ ������ ���̶��
		{
			if ( jumpbufcnt )			// if �� ���� ���κ��̶��, �ּҸ� �Է� ��Ŵ
			{
				jumpbufcnt--;			// �ٷ� ���� ������ ���̶�� ���� ǥ��
				buf[jumpbuf[jumpbufcnt]] = (BYTE)(bufcnt >> 24)&0xff;
				buf[jumpbuf[jumpbufcnt] + 1] = (BYTE)(bufcnt >> 16)&0xff;
				buf[jumpbuf[jumpbufcnt] + 2] = (BYTE)(bufcnt >> 8)&0xff;
				buf[jumpbuf[jumpbufcnt] + 3] = (BYTE)(bufcnt & 0xff);

				continue;
			}
			else
			{
				buf[bufcnt] = END_COMMAND;		// ������ ���� �����̶��..
				bufcnt++;


				if ( !event_handler )			// �̺�Ʈ�� ���õ� ������ �ƴ϶��. ���������� ��
					break;
				else
				{
					event_handler = FALSE;		// �̺�Ʈ �� �����̶��, ���� �̺�Ʈ
					continue;
				}
			}
		}

		if ( !strcmp( temp, "if" ) )			// ��ɾ� ó�� �κ�
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
			if ( !Check_var( temp, fp ) )		// ���Ը���� �ƴ��� ó��
			{
				if ( !Event_check( temp, fp ) )	// �̺�Ʈ ó���κ����� üũ
				{
					if ( !Check_function( temp, fp ) )	// �Լ� ó���κ� üũ
						if ( !Check_label( temp, fp ) )
							error_process( "Command Error -> �߸��� ��� �Ǵ� �Լ��Դϴ�.", fp );
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





