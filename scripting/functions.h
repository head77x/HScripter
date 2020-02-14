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
	Init_variables	: ���� ���� �ʱ�ȭ �Լ� ( OnInitial ���� ���� ���� )


	Set_npcname		: NPC�� ���͵��� �̸� ���� ( OnInitial ���� ���� ���� )
		- lvar1 : ĳ���� �ε���
		- svar1 : �̸�


	Set_questlog	: �� ����Ʈ �� �α� ���� ( OnInitial ���� ���� ���� )
		- lvar1 : �ش� ����Ʈ ����
		- lvar2 : �ش� ����Ʈ ������ ��
		- lvar3 : ����Ʈ ����
		- lvar3 : ����Ʈ Log

	
	Check_user_item : ������ �������� ������ �ִ��� üũ ( ���� ���ǹ������� ���� ) - ���ϰ��� lvar1 : 1 - ����, 0 - ����
		- lvar1 : üũ�� ������ index
		- lvar2 : üũ�� ������ lair


	Show_clickmessage : ��ȭ ǥ��
		- lvar1 : ĳ���� �̹��� ��ȣ
		- lvar2 : ��ȭ #1
		- lvar3 : ��ȭ #2
		- lvar4 : ��ȭ #3

	Show_yesno_message : ��ȭ ǥ��			���ϰ� lvar1
		- lvar1 : ĳ���� �̹��� ��ȣ
		- lvar2 : ���� 
		- lvar3 : ���� 1
		- lvar4 : ���� 2


	Set_scriptvar : ����Ʈ ������ ����
		- lvar1 : NPC ���� ���� ������ index 
		- lvar2 : NPC ���� ���� ������ ���� 

		- lvar3 : ���� ������ index 
		- lvar4 : ���� ������ lair 
		- lvar5 : ���� ������ ���� 

		- lvar6 : �׺��� ������ ���� ( ���� 1, ���� 2, ��Ʋ�� 3, ��� 4, ���� 5 )
		- lvar7 : ���׷��̵� �� ������ ���� ( ���� 1, ���� 2, ��Ʋ�� 3, ��� 4, ���� 5 )
		- lvar8 : ���׷��̵� �ܰ�

		- lvar9 : �þ ����ġ ��
		- lvar10 : ����� HP %
		- lvar11 : ����� EP %
		- lvar12 : ����� Jaia ����Ʈ ��( +, - )

		- lvar13 : ĳ���Ͱ� �̵��� ���� ��ȣ
		- lvar14 : ĳ���Ͱ� �̵��� X ��ǥ
		- lvar15 : ĳ���Ͱ� �̵��� Y ��ǥ
		- lvar16 : ĳ���Ͱ� �̵��� Z ��ǥ

		- lvar17 : �������� ��ȯ�� ������

		- lvar18 : ������ ����Ʈ ����
		- lvar19 : ������ ����Ʈ ������ ��


	Newbie_quest
		- lvar1 : �ʺ��ڿ� ����Ʈ ���� ��ȣ
*/


/*

�� ����Ʈ ����� ��ȭ ����

- ������ ����  / ������ ���� - Item_give_and_take �Լ� ( ��� ���� / ��� �谨 ���� )

- ����ġ ���� - Get_exp �Լ�

- ĳ���� �̵� - Move_user �Լ�

- �����, ������ ȸ�� - HP_EP_change �Լ�

- ���̾� ���� / ���̾� �谨 - Jaia_change �Լ�

- �׺� 1ȸ ����  - ����, ����, ��Ʋ��, ���, ���� - Gambling �Լ�

- ������ ���׷��̵� ( ��� ) - ����, ����, ��Ʋ��, ���, ���� - Item_upgrade �Լ�

- ���� ���� ��ȯ - Monster_recall �Լ�

*/

#endif
