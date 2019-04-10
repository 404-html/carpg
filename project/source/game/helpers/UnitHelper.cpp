#include "Pch.h"
#include "GameCore.h"
#include "UnitHelper.h"

namespace UnitHelper
{
	inline void EnsureList(const ItemList*& lis)
	{
		if(!lis)
			lis = ItemList::Get("base_items").lis;
	}
}

const Item* UnitHelper::GetBaseWeapon(const Unit& unit, const ItemList* lis)
{
	EnsureList(lis);

	if(IS_SET(unit.data->flags, F_MAGE))
	{
		for(const Item* item : lis->items)
		{
			if(item->type == IT_WEAPON && IS_SET(item->flags, ITEM_MAGE))
				return item;
		}
	}

	WEAPON_TYPE best = unit.GetBestWeaponType();
	for(const Item* item : lis->items)
	{
		if(item->ToWeapon().weapon_type == best)
			return item;
	}

	// never should happen, list is checked in CheckBaseItems
	return nullptr;
}

const Item* UnitHelper::GetBaseArmor(const Unit& unit, const ItemList* lis)
{
	EnsureList(lis);

	if(IS_SET(unit.data->flags, F_MAGE))
	{
		for(const Item* item : lis->items)
		{
			if(item->type == IT_ARMOR && IS_SET(item->flags, ITEM_MAGE))
				return item;
		}
	}

	ARMOR_TYPE armor_type = unit.GetBestArmorType();
	for(const Item* item : lis->items)
	{
		if(item->type == IT_ARMOR && item->ToArmor().armor_type == armor_type)
			return item;
	}

	return nullptr;
}

const Item* UnitHelper::GetBaseItem(ITEM_TYPE type, const ItemList* lis)
{
	EnsureList(lis);

	for(const Item* item : lis->items)
	{
		if(item->type == type)
			return item;
	}

	return nullptr;
}

UnitHelper::BetterItem UnitHelper::GetBetterAmulet(const Unit& unit)
{
	static const LeveledItemList* lis = ItemList::Get("amulet").llis;
	const Item* amulet = unit.slots[SLOT_AMULET];
	const Item* best_item = nullptr;
	float prev_value = amulet ? unit.GetItemAiValue(amulet) : 0.f;
	float best_value = prev_value;
	for(auto& e : lis->items)
	{
		const Item* item = e.item;
		if(item == amulet)
			continue;
		if(item->value > unit.gold)
			continue;
		float value = unit.GetItemAiValue(item);
		if(value > best_value)
		{
			best_item = item;
			best_value = value;
		}
	}
	return { best_item, best_value, prev_value };
}

array<UnitHelper::BetterItem, 2> UnitHelper::GetBetterRings(const Unit& unit)
{
	static const LeveledItemList* lis = ItemList::Get("ring").llis;
	const Item* rings[2] = { unit.slots[SLOT_RING1], unit.slots[SLOT_RING2] };
	float prev_value[2] = {
		rings[0] ? unit.GetItemAiValue(rings[0]) : 0,
		rings[1] ? unit.GetItemAiValue(rings[1]) : 0
	};
	float best_value[2] = { prev_value[0], prev_value[1] };
	const Item* best_item[2] = { nullptr, nullptr };
	for(auto& e : lis->items)
	{
		const Item* item = e.item;
		if(item->value > unit.gold)
			continue;
		if(item == rings[0] && item == rings[1])
			continue;
		float value = unit.GetItemAiValue(item);
		if(value > best_value[0])
		{
			if(best_value[0] > best_value[1])
			{
				best_item[1] = item;
				best_value[1] = value;
			}
			else
			{
				best_item[0] = item;
				best_value[0] = value;
			}
		}
		else if(value > best_value[1])
		{
			best_item[1] = item;
			best_value[1] = value;
		}
	}
	return
	{ {
		{ best_item[0], best_value[0], prev_value[0] },
		{ best_item[1], best_value[1], prev_value[1] }
	} };
}
