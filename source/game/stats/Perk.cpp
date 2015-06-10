#include "Pch.h"
#include "Base.h"
#include "Perk.h"

PerkInfo g_perks[(int)Perk::Max] = {
	Perk::Weakness, "S�abo��", "Wada - Zmniejsza wybrany atrybut o 5 punkt�w. Pozwala wybra� dwa dodatkowe atuty.", PerkInfo::Flaw | PerkInfo::History,
	Perk::Strength, "Si�a", "Zwi�ksza wybrany atrybut o 5 punkt�w.", PerkInfo::History,
	Perk::Skilled, "Fachowiec", "Daje dwa dodatkowe punkty umiej�tno�ci.", PerkInfo::Multiple | PerkInfo::History,
	Perk::SkillFocus, "Skupienie na umiej�tno�ci", "Darmowy - Zmniejsza dwie wybrane umiej�tno�ci o 5, zwi�ksza jedn� o 10.", PerkInfo::Free | PerkInfo::History,
	Perk::Talent, "Talent", "Zwi�ksza wybran� umiej�tno�� o 5 punkt�w.", PerkInfo::History,
	Perk::CraftingTradition, "Tradycja rzemie�lnicza", "Zwi�ksza umiej�tno�� Rzemios�o o 10 punkt�w.", PerkInfo::History,
};
