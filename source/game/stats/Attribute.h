#pragma once

//-----------------------------------------------------------------------------
enum class Attribute
{
	STR,
	CON,
	DEX,
	/*INT,
	WIS,
	CHA,*/
	MAX,
	NONE
};

//-----------------------------------------------------------------------------
struct AttributeInfo
{
	Attribute attrib_id;
	cstring id;
	string name, desc;

	inline AttributeInfo(Attribute attrib_id, cstring id) : attrib_id(attrib_id), id(id)
	{

	}

	static AttributeInfo* Find(const string& id);
	static void Validate(int& err);
};

//-----------------------------------------------------------------------------
extern AttributeInfo g_attributes[(int)Attribute::MAX];