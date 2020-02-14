#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <io.h>
#include <conio.h>
#include <direct.h>

#include "scripting.h"
#include "functions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		�Լ� üũ�ϴ� �κ�
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	Check_function( char *temp, FILE *fp )
{
	if ( !strcmp( temp, "Init_variables" ) )		 //		�ʱ�ȭ �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = INIT_VARIABLES_FUNC;	// init_variables ��ġ �־���
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_npcname" ) )	// �̸� �����ϴ� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_NPCNAME_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_mapname" ) )	// �̸� �����ϴ� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_MAPNAME_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_questlog" ) )	// ����Ʈ �α� ����� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_QUESTLOG_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_user_item" ) )		// ���� ������ ���� ���� üũ�ϴ� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = CHECK_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Give_user_item" ) )		// ���� ������ ���� ���� üũ�ϴ� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = GIVE_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Delete_user_item" ) )		// ���� ������ ����
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = DELETE_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_toy_level" ) )		// ���� ���� ����
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = CHECK_TOY_LEVEL_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Show_clickmessage" ) )		// ĳ���� ����� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SHOW_CLICKMESSAGE_FUNC;	// init_variables ��ġ �־���
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Get_scriptvar" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = GET_SCRIPTVAR_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_scriptvar" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_SCRIPTVAR_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_quiz" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_QUIZ_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_quiz" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = RUN_QUIZ_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Get_toystatus" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = GET_TOYSTATUS_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_toygetitem" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_TOYGETITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Reset_toygetitem" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = RESET_TOYGETITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Show_multiselect" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SHOW_MULTISELECT_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_minigame" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = RUN_MINIGAME_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_shop" ) )		// ��ũ��Ʈ ���� ���� �Լ�
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = RUN_SHOP_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_englishword" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = SET_ENGLISHWORD_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Move_character" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = MOVE_CHARACTER_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Start_monsterkill" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = START_MONSTERKILL_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_nowmap" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// �Լ���
		bufcnt++;

		buf[bufcnt] = CHECK_NOWMAP_FUNC;	
		bufcnt++;
	}
	else
		return FALSE;

	return TRUE;
}

