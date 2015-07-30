// obs�uga zawod�w na arenie
#include "Pch.h"
#include "Game.h"
#include "Quest_Mages.h"

//=================================================================================================
void Game::StartTournament(Unit* arena_master)
{
	zawody_rok = year;
	zawody_stan = IS_ROZPOCZYNANIE;
	zawody_czas = 0.f;
	zawody_stan2 = 0;
	zawody_mistrz = arena_master;
	zawody_ludzie.clear();
	zawody_wygenerowano = false;
	zawody_zwyciezca = NULL;
	zawody_walczacy.clear();
	zawody_niewalczacy = NULL;
}

//=================================================================================================
bool Game::IfUnitJoinTournament(Unit& u)
{
	if(u.IsStanding() && u.IsHero() && u.frozen == 0)
	{
		if(IS_SET(u.data->flags2, F2_TOURNAMENT))
			return true;
		else if(IS_SET(u.data->flags3, F3_DRUNK_MAGE) && quest_mages2->mages_state >= Quest_Mages2::State::MageCured)
		{
			if(!u.IsTeamMember())
			{
				// po wszystkim wr�� do karczmy
				u.ai->goto_inn = true;
			}
			return true;
		}
	}
	return false;
}

//=================================================================================================
void Game::GenerateTournamentUnits()
{
	VEC3 pos = city_ctx->FindBuilding(B_ARENA)->walk_pt;
	zawody_mistrz = FindUnitByIdLocal("arena_master");

	// przenie� heros�w przed aren�
	for(vector<Unit*>::iterator it = local_ctx.units->begin(), end = local_ctx.units->end(); it != end; ++it)
	{
		Unit& u = **it;
		if(IfUnitJoinTournament(u) && !u.IsFollowingTeamMember())
			WarpNearLocation(local_ctx, u, pos, 12.f, false);
	}
	InsideBuilding* inn = city_ctx->FindInn();
	for(vector<Unit*>::iterator it = inn->units.begin(), end = inn->units.end(); it != end;)
	{
		Unit& u = **it;
		if(IfUnitJoinTournament(u) && !u.IsFollowingTeamMember())
		{
			BreakAction(u);
			u.in_building = -1;
			WarpNearLocation(local_ctx, u, pos, 12.f, false);
			local_ctx.units->push_back(&u);
			it = inn->units.erase(it);
			end = inn->units.end();
		}
		else
			++it;
	}

	// generuj heros�w
	int ile = random(6,9);
	for(int i=0; i<ile; ++i)
	{
		Unit* u = SpawnUnitNearLocation(local_ctx, pos, *GetRandomHeroData(), NULL, random(5,20), 12.f);
		if(u)
		{
			u->temporary = true;
			if(IsOnline())
				Net_SpawnUnit(u);
		}
	}

	zawody_wygenerowano = true;
}

//=================================================================================================
void Game::UpdateTournament(float dt)
{
	if(zawody_stan == IS_ROZPOCZYNANIE)
	{
		if(zawody_mistrz->busy == Unit::Busy_No)
			zawody_czas += dt;

		// do��czanie cz�onk�w dru�yny
		const VEC3& walk_pt = city_ctx->FindBuilding(B_ARENA)->walk_pt; 
		for(vector<Unit*>::iterator it = team.begin(), end = team.end(); it != end; ++it)
		{
			Unit& u = **it;
			if(u.busy == Unit::Busy_No && distance2d(u.pos, zawody_mistrz->pos) <= 16.f && !u.dont_attack && IfUnitJoinTournament(u))
			{
				u.busy = Unit::Busy_Tournament;
				u.ai->idle_action = AIController::Idle_Move;
				u.ai->idle_data.pos.Build(walk_pt);
				u.ai->timer = random(5.f,10.f);

				UnitTalk(**it, random_string(txAiJoinTour));
			}
		}

		if(zawody_stan2 == 0)
		{
			if(zawody_czas >= 5.f)
			{
				zawody_stan2 = 1;
				TournamentTalk(txTour[0]);
			}
		}
		else if(zawody_stan2 == 1)
		{
			if(zawody_czas >= 30.f)
			{
				zawody_stan2 = 2;
				TournamentTalk(txTour[1]);
			}
		}
		else if(zawody_stan2 == 2)
		{
			if(zawody_czas >= 60.f)
			{
				// zbierz npc
				for(vector<Unit*>::iterator it = local_ctx.units->begin(), end = local_ctx.units->end(); it != end; ++it)
				{
					Unit& u = **it;
					if(distance2d(u.pos, zawody_mistrz->pos) < 64.f && IfUnitJoinTournament(u))
					{
						zawody_ludzie.push_back(&u);
						u.busy = Unit::Busy_Tournament;
					}
				}

				zawody_stan2 = 3;
				zawody_runda = 0;
				zawody_mistrz->busy = Unit::Busy_Yes;
				TournamentTalk(txTour[2]);
				zawody_niewalczacy = NULL;
				StartTournamentRound();
			}
		}
		else if(zawody_stan2 == 3)
		{
			if(!zawody_mistrz->talking)
			{
				if(!zawody_walczacy.empty())
				{
					zawody_stan2 = 4;
					TournamentTalk(Format(txTour[3], zawody_walczacy.size()*2 + (zawody_niewalczacy ? 1 : 0)));
					SpawnArenaViewers(5);
				}
				else
				{
					zawody_stan2 = 5;
					TournamentTalk(txTour[22]);
				}
			}
		}
		else if(zawody_stan2 == 4)
		{
			if(!zawody_mistrz->talking)
			{
				zawody_stan = IS_TRWAJA;
				zawody_stan2 = 0;
				zawody_stan3 = 0;
			}
		}
		else
		{
			if(!zawody_mistrz->talking)
			{
				zawody_stan = IS_BRAK;
				for(vector<Unit*>::iterator it = local_ctx.units->begin(), end = local_ctx.units->end(); it != end; ++it)
				{
					Unit& u = **it;
					if(u.busy == Unit::Busy_Tournament)
						u.busy = Unit::Busy_No;
				}
				zawody_mistrz->busy = Unit::Busy_No;
			}
		}
	}
	else
	{
		if(zawody_stan2 == 0)
		{
			// gadanie przed walk�
			if(!zawody_mistrz->talking)
			{
				cstring text;
				if(zawody_stan3 == 0)
					text = Format(txTour[4], zawody_runda+1);
				else if(zawody_stan3 <= (int)zawody_walczacy.size())
				{
					std::pair<Unit*, Unit*>& p = zawody_walczacy[zawody_stan3-1];
					text = Format(txTour[5], p.first->GetRealName(), p.second->GetRealName());
				}
				else if(zawody_stan3 == (int)zawody_walczacy.size()+1)
				{
					if(zawody_niewalczacy)
						text = Format(txTour[6], zawody_niewalczacy->GetRealName());
					else
						text = txTour[zawody_runda == 0 ? 7 : 8];
				}
				else
					text = txTour[zawody_runda == 0 ? 7 : 8];

				TournamentTalk(text);

				++zawody_stan3;
				bool koniec = false;
				if(zawody_stan3 == (int)zawody_walczacy.size()+1)
				{
					if(!zawody_niewalczacy)
						koniec = true;
				}
				else if(zawody_stan3 == (int)zawody_walczacy.size()+2)
					koniec = true;

				if(koniec)
				{
					zawody_stan2 = 1;
					zawody_stan3 = 0;
					std::reverse(zawody_walczacy.begin(), zawody_walczacy.end());
				}
			}
		}
		else if(zawody_stan2 == 1)
		{
			if(zawody_stan3 == 0)
			{
				// gadanie przed walk�
				if(!zawody_mistrz->talking)
				{
					std::pair<Unit*, Unit*>& p = zawody_walczacy.back();
					if(p.first && p.second)
						TournamentTalk(Format(txTour[9], p.first->GetRealName(), p.second->GetRealName()));
					else
						TournamentTalk(txTour[10]);
					zawody_stan3 = 1;
				}
			}
			else if(zawody_stan3 == 1)
			{
				// sprawdza czy s� w pobli�u, rozpoczyna walk� lub m�wi �e ich nie ma na arenie
				if(!zawody_mistrz->talking)
				{
					std::pair<Unit*, Unit*> p = zawody_walczacy.back();
					zawody_walczacy.pop_back();
					if(!p.first || !p.second)
					{
						if(p.first)
							zawody_ludzie.push_back(p.first);
						else if(p.second)
							zawody_ludzie.push_back(p.second);
					}
					else if(!p.first->IsStanding() || p.first->frozen != 0 || !(distance2d(p.first->pos, zawody_mistrz->pos) <= 64.f || p.first->in_building == zawody_arena))
					{
						zawody_stan3 = 2;
						zawody_drugi_zawodnik = p.second;
						TournamentTalk(Format(txTour[11], p.first->GetRealName()));
					}
					else if(!p.second->IsStanding() || p.second->frozen != 0 || !(distance2d(p.second->pos, zawody_mistrz->pos) <= 64.f || p.second->in_building == zawody_arena))
					{
						zawody_stan3 = 3;
						zawody_ludzie.push_back(p.first);
						TournamentTalk(Format(txTour[12], p.second->GetRealName(), p.first->GetRealName()));
					}
					else
					{
						// walka
						zawody_mistrz->busy = Unit::Busy_No;
						zawody_stan3 = 4;
						arena_free = false;
						arena_tryb = Arena_Zawody;
						arena_etap = Arena_OdliczanieDoPrzeniesienia;
						arena_t = 0.f;
						at_arena.clear();
						at_arena.push_back(p.first);
						at_arena.push_back(p.second);

						p.first->busy = Unit::Busy_No;
						p.first->frozen = 2;
						if(p.first->IsPlayer())
						{
							p.first->player->arena_fights++;
							if(IsOnline())
								p.first->player->stat_flags |= STAT_ARENA_FIGHTS;
							if(p.first->player == pc)
							{
								fallback_co = FALLBACK_ARENA;
								fallback_t = -1.f;
							}
							else
							{
								NetChangePlayer& c = Add1(net_changes_player);
								c.type = NetChangePlayer::ENTER_ARENA;
								c.pc = p.first->player;
								GetPlayerInfo(c.pc).NeedUpdate();
							}
						}

						p.second->busy = Unit::Busy_No;
						p.second->frozen = 2;
						if(p.second->IsPlayer())
						{
							p.second->player->arena_fights++;
							if(IsOnline())
								p.second->player->stat_flags |= STAT_ARENA_FIGHTS;
							if(p.second->player == pc)
							{
								fallback_co = FALLBACK_ARENA;
								fallback_t = -1.f;
							}
							else
							{
								NetChangePlayer& c = Add1(net_changes_player);
								c.type = NetChangePlayer::ENTER_ARENA;
								c.pc = p.second->player;
								GetPlayerInfo(c.pc).NeedUpdate();
							}
						}
					}
				}
			}
			else if(zawody_stan3 == 2)
			{
				// sprawd� czy drugi zawodnik przyszed�
				if(!zawody_mistrz->talking)
				{
					if(zawody_drugi_zawodnik)
					{
						if(!zawody_drugi_zawodnik->IsStanding() || zawody_drugi_zawodnik->frozen != 0 ||
							!(distance2d(zawody_drugi_zawodnik->pos, zawody_mistrz->pos) <= 64.f || zawody_drugi_zawodnik->in_building == zawody_arena))
						{
							TournamentTalk(Format(txTour[13], zawody_drugi_zawodnik->GetRealName()));
						}
						else
						{
							zawody_ludzie.push_back(zawody_drugi_zawodnik);
							TournamentTalk(Format(txTour[14], zawody_drugi_zawodnik->GetRealName()));
						}
					}

					zawody_stan3 = 3;
				}
			}
			else if(zawody_stan3 == 3)
			{
				// jeden lub obaj zawodnicy nie przyszli
				// lub pogada� po walce
				if(!zawody_mistrz->talking)
				{
					if(zawody_walczacy.empty())
					{
						// nie ma ju� walk w tej rundzie
						if(zawody_ludzie.size() <= 1 && !zawody_niewalczacy)
						{
							// jest zwyci�zca / lub nie ma nikogo
							if(!zawody_ludzie.empty())
								zawody_zwyciezca = zawody_ludzie.back();
							else
								zawody_zwyciezca = zawody_niewalczacy;

							if(!zawody_zwyciezca)
							{
								TournamentTalk(txTour[15]);
								zawody_stan3 = 1;
								AddNews(txTour[17]);
								zawody_stan = IS_BRAK;
								zawody_mistrz->busy = Unit::Busy_No;
							}
							else
							{
								TournamentTalk(Format(txTour[16], zawody_zwyciezca->GetRealName()));
								zawody_stan3 = 0;
								AddNews(Format(txTour[18], zawody_zwyciezca->GetRealName()));
							}

							zawody_ludzie.clear();
							zawody_stan2 = 2;

							RemoveArenaViewers();
							arena_viewers.clear();
						}
						else
						{
							// kolejna runda
							++zawody_runda;
							StartTournamentRound();
							zawody_stan2 = 0;
							zawody_stan3 = 0;
						}
					}
					else
					{
						// kolejna walka
						zawody_stan3 = 0;
					}
				}
			}
			else if(zawody_stan3 == 4)
			{
				// trwa walka, obs�ugiwane przez kod areny
			}
			else if(zawody_stan3 == 5)
			{
				// po walce
				if(!zawody_mistrz->talking && zawody_mistrz->busy == Unit::Busy_No)
				{
					// daj miksturki lecznicze
					static const Item* p1 = FindItem("p_hp");
					static const Item* p2 = FindItem("p_hp2");
					static const Item* p3 = FindItem("p_hp3");
					for(vector<Unit*>::iterator it = at_arena.begin(), end = at_arena.end(); it != end; ++it)
					{
						Unit& u = **it;
						float mhp = u.hpmax - u.hp;
						int given_items = 0;
						if(mhp > 0.f && u.IsAI())
							u.ai->have_potion = 2;
						while(mhp >= 600.f)
						{
							u.AddItem(p3, 1, false);
							mhp -= 600.f;
							++given_items;
						}
						if(mhp >= 400.f)
						{
							u.AddItem(p2, 1, false);
							mhp -= 400.f;
							++given_items;
						}
						while(mhp > 0.f)
						{
							u.AddItem(p1, 1, false);
							mhp -= 200.f;
							++given_items;
						}
						if(u.IsPlayer() && given_items)
						{
							if(u.player == pc)
							{
								if(given_items == 1)
									AddGameMsg3(GMS_ADDED_ITEM);
								else
									AddGameMsg(Format(txGmsAddedItems, given_items), 3.f);
							}
							else
							{
								if(given_items == 1)
									Net_AddedItemMsg(u.player);
								else
								{
									NetChangePlayer& c = Add1(net_changes_player);
									c.type = NetChangePlayer::ADDED_ITEM_MSG;
									c.ile = given_items;
									c.pc = u.player;
									GetPlayerInfo(c.pc).NeedUpdate();
								}
							}
						}
					}

					// zwyci�zca
					Unit* wygrany = at_arena[arena_wynik];
					wygrany->busy = Unit::Busy_Tournament;
					zawody_ludzie.push_back(wygrany);
					TournamentTalk(Format(txTour[rand2()%2 == 0 ? 19 : 20], wygrany->GetRealName()));
					zawody_stan3 = 3;
					at_arena.clear();
				}
			}
		}
		else if(zawody_stan2 == 2)
		{
			if(!zawody_mistrz->talking)
			{
				if(zawody_stan3 == 0)
				{
					const int NAGRODA = 5000;

					zawody_stan3 = 1;
					zawody_mistrz->look_target = zawody_zwyciezca;
					zawody_zwyciezca->gold += NAGRODA;
					if(zawody_zwyciezca->IsHero())
					{
						zawody_zwyciezca->look_target = zawody_mistrz;
						zawody_zwyciezca->hero->LevelUp();
					}
					else
					{
						zawody_zwyciezca->busy = Unit::Busy_No;
						if(zawody_zwyciezca->player != pc)
						{
							NetChangePlayer& c = Add1(net_changes_player);
							c.type = NetChangePlayer::GOLD_MSG;
							c.id = 1;
							c.ile = NAGRODA;
							c.pc = zawody_zwyciezca->player;
							GetPlayerInfo(c.pc).NeedUpdateAndGold();
						}
						else
							AddGameMsg(Format(txGoldPlus, NAGRODA), 3.f);
					}
					TournamentTalk(Format(txTour[21], zawody_zwyciezca->GetRealName()));

					zawody_mistrz->busy = Unit::Busy_No;
					zawody_mistrz->ai->idle_action = AIController::Idle_None;
				}
				else if(zawody_stan3 == 1)
				{
					// koniec zawod�w
					if(zawody_zwyciezca && zawody_zwyciezca->IsHero())
					{
						zawody_zwyciezca->look_target = NULL;
						zawody_zwyciezca->ai->idle_action = AIController::Idle_None;
						zawody_zwyciezca->busy = Unit::Busy_No;
					}
					zawody_mistrz->look_target = NULL;
					zawody_stan = IS_BRAK;
				}
			}
		}
	}
}

//=================================================================================================
void Game::StartTournamentRound()
{
	std::random_shuffle(zawody_ludzie.begin(), zawody_ludzie.end(), myrand);
	zawody_walczacy.clear();

	Unit* first = zawody_niewalczacy;
	for(vector<Unit*>::iterator it = zawody_ludzie.begin(), end = zawody_ludzie.end(); it != end; ++it)
	{
		if(!first)
			first = *it;
		else
		{
			zawody_walczacy.push_back(std::pair<Unit*, Unit*>(first, *it));
			first = NULL;
		}
	}

	zawody_niewalczacy = first;
	zawody_ludzie.clear();
}

//=================================================================================================
void Game::TournamentTalk(cstring text)
{
	UnitTalk(*zawody_mistrz, text);
	game_gui->AddSpeechBubble(VEC3_x0y(GetArena()->exit_area.Midpoint(), 1.5f), text);
}

//=================================================================================================
void Game::TournamentTrain(Unit& u)
{
	zawody_mistrz = NULL;
	Train(u, false, (int)Attribute::STR);
	Train(u, false, (int)Attribute::END);
	Train(u, false, (int)Attribute::DEX);
	if(u.HaveWeapon())
	{
		Train(u, true, (int)Skill::ONE_HANDED_WEAPON);
		Train(u, true, (int)u.GetWeapon().GetInfo().skill);
	}
	if(u.HaveBow())
		Train(u, true, (int)Skill::BOW);
	if(u.HaveShield())
		Train(u, true, (int)Skill::SHIELD);
	if(u.HaveArmor())
		Train(u, true, (int)u.GetArmor().skill);
}

//=================================================================================================
void Game::CleanArena()
{
	InsideBuilding* arena = city_ctx->FindInsideBuilding(B_ARENA);

	// wyrzu� ludzi z areny
	for(vector<Unit*>::iterator it = at_arena.begin(), end = at_arena.end(); it != end; ++it)
	{
		Unit& u = **it;
		u.frozen = 0;
		u.in_arena = -1;
		u.in_building = -1;
		u.busy = Unit::Busy_No;
		if(u.hp <= 0.f)
		{
			u.HealPoison();
			u.live_state = Unit::ALIVE;
		}
		if(u.IsAI())
			u.ai->Reset();
		WarpUnit(u, arena->outside_spawn);
		u.rot = arena->outside_rot;
	}
	RemoveArenaViewers();
	at_arena.clear();
	arena_free = true;
	arena_tryb = Arena_Brak;
}

//=================================================================================================
void Game::CleanTournament()
{
	if(!arena_free)
		CleanArena();

	// zmie� zaj�to��
	for(vector<Unit*>::iterator it = zawody_ludzie.begin(), end = zawody_ludzie.end(); it != end; ++it)
		(*it)->busy = Unit::Busy_No;
	zawody_ludzie.clear();
	for(vector<std::pair<Unit*, Unit*> >::iterator it2 = zawody_walczacy.begin(), end2 = zawody_walczacy.end(); it2 != end2; ++it2)
	{
		it2->first->busy = Unit::Busy_No;
		it2->second->busy = Unit::Busy_No;
	}
	zawody_walczacy.clear();
	if(zawody_niewalczacy)
	{
		zawody_niewalczacy->busy = Unit::Busy_No;
		zawody_niewalczacy = NULL;
	}
	zawody_mistrz->busy = Unit::Busy_No;
	zawody_mistrz = NULL;
	zawody_zwyciezca = NULL;

	zawody_stan = IS_BRAK;
}
