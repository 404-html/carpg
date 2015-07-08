#pragma once

//-----------------------------------------------------------------------------
#include "Quest.h"

//-----------------------------------------------------------------------------
// orkowie napadaj� na karawany, trzeba i�� do podziemi i ich pozabija�
// w podziemiach jest zamkni�ty pok�j kt�ry klucz ma najsilniejsza jednostka
// w zamkni�tym pokoju jest ork kt�ry si� do nas przy��cza [Gorush]
// po jakim� czasie m�wi nam o obozie ork�w kt�rzy go porwali gdy by� m�ody
// po zniszczeniu awansuje na szamana/�owce/wojownika
// po jakim� czasie m�wi o swoim klanie kt�ry zosta� podbity przez silnego orka
// trzeba i�� i ich pobi� a wtedy ork zostaje nowym wodzem, mo�na tam spotka� orkowego kowala kt�ry sprzedaje orkowe przedmioty
class Quest_Orcs : public Quest_Dungeon, public LocationEventHandler
{
public:
	enum Progress
	{
		None,
		TalkedWithGuard,
		NotAccepted,
		Started,
		ClearedLocation,
		Finished
	};

	void Start();
	DialogEntry* GetDialog(int type2);
	void SetProgress(int prog2);
	cstring FormatString(const string& str);
	bool IfNeedTalk(cstring topic);
	void HandleLocationEvent(LocationEventHandler::Event event);
	void Save(HANDLE file);
	void Load(HANDLE file);
	int GetLocationEventHandlerQuestRefid()
	{
		return refid;
	}

private:
	int dungeon_levels, levels_cleared;
};

//-----------------------------------------------------------------------------
class Quest_Orcs2 : public Quest_Dungeon, public LocationEventHandler, public UnitEventHandler
{
public:
	enum Progress
	{
		None,
		TalkedOrc,
		NotJoined,
		Joined,
		TalkedAboutCamp,
		TalkedWhereIsCamp,
		ClearedCamp,
		TalkedAfterClearingCamp,
		SelectWarrior,
		SelectHunter,
		SelectShaman,
		SelectRandom,
		ChangedClass,
		TalkedAboutBase,
		TalkedWhereIsBase,
		KilledBoss,
		Finished
	};

	enum Talked
	{
		No,
		AboutCamp,
		AboutBase,
		AboutBoss
	};

	void Start();
	DialogEntry* GetDialog(int type2);
	void SetProgress(int prog2);
	cstring FormatString(const string& str);
	bool IfNeedTalk(cstring topic);
	bool IfQuestEvent();
	void HandleLocationEvent(LocationEventHandler::Event event);
	void HandleUnitEvent(UnitEventHandler::TYPE event, Unit* unit);
	void Save(HANDLE file);
	void Load(HANDLE file);
	int GetUnitEventHandlerQuestRefid()
	{
		return refid;
	}
	int GetLocationEventHandlerQuestRefid()
	{
		return refid;
	}

	Talked talked;

private:
	void ChangeClass(int klasa);

	int near_loc;
};
