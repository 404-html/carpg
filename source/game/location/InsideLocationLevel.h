// dane poziomu lokacji
#pragma once

#include "Mapa2.h"
#include "Unit.h"
#include "Chest.h"
#include "Trap.h"
#include "Door.h"
#include "GroundItem.h"
#include "Useable.h"
#include "Object.h"
#include "GameCommon.h"

//-----------------------------------------------------------------------------
struct InsideLocationLevel
{
	Pole* mapa;
	int w, h;
	vector<Unit*> units;
	vector<Chest*> chests;
	vector<Object> objects;
	vector<Light> lights;
	vector<Pokoj> pokoje;
	vector<Trap*> traps;
	vector<Door*> doors;
	vector<GroundItem*> items;
	vector<Useable*> useables;
	vector<Blood> bloods;
	INT2 schody_gora, schody_dol;
	int schody_gora_dir, schody_dol_dir;
	bool schody_dol_w_scianie;

	InsideLocationLevel() : mapa(NULL)
	{

	}
	~InsideLocationLevel();

	inline bool IsInside(int _x, int _y) const
	{
		return _x >= 0 && _y >= 0 && _x < w && _y < h;
	}
	inline bool IsInside(const INT2& _pt) const
	{
		return IsInside(_pt.x, _pt.y);
	}
	inline VEC3 GetRandomPos() const
	{
		return VEC3(random(2.f*w),0,random(2.f*h));
	}
	Pokoj* GetNearestRoom(const VEC3& pos);
	Pokoj* FindEscapeRoom(const VEC3& my_pos, const VEC3& enemy_pos);
	inline int GetRoomId(Pokoj* room) const
	{
		assert(room);
		return (int)(room - &pokoje[0]);
	}
	inline INT2 GetUpStairsFrontTile() const
	{
		return schody_gora + dir_to_pos(schody_gora_dir);
	}
	inline INT2 GetDownStairsFrontTile() const
	{
		return schody_dol + dir_to_pos(schody_dol_dir);
	}
	inline Pokoj* GetRandomRoom()
	{
		return &pokoje[rand2()%pokoje.size()];
	}
	bool GetRandomNearWallTile(const Pokoj& pokoj, INT2& tile, int& rot, bool nocol=false);
	Pokoj& GetFarRoom(bool have_down_stairs);
	Pokoj* GetRoom(const INT2& pt);
	inline Pokoj* GetUpStairsRoom()
	{
		return GetRoom(schody_gora);
	}
	inline Pokoj* GetDownStairsRoom()
	{
		return GetRoom(schody_dol);
	}

	void SaveLevel(HANDLE file, bool local);
	void LoadLevel(HANDLE file, bool local);

	void BuildRefidTable();

	inline Pole& At(const INT2& pt)
	{
		assert(IsInside(pt));
		return mapa[pt(w)];
	}
	int FindRoomId(int cel);

	inline Door* FindDoor(const INT2& pt) const
	{
		for(vector<Door*>::const_iterator it = doors.begin(), end = doors.end(); it != end; ++it)
		{
			if((*it)->pt == pt)
				return *it;
		}
		return NULL;
	}

	inline bool IsTileVisible(const VEC3& pos) const
	{
		INT2 pt = pos_to_pt(pos);
		return IS_SET(mapa[pt(w)].flagi, Pole::F_ODKRYTE);
	}

	// sprawdza czy pole le�y przy �cianie, nie uwzgl�dnia na ukos, nie mo�e by� na kraw�dzi mapy!
	bool IsTileNearWall(const INT2& pt) const;
	bool IsTileNearWall(const INT2& pt, int& dir) const;
};
