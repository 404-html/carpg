#pragma once

//-----------------------------------------------------------------------------
// identyfikatory quest�w
// kolejno�� nie jest nigdzie u�ywana, mo�na dawa� jak si� chce, ale na ko�cu �eby zapisy by�y kompatybilne
enum QUEST
{
	Q_KOPALNIA, // po oczyszczeniu kopalni zysk 1000 sz/m, po jakim� czasie kurier, �e odkryto �y�� z�ota i po zainwestowaniu 100k daje 2500 sz/m, po jakim� czasie dokopuj� si� do portalu kt�ry przenosi do miejsca gdzie jest artefakt
	Q_TARTAK, // trzeba oczy�ci� las z potwor�w i od tego czasu przynosi zysk 500 sz/m
	Q_BANDYCI, // pomagasz mistrzowi agent�w przeciwko bandytom
	Q_MAGOWIE, // najpierw jaki� quest �e magowie zachodz� ci za sk�r�, potem przez przypadek im pomagasz,
	Q_MAGOWIE2,
	Q_ORKOWIE,
	Q_ORKOWIE2,
	Q_GOBLINY,
	Q_ZLO,

	Q_DELIVER_LETTER,
	Q_DELIVER_PARCEL,
	Q_SPREAD_NEWS,
	Q_RESCUE_CAPTIVE,
	Q_BANDITS_COLLECT_TOLL,
	Q_CAMP_NEAR_CITY,
	Q_RETRIVE_PACKAGE,
	Q_KILL_ANIMALS,
	Q_ZGUBIONY_PRZEDMIOT, // tawerna, kto� zgubi� przedmiot w podziemiach i chce go odzyska�
	Q_UKRADZIONY_PRZEDMIOT, // tawerna, bandyci co� ukradli, trzeba to odzyska�
	Q_PRZYNIES_ARTEFAKT, // tawerna, poszukuje jakiego� artefaktu, jest w podziemiach/krypcie

	Q_SZALENI,
	Q_LIST_GONCZY,

	Q_MAIN,
};

//-----------------------------------------------------------------------------
static const int UNIQUE_QUESTS = 8;
