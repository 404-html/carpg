#pragma once

//-----------------------------------------------------------------------------
#include "Quest.h"

//-----------------------------------------------------------------------------
// jaki� facet chce �eby odnale�� dla niego magiczny artefakt
// idziemy do krypty i go przynosimy
// p�aci i znika jak pos�aniec
// po jakim� czasie natrafiamy na golema na drodze kt�ry ka�e nam odda� ca�e z�oto, dostajemy questa "Golem na drodze"
// mo�emy porozmawia� z dowolnym kapitanem stra�y kt�ry m�wi o tym �e ma wi�cej informacji o golemach bududowanych przez mag�w
// wysy�a nas do jakiego� maga w wiosce
// jest pijany i chce piwo, potem w�dk�, potem prowadzi nas do jakich� podziemi w kt�rych nic nie ma
// wracamy do kapitana kt�ry nam to zleci�
// ka�e nam przynie�� miksturk� oczyszczenia umys�u (mo�na kupi� u alchemika za 100 z�ota)
// dajemy j� magowie [m�wi �e sobie wszystko przypomnia� i nienawidzi piwa]
// informuje nas o tym �e jaki� poszukiwacz przyg�d pom�g� jednemu z mag�w znale�� kule wi�zi potrzebn� do budowy golem�w
// m�wi nam gdzie trzeba i�� zabi� maga
// wracamy do kapitana, daje nagrod�
class Quest_Magowie : public Quest_Dungeon
{
public:
	enum Progress
	{
		None,
		Started,
		Finished,
		EncounteredGolem
	};

	void Start();
	DialogEntry* GetDialog(int type2);
	void SetProgress(int prog2);
	cstring FormatString(const string& str);
	bool IfNeedTalk(cstring topic);
	void Load(HANDLE file);
};

//-----------------------------------------------------------------------------
class Quest_Mages : public Quest_Dungeon, public UnitEventHandler
{
public:
	enum Progress
	{
		None,
		Started,
		MageWantsBeer,
		MageWantsVodka,
		GivenVodka,
		GotoTower,
		MageTalkedAboutTower,
		TalkedWithCaptain,
		BoughtPotion,
		MageDrinkPotion,
		NotRecruitMage,
		RecruitMage,
		KilledBoss,
		TalkedWithMage,
		Finished
	};

	enum Talked
	{
		No,
		AboutHisTower,
		AfterEnter,
		BeforeBoss
	};
	//0-nie powiedzia�, 1-pogada� w jego wie�y, 2-pogada� po wej�ciu, 3-pogada� przed bossem

	void Start();
	DialogEntry* GetDialog(int type2);
	void SetProgress(int prog2);
	cstring FormatString(const string& str);
	bool IfNeedTalk(cstring topic);
	void HandleUnitEvent(UnitEventHandler::TYPE type, Unit* unit);
	void Save(HANDLE file);
	void Load(HANDLE file);
	int GetUnitEventHandlerQuestRefid()
	{
		return refid;
	}

	Talked talked;

private:
	int mage_loc;
};
