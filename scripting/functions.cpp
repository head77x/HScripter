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
//		함수 체크하는 부분
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	Check_function( char *temp, FILE *fp )
{
	if ( !strcmp( temp, "Init_variables" ) )		 //		초기화 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = INIT_VARIABLES_FUNC;	// init_variables 수치 넣어줌
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_npcname" ) )	// 이름 설정하는 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_NPCNAME_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_mapname" ) )	// 이름 설정하는 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_MAPNAME_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_questlog" ) )	// 퀘스트 로그 남기는 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_QUESTLOG_FUNC;
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_user_item" ) )		// 유저 아이템 소지 여부 체크하는 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = CHECK_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Give_user_item" ) )		// 유저 아이템 소지 여부 체크하는 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = GIVE_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Delete_user_item" ) )		// 유저 아이템 삭제
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = DELETE_USER_ITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_toy_level" ) )		// 토이 레벨 조사
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = CHECK_TOY_LEVEL_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Show_clickmessage" ) )		// 캐릭터 세우기 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SHOW_CLICKMESSAGE_FUNC;	// init_variables 수치 넣어줌
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Get_scriptvar" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = GET_SCRIPTVAR_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_scriptvar" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_SCRIPTVAR_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_quiz" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_QUIZ_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_quiz" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = RUN_QUIZ_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Get_toystatus" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = GET_TOYSTATUS_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_toygetitem" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_TOYGETITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Reset_toygetitem" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = RESET_TOYGETITEM_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Show_multiselect" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SHOW_MULTISELECT_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_minigame" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = RUN_MINIGAME_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Run_shop" ) )		// 스크립트 변수 셋팅 함수
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = RUN_SHOP_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Set_englishword" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = SET_ENGLISHWORD_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Move_character" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = MOVE_CHARACTER_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Start_monsterkill" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = START_MONSTERKILL_FUNC;	
		bufcnt++;
	}
	else
	if ( !strcmp( temp, "Check_nowmap" ) )
	{
		buf[bufcnt] = FUNCTION_COMMAND;		// 함수임
		bufcnt++;

		buf[bufcnt] = CHECK_NOWMAP_FUNC;	
		bufcnt++;
	}
	else
		return FALSE;

	return TRUE;
}

