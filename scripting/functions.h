#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <io.h>
#include <conio.h>
#include <direct.h>

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

enum { INIT_VARIABLES_FUNC = 1, SET_NPCNAME_FUNC, SET_MAPNAME_FUNC, SET_QUESTLOG_FUNC, CHECK_USER_ITEM_FUNC, GIVE_USER_ITEM_FUNC, DELETE_USER_ITEM_FUNC, CHECK_TOY_LEVEL_FUNC, 
	SHOW_CLICKMESSAGE_FUNC, GET_SCRIPTVAR_FUNC, SET_SCRIPTVAR_FUNC, SET_QUIZ_FUNC, RUN_QUIZ_FUNC, GET_TOYSTATUS_FUNC, SET_TOYGETITEM_FUNC, RESET_TOYGETITEM_FUNC, SHOW_MULTISELECT_FUNC,
	RUN_MINIGAME_FUNC, RUN_SHOP_FUNC, SET_ENGLISHWORD_FUNC, MOVE_CHARACTER_FUNC, START_MONSTERKILL_FUNC, CHECK_NOWMAP_FUNC };

extern BOOL	Check_function( char *temp, FILE *fp );

/*
	Init_variables	: 각종 변수 초기화 함수 ( OnInitial 에서 최초 실행 )


	Set_npcname		: NPC및 몬스터들의 이름 지정 ( OnInitial 에서 최초 실행 )
		- lvar1 : 캐릭터 인덱스
		- svar1 : 이름


	Set_questlog	: 각 퀘스트 별 로그 지정 ( OnInitial 에서 최초 실행 )
		- lvar1 : 해당 퀘스트 변수
		- lvar2 : 해당 퀘스트 변수의 값
		- lvar3 : 퀘스트 제목
		- lvar3 : 퀘스트 Log

	
	Check_user_item : 유저가 아이템을 가지고 있는지 체크 ( 각종 조건문이전에 실행 ) - 리턴값은 lvar1 : 1 - 있음, 0 - 없음
		- lvar1 : 체크할 아이템 index
		- lvar2 : 체크할 아이템 lair


	Show_clickmessage : 대화 표시
		- lvar1 : 캐릭터 이미지 번호
		- lvar2 : 대화 #1
		- lvar3 : 대화 #2
		- lvar4 : 대화 #3

	Show_yesno_message : 대화 표시			리턴값 lvar1
		- lvar1 : 캐릭터 이미지 번호
		- lvar2 : 물음 
		- lvar3 : 선택 1
		- lvar4 : 선택 2


	Set_scriptvar : 퀘스트 변수값 셋팅
		- lvar1 : NPC 에게 뺏길 아이템 index 
		- lvar2 : NPC 에게 뺏길 아이템 갯수 

		- lvar3 : 받을 아이템 index 
		- lvar4 : 받을 아이템 lair 
		- lvar5 : 받을 아이템 갯수 

		- lvar6 : 겜블링할 아이템 종류 ( 무기 1, 갑옷 2, 건틀렛 3, 헬멧 4, 반지 5 )
		- lvar7 : 업그레이드 할 아이템 종류 ( 무기 1, 갑옷 2, 건틀렛 3, 헬멧 4, 반지 5 )
		- lvar8 : 업그레이드 단계

		- lvar9 : 늘어날 경험치 양
		- lvar10 : 변경될 HP %
		- lvar11 : 변경될 EP %
		- lvar12 : 변경될 Jaia 포인트 값( +, - )

		- lvar13 : 캐릭터가 이동할 지역 번호
		- lvar14 : 캐릭터가 이동할 X 좌표
		- lvar15 : 캐릭터가 이동할 Y 좌표
		- lvar16 : 캐릭터가 이동할 Z 좌표

		- lvar17 : 주위에서 소환할 마리수

		- lvar18 : 변경할 퀘스트 변수
		- lvar19 : 변경할 퀘스트 변수의 값


	Newbie_quest
		- lvar1 : 초보자용 퀘스트 수행 번호
*/


/*

◎ 퀘스트 진행시 변화 내용

- 아이템 지급  / 아이템 삭제 - Item_give_and_take 함수 ( 골드 지급 / 골드 삭감 포함 )

- 경험치 증가 - Get_exp 함수

- 캐릭터 이동 - Move_user 함수

- 생명력, 에너지 회복 - HP_EP_change 함수

- 자이아 지급 / 자이아 삭감 - Jaia_change 함수

- 겜블링 1회 가능  - 무기, 갑옷, 건틀렛, 헬멧, 반지 - Gambling 함수

- 아이템 업그레이드 ( 루멘 ) - 무기, 갑옷, 건틀렛, 헬멧, 반지 - Item_upgrade 함수

- 몬스터 렌덤 소환 - Monster_recall 함수

*/

#endif
