#pragma once

//-----------------------------------------------------------------------------
#include "Material.h"
#include "UnitStats.h"
#include "Blood.h"
#include "StatProfile.h"
#include "ArmorUnitType.h"
#include "Resource.h"

//-----------------------------------------------------------------------------
struct Spell;
struct DialogEntry;

//-----------------------------------------------------------------------------
// Lista zakl�� postaci
struct SpellList
{
	int level[3];
	cstring name[3];
	Spell* spell[3];
	bool have_non_combat;

	SpellList(int _l1, cstring _n1, int _l2, cstring _n2, int _l3, cstring _n3, bool _have_non_combat) : spell(), have_non_combat(_have_non_combat)
	{
		level[0] = _l1;
		level[1] = _l2;
		level[2] = _l3;
		name[0] = _n1;
		name[1] = _n2;
		name[2] = _n3;
	}
};

//-----------------------------------------------------------------------------
// Unit groups
enum UNIT_GROUP
{
	G_PLAYER,
	G_TEAM,
	G_CITZENS,
	G_GOBLINS,
	G_ORCS,
	G_UNDEADS,
	G_MAGES,
	G_ANIMALS,
	G_CRAZIES,
	G_BANDITS
};

//-----------------------------------------------------------------------------
// Flagi postaci
enum FLAGI_POSTACI
{
	F_CZLOWIEK = 1<<0, // u�ywa przedmiot�w, wida� zbroj�, ma brod� i w�osy
	F_HUMANOID = 1<<1, // u�ywa przedmiot�w
	F_TCHORZLIWY = 1<<2, // ucieka gdy ma ma�o hp albo kto� zginie
	F_NIE_UCIEKA = 1<<3, // nigdy nie ucieka
	F_LUCZNIK = 1<<4, // preferuje walk� broni� dystansow�
	F_DOWODCA = 1<<5, // inni go chroni� ?
	F_KLUTE25 = 1<<6, // odporno�� na k�ute 25%
	F_CIETE25 = 1<<7, // odporno�� na ci�te 25%
	F_OBUCHOWE25 = 1<<8, // odporno�� na obuchowe 25%
	F_KLUTE_MINUS25 = 1<<9, // podatno�� na k�ute 25%
	F_CIETE_MINUS25 = 1<<10, // podatno�� na ci�te 25%
	F_OBUCHOWE_MINUS25 = 1<<11, // podatno�� na obuchowe 25%
	F_NIEUMARLY = 1<<12, // mo�na o�ywi�
	F_POWOLNY = 1<<13, // nie biega
	F_TRUJACY_ATAK = 1<<14, // atak zatruwa
	F_NIESMIERTELNY = 1<<15, // nie mo�na zabi� tej postaci
	F_TOMASH = 1<<16, // przy generowaniu postaci jest �ci�le okre�lony kolor i w�osy
	F_SZALONY = 1<<17, // mo�liwe kolorowe w�osy
	F_NIE_OTWIERA = 1<<18, // nie potrafi otwiera� drzwi
	F_ZA_LEKKI = 1<<19, // nie uruchamia pu�apek
	F_SEKRETNA = 1<<20, // nie mo�na zespawnowa�
	F_NIE_CIERPI = 1<<21, // odporno�� na b�l
	F_MAG = 1<<22, // pr�buje sta� jak najdalej od przeciwnika
	F_ODPORNOSC_NA_TRUCIZNY = 1<<23, // odporno�� na trucizny
	F_SZARE_WLOSY = 1<<24, // dla nieumar�ych i nekromant�w
	F_BRAK_POTEZNEGO_ATAKU = 1<<25, // nie posiada pot�nego ataku
	F_AI_URZEDNIK = 1<<26, // jak stoi ale ma animacj� przegl�dania dokument�w, chodzi niedaleko
	F_AI_STRAZNIK = 1<<27, // stoi w miejscu i si� rozgl�da
	F_AI_STOI = 1<<28, // stoi w miejscu i u�ywa obiekt�w, chodzi niedaleko
	F_AI_CHODZI = 1<<29, // u�ywa obiekt�w, �azi po ca�ym mie�cie
	F_AI_PIJAK = 1<<30, // pije piwo o ile jest w budynku
	F_BOHATER = 1<<31 // mo�na go do��czy� do dru�yny, ma HeroData
};

//-----------------------------------------------------------------------------
// Kolejne flagi postaci
enum FLAGI_POSTACI2
{
	F2_AI_TRENUJE = 1<<0, // trenuje walk� na manekinie/celu strzelniczym
	F2_OKRESLONE_IMIE = 1<<1, // nie generuje imienia
	F2_BEZ_KLASY = 1<<2, // ta posta� tak na prawde nie jest bohaterem ale ma imie/mo�e pod��a� za graczem
	F2_ZAWODY_W_PICIU = 1<<3, // do��cza do zawod�w w piciu
	F2_ZAWODY_W_PICIU_50 = 1<<4, // 50% �e do��czy do zawod�w w piciu
	F2_FLAGA_KLASY = 1<<5, // ma flag� klasy
	F2_WOJOWNIK = 1<<6, // okre�lona klasa - wojownik
	F2_LOWCA = 1<<7, // okre�lona klasa - �owca
	F2_LOTRZYK = 1<<8, // okre�lona klasa - �otrzyk
	F2_STARY = 1<<9, // siwe w�osy
	F2_WALKA_WRECZ = 1<<10, // walczy wr�cz nawet jak ma �uk a wr�g jest daleko
	F2_WALKA_WRECZ_50 = 1<<11, // walczy wr�cz 50%
	F2_BOSS = 1<<12, // muzyka bossa
	F2_BRAK_KRWII = 1<<13, // nie mo�na rzuci� wyssania hp
	F2_OGRANICZONY_OBROT = 1<<14, // stoi w miar� prosto - karczmarz za lad�
	F2_KAPLAN = 1<<15, // okre�lona klasa - kap�an
	F2_AKTUALIZUJ_PRZEDMIOTY_V0 = 1<<16, // aktualizuje ekwipunek je�li zapisano w V0
	F2_UZYWA_TRONU = 1<<17, // siada na tronie
	F2_DZWIEK_ORK = 1<<18, // d�wi�k gadania
	F2_DZWIEK_GOBLIN = 1<<19, // d�wi�k gadania
	F2_XAR = 1<<20, // d�wi�k gadania, stoi przed o�tarzem i si� modli
	F2_DZWIEK_GOLEM = 1<<21, // d�wi�k gadania
	F2_ZAWODY = 1<<22, // bierze udzia� w zawodach
	F2_OKRZYK = 1<<23, // okrzyk bojowy nawet gdy kto� inny pierwszy zauwa�y wroga
	F2_CIOS_W_PLECY = 1<<24, // podw�jna premia za cios w plecy
	F2_OMIJA_BLOK = 1<<25, // blokowanie mniej daje przeciwko jego atakom
	F2_ODPORNOSC_NA_CIOS_W_PLECY = 1<<26, // 50% odporno�ci na ataki w plecy
	F2_ODPORNOSC_NA_MAGIE_50 = 1<<27, // 50% odporno�ci na magi�
	F2_ODPORNOSC_NA_MAGIE_25 = 1<<28, // 25% odporno�ci na magi�
	F2_OZNACZ = 1<<29, // rysuje trupa na minimapie
	F2_OBSTAWA = 1<<30, // jednostki wygenerowane w tym samym pokoju chroni� go (dzia�a tylko w podziemiach na Event::unit_to_spawn)
	F2_NIE_GOBLIN = 1<<31, // nie ma tekst�w goblina
};

//-----------------------------------------------------------------------------
// Nowe flagi postaci...
enum FLAGI_POSTACI3
{
	F3_ZAWODY_W_PICIU_25 = 1<<0, // 25% szansy �e we�mie udzia� w zawodach w piciu
	F3_MAG_PIJAK = 1<<1, // bierze udzia� w zawodach w piciu o ile jest pijakiem, bierze udzia� w walce na arenie o ile nie jest, pije w karczmie i po do��czeniu
	F3_PIJAK_PO_ZAWODACH = 1<<2, // jak pijak ale po zawodach
	F3_NIE_JE = 1<<3, // nie je bo nie mo�e albo jest w pracy
	F3_ORKOWE_JEDZENIE = 1<<4, // je orkowe jedzenie a nie normalne
	F3_GORNIK = 1<<5, // 50% szansy �e zajmie si� wydobywaniem
	F3_GADA_NA_ZAWODACH = 1<<6, // nie gada o pierdo�ach na zawodach
};

//-----------------------------------------------------------------------------
// Id d�wi�ku
enum SOUND_ID
{
	SOUND_SEE_ENEMY,
	SOUND_PAIN,
	SOUND_DEATH,
	SOUND_ATTACK,
	SOUND_MAX
};

//-----------------------------------------------------------------------------
// D�wi�ki postaci
struct SoundPak
{
	cstring id[SOUND_MAX];
	SOUND sound[SOUND_MAX];
	bool inited;

	SoundPak(cstring see_enemy, cstring pain, cstring death, cstring attack) : inited(false)
	{
		id[SOUND_SEE_ENEMY] = see_enemy;
		id[SOUND_PAIN] = pain;
		id[SOUND_DEATH] = death;
		id[SOUND_ATTACK] = attack;
		for(int i=0; i<SOUND_MAX; ++i)
			sound[i] = NULL;
	}
};

//-----------------------------------------------------------------------------
// Typy ramek
enum FRAME_INDEX
{
	F_CAST,
	F_TAKE_WEAPON,
	F_ATTACK1_START,
	F_ATTACK1_END,
	F_ATTACK2_START,
	F_ATTACK2_END,
	F_ATTACK3_START,
	F_ATTACK3_END,
	F_BASH,
	F_MAX
};

//-----------------------------------------------------------------------------
// Flagi jaki atak nale�y do tej animacji ataku
#define A_SHORT_BLADE (1<<0)
#define A_LONG_BLADE (1<<1)
#define A_BLUNT (1<<2)
#define A_AXE (1<<3)

//-----------------------------------------------------------------------------
// Informacje o animacjach ataku postaci
struct AttackFrameInfo
{
	struct Entry
	{
		float start, end;
		int flags;

		inline float lerp() const
		{
			return ::lerp(start, end, 2.f/3);
		}
	};
	static const int attacks = 5;
	Entry e[attacks];
};

//-----------------------------------------------------------------------------
// Informacje o ramce animacji
struct FrameInfo
{
	AttackFrameInfo* extra;
	float t[F_MAX];
	int attacks;

	inline float lerp(int frame) const
	{
		return ::lerp(t[frame], t[frame+1], 2.f/3);
	}
};

//-----------------------------------------------------------------------------
// Dane postaci
struct UnitData
{
	string id, mesh, name;
	Animesh* ani;
	MATERIAL_TYPE mat;
	INT2 level;
	StatProfileType stat_profile;
	int hp_bonus, def_bonus, dmg_type, flags, flags2, flags3;
	const int* items;
	SpellList* spells;
	INT2 gold, gold2;
	DialogEntry* dialog;
	UNIT_GROUP group;
	float walk_speed, run_speed, rot_speed, width, attack_range;
	BLOOD blood;
	SoundPak* sounds;
	FrameInfo* frames;
	TexId* tex;
	cstring* idles;
	int idles_count;
	ArmorUnitType armor_type;

	UnitData(cstring id, cstring _mesh, MATERIAL_TYPE mat, const INT2& level, StatProfileType stat_profile, int flags, int flags2, int flags3, int hp_bonus, int def_bonus,
		const int* items, SpellList* spells, const INT2& gold, const INT2& gold2, DialogEntry* dialog, UNIT_GROUP group, int dmg_type, float walk_speed, float run_speed, float rot_speed,
		BLOOD blood, SoundPak* sounds, FrameInfo* frames, TexId* tex, cstring* idles, int idles_count, float width, float attack_range, ArmorUnitType armor_type) :
		id(id), mat(mat), ani(NULL), level(level), stat_profile(stat_profile), hp_bonus(hp_bonus), def_bonus(def_bonus), dmg_type(dmg_type), flags(flags), flags2(flags2),
		flags3(flags3), items(items), spells(spells), gold(gold), gold2(gold2), dialog(dialog), group(group), walk_speed(walk_speed), run_speed(run_speed), rot_speed(rot_speed),
		width(width), attack_range(attack_range), blood(blood), sounds(sounds), frames(frames), tex(tex), idles(idles), idles_count(idles_count), armor_type(armor_type)
	{
		if(_mesh)
			mesh = _mesh;
	}

	inline float GetRadius() const
	{
		return width;
	}

	inline StatProfile& GetStatProfile() const
	{
		return g_stat_profiles[(int)stat_profile];
	}
};
extern UnitData g_base_units[];
extern const uint n_base_units;

//-----------------------------------------------------------------------------
inline UnitData* FindUnitData(cstring id, bool report=true)
{
	assert(id);

	for(uint i=0; i<n_base_units; ++i)
	{
		if(g_base_units[i].id == id)
			return &g_base_units[i];
	}

	// konwersja 0.2.(0/1) do 0.2.5
	if(strcmp(id, "necromant") == 0)
		return FindUnitData("necromancer", report);

	if(report)
		throw Format("Can't find base unit data '%s'!", id);

	return NULL;
}
