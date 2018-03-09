#include "Pch.h"
#include "GameCore.h"
#include "Item.h"
#include "ItemSlot.h"
#include "Unit.h"
#include "Language.h"

cstring txAttack, txDefense, txMobility, txRequiredStrength, txDTBlunt, txDTPierce, txDTSlash, txDTBluntPierce, txDTBluntSlash, txDTSlashPierce, txDTMagical, txWeight,
txValue, txInvalidArmor;

//=================================================================================================
void SetItemStatsText()
{
	txAttack = Str("attack");
	txDefense = Str("defense");
	txMobility = Str("mobility");
	txRequiredStrength = Str("requiredStrength");
	txDTBlunt = Str("dtBlunt");
	txDTPierce = Str("dtPierce");
	txDTSlash = Str("dtSlash");
	txDTBluntPierce = Str("dtBluntPierce");
	txDTBluntSlash = Str("dtBluntSlash");
	txDTSlashPierce = Str("dtSlashPierce");
	txDTMagical = Str("dtMagical");
	txWeight = Str("weight");
	txValue = Str("value");
	txInvalidArmor = Str("invalidArmor");
}

//=================================================================================================
// Funkcja u�ywana do sortowanie przedmiot�w
// true - s1 ma by� wcze�niej
//=================================================================================================
static bool SortItemsPred(const ItemSlot& s1, const ItemSlot& s2)
{
	if(!s1.item)
		return false;
	if(!s2.item)
		return true;
	return ItemCmp(s1.item, s2.item);
}

//=================================================================================================
// Sortowanie przedmiot�w
//=================================================================================================
void SortItems(vector<ItemSlot>& items)
{
	// sortuj przedmioty
	std::sort(items.begin(), items.end(), SortItemsPred);

	// usu� puste elementy
	while(!items.empty() && !items.back().item)
		items.pop_back();
}

void GetItemString(string& str, const Item* item, Unit* unit, uint count)
{
	assert(item);

	str = item->name;

	switch(item->type)
	{
	case IT_WEAPON:
		{
			/*
			Rapier - Short blade
			Attack: 30 (40 -> 50) piercing
			Required strength: $50$
			*/
			const Weapon& weapon = item->ToWeapon();

			cstring dmg_type;
			switch(weapon.dmg_type)
			{
			case DMG_BLUNT:
				dmg_type = txDTBlunt;
				break;
			case DMG_PIERCE:
				dmg_type = txDTPierce;
				break;
			case DMG_SLASH:
				dmg_type = txDTSlash;
				break;
			case DMG_BLUNT | DMG_PIERCE:
				dmg_type = txDTBluntPierce;
				break;
			case DMG_BLUNT | DMG_SLASH:
				dmg_type = txDTBluntSlash;
				break;
			case DMG_SLASH | DMG_PIERCE:
				dmg_type = txDTSlashPierce;
				break;
			case DMG_BLUNT | DMG_PIERCE | DMG_SLASH:
				dmg_type = txDTMagical;
				break;
			default:
				dmg_type = "???";
				break;
			}

			int old_attack = (unit->HaveWeapon() ? (int)unit->CalculateAttack() : 0);
			int new_attack = (int)unit->CalculateAttack(item);
			cstring atk_desc;
			if(old_attack == new_attack)
				atk_desc = Format("%d", old_attack);
			else
				atk_desc = Format("%d -> %d", old_attack, new_attack);

			str += Format(" - %s\n%s: %d (%s) %s\n%s: $c%c%d$c-\n",
				WeaponTypeInfo::info[weapon.weapon_type].name,
				txAttack,
				weapon.dmg,
				atk_desc,
				dmg_type,
				txRequiredStrength,
				(unit->Get(AttributeId::STR) >= weapon.req_str ? '-' : 'r'),
				weapon.req_str);
		}
		break;
	case IT_BOW:
		{
			/*
			Long bow
			Attack: 30 (40 -> 50) piercing
			Required strength: $40$
			*/
			const Bow& bow = item->ToBow();

			int old_attack = (unit->HaveBow() ? (int)unit->CalculateAttack(&unit->GetBow()) : 0);
			int new_attack = (int)unit->CalculateAttack(item);
			cstring atk_desc;
			if(old_attack == new_attack)
				atk_desc = Format("%d", old_attack);
			else
				atk_desc = Format("%d -> %d", old_attack, new_attack);

			str += Format("\n%s: %d (%s) %s\n%s: $c%c%d$c-\n",
				txAttack,
				bow.dmg,
				atk_desc,
				txDTPierce,
				txRequiredStrength,
				(unit->Get(AttributeId::STR) >= bow.req_str ? '-' : 'r'),
				bow.req_str);
		}
		break;
	case IT_ARMOR:
		{
			/*
			Chainmail - Medium armor [(Does not fit)]
			Defense: 30 (40 -> 50)
			Required strength: $40$
			Mobility: 50 (40 -> 50)
			*/
			const Armor& armor = item->ToArmor();
			cstring mob_str, armor_type;

			cstring skill = Skill::skills[(int)armor.skill].name.c_str();
			if(unit->data->armor_type == armor.armor_type)
				armor_type = skill;
			else
				armor_type = Format("%s (%s)", skill, txInvalidArmor);

			int old_mob = (int)unit->CalculateMobility();
			int new_mob = (int)unit->CalculateMobility(armor);
			if(old_mob == new_mob)
				mob_str = Format("%d", new_mob);
			else
				mob_str = Format("%d -> %d", old_mob, new_mob);

			int old_def = (int)unit->CalculateDefense();
			int new_def = (int)unit->CalculateDefense(item);
			cstring def_desc;
			if(old_def == new_def)
				def_desc = Format("%d", old_def);
			else
				def_desc = Format("%d -> %d", old_def, new_def);

			str += Format(" - %s\n%s: %d (%s)\n%s: $c%c%d$c-\n%s: %d (%s)\n",
				armor_type,
				txDefense,
				armor.def,
				def_desc,
				txRequiredStrength,
				(unit->Get(AttributeId::STR) >= armor.req_str ? '-' : 'r'),
				armor.req_str,
				txMobility,
				armor.mobility,
				mob_str);
		}
		break;
	case IT_SHIELD:
		{
			/*
			Iron shield
			Defense: 30 (40 -> 50)
			Required strength: $40$
			*/
			const Shield& shield = item->ToShield();

			cstring block_desc;
			int new_block = (int)unit->CalculateDefense(item);
			if(unit->HaveShield())
			{
				int old_block = (int)unit->CalculateBlock();
				if(old_block == new_block)
					block_desc = Format("%d", new_block);
				else
					block_desc = Format("%d -> %d", old_block, new_block);
			}
			else
				block_desc = Format("%d", new_block);

			str += Format("\n%s: %d (%s)\n%s: $c%c%d$c-\n",
				txDefense,
				shield.def,
				block_desc,
				txRequiredStrength,
				(unit->Get(AttributeId::STR) >= shield.req_str ? '-' : 'r'),
				shield.req_str);
		}
		break;
	default:
		str += "\n";
		break;
	}

	// waga
	str += Format(txWeight, item->GetWeight());
	if(count > 1)
		str += Format(" (%g)\n", item->GetWeight()*count);
	else
		str += "\n";

	// cena
	str += Format(txValue, item->value);
}

// dodaj stackuj�cy si� przedmiot do wektora przedmiot�w
bool InsertItemStackable(vector<ItemSlot>& items, ItemSlot& slot)
{
	vector<ItemSlot>::iterator it = std::lower_bound(items.begin(), items.end(), slot, SortItemsPred);
	if(it == items.end())
	{
		// dodaj na samym ko�cu
		items.push_back(slot);
		return false;
	}
	else
	{
		if(it->item == slot.item)
		{
			// stackuj przedmiot
			it->count += slot.count;
			it->team_count += slot.team_count;
			return true;
		}
		else
		{
			// dodaj przedmiot
			items.insert(it, slot);
			return false;
		}
	}
}

// dodaj nie stackuj�cy si� przedmiot do wektora przedmiot�w
void InsertItemNotStackable(vector<ItemSlot>& items, ItemSlot& slot)
{
	vector<ItemSlot>::iterator it = std::lower_bound(items.begin(), items.end(), slot, SortItemsPred);
	uint size = items.size();
	ItemSlot slot2;
	slot2.item = slot.item;
	slot2.count = 1;
	if(slot.team_count)
	{
		slot2.team_count = 1;
		--slot.team_count;
	}
	else
		slot2.team_count = 0;

	if(it == items.end())
	{
		// powi�ksz wektor
		items.reserve(size + slot.count);

		// dodaj na samym ko�cu
		while(slot.count)
		{
			items.push_back(slot2);
			if(slot.team_count)
			{
				slot2.team_count = 1;
				--slot.team_count;
			}
			else
				slot2.team_count = 0;
			--slot.count;
		}
	}
	else
	{
		vector<ItemSlot> v_copy(items.begin(), it);
		v_copy.reserve(items.size() + slot.count);

		// dodaj nowe
		while(slot.count)
		{
			v_copy.push_back(slot2);
			if(slot.team_count)
			{
				slot2.team_count = 1;
				--slot.team_count;
			}
			else
				slot2.team_count = 0;
			--slot.count;
		}

		// dodaj stare z prawej
		for(vector<ItemSlot>::iterator end = items.end(); it != end; ++it)
			v_copy.push_back(*it);

		// zamie�
		items.swap(v_copy);
	}
}

bool InsertItem(vector<ItemSlot>& items, ItemSlot& slot)
{
	assert(slot.item && slot.count && slot.team_count <= slot.count);

	if(slot.item->IsStackable())
		return InsertItemStackable(items, slot);
	else
	{
		InsertItemNotStackable(items, slot);
		return false;
	}
}

void InsertItemBare(vector<ItemSlot>& items, const Item* item, uint count, uint team_count)
{
	assert(item && count && count >= team_count);

	if(item->IsStackable())
	{
		for(vector<ItemSlot>::iterator it = items.begin(), end = items.end(); it != end; ++it)
		{
			if(it->item == item)
			{
				it->count += count;
				it->team_count += team_count;
				return;
			}
		}

		ItemSlot& slot = Add1(items);
		slot.Set(item, count, team_count);
	}
	else
	{
		items.reserve(items.size() + count);
		ItemSlot slot;
		slot.Set(item, 1, 0);
		items.resize(items.size() + count - team_count, slot);
		slot.team_count = 1;
		items.resize(items.size() + team_count, slot);
	}
}
