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

	Q_DOSTARCZ_LIST, // burmistrz, dostarcz list do innego burmistrza
	Q_DOSTARCZ_PACZKE, // burmistrz, id� do innego miasta i odbierz paczk� od burmistrza i dostarcz do tego
	Q_ROZNIES_WIESCI, // burmistrz, zanie� wie�ci do okolicznych burmistrz�w
	Q_URATUJ_PORWANA_OSOBE, // stra�nik, bandyci porwali osob�, udaj si� do podziemi i j� uratuj
	Q_BANDYCI_POBIERAJA_OPLATE, // stra�nik, bandyci rozbili ob�z ko�o drogi i pobieraj� op�at�, zabij ich
	Q_OBOZ_KOLO_MIASTA, // stra�nik, bandyci rozbili ob�z niedaleko miasta, trzeba ich pokona�
	Q_ODZYSKAJ_PACZKE, // burmistrz, bandyci przejeli paczk�, trzeba j� odzyska�
	Q_ZABIJ_ZWIERZETA, // stra�nik, w lesie/jaskini s� zwierz�ta ko�o miasta, trzba je pokona�
	Q_ZGUBIONY_PRZEDMIOT, // tawerna, kto� zgubi� przedmiot w podziemiach i chce go odzyska�
	Q_UKRADZIONY_PRZEDMIOT, // tawerna, bandyci co� ukradli, trzeba to odzyska�
	Q_PRZYNIES_ARTEFAKT, // tawerna, poszukuje jakiego� artefaktu, jest w podziemiach/krypcie

	Q_SZALENI,
	Q_LIST_GONCZY,

	Q_MAIN,
};

//-----------------------------------------------------------------------------
static const int UNIQUE_QUESTS = 8;
