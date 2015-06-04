#include "Pch.h"
#include "Base.h"
#include "GameGui.h"
#include "Game.h"
#include "Language.h"

//-----------------------------------------------------------------------------
#define FLT_(x, m) (float(int(x*m))/m)
#define FLT_1(x) FLT_(x, 10)
#define FLT_2(x) FLT_(x, 100)

//-----------------------------------------------------------------------------
static enum class TooltipGroup
{
	Sidebar,
	Buff,
	Invalid = -1
};

//=================================================================================================
GameGui::GameGui() : nd_pass(false), debug_info_size(0, 0), profiler_size(0, 0), use_cursor(false)
{
	txDeath = Str("death");
	txDeathAlone = Str("deathAlone");
	txGameTimeout = Str("gameTimeout");
	txChest = Str("chest");
	txDoor = Str("door");
	txDoorLocked = Str("doorLocked");
	txGamePausedBig = Str("gamePausedBig");
	txPressEsc = Str("pressEsc");
	txMenu = Str("menu");
	txBuffPoison = Str("buffPoison");
	txBuffAlcohol = Str("buffAlcohol");
	txBuffRegeneration = Str("buffRegeneration");
	txBuffNatural = Str("buffNatural");
	txBuffFood = Str("buffFood");

	scrollbar.parent = this;

	focusable2 = true;

	tooltip.Init(TooltipGetText(this, &GameGui::GetTooltip));
}

//=================================================================================================
void GameGui::Draw(ControlDrawData*)
{
	Game& game = Game::Get();

	if(nd_pass)
	{
		if(game.debug_info2)
		{
			Unit& u = *game.pc->unit;
			cstring text = Format("Pos: %g; %g; %g (%d; %d)\nRot: %g %s\nFps: %g", FLT_1(u.pos.x), FLT_1(u.pos.y), FLT_1(u.pos.z), int(u.pos.x/2), int(u.pos.z/2), FLT_2(u.rot),
				kierunek_nazwa_s[AngleToDir(clip(u.rot))], FLT_1(game.fps));
			INT2 s = GUI.default_font->CalculateSize(text);
			debug_info_size = Max(s, debug_info_size);
			GUI.DrawItem(tDialog, INT2(0,0), debug_info_size+INT2(24,24), COLOR_RGBA(255,255,255,128));
			RECT r = {12,12,12+s.x,12+s.y};
			GUI.DrawText(GUI.default_font, text, DT_NOCLIP, BLACK, r);
		}

		const string& str = g_profiler.GetString();
		if(!str.empty())
		{
			INT2 block_size = GUI.default_font->CalculateSize(str) + INT2(24,24);
			profiler_size = Max(block_size, profiler_size);
			GUI.DrawItem(tDialog, INT2(GUI.wnd_size.x-profiler_size.x,0), profiler_size, COLOR_RGBA(255,255,255,128));
			RECT rect = {GUI.wnd_size.x-profiler_size.x+12, 12, GUI.wnd_size.x, GUI.wnd_size.y};
			GUI.DrawText(GUI.default_font, str, DT_LEFT, BLACK, rect);
		}

		tooltip.Draw();
	}
	else
	{
		if(!game.IsAnyoneAlive())
		{
			// ekran �mierci
			if(game.death_screen > 0)
			{
				// czarne t�o
				int color;
				if(game.death_screen == 1)
					color = (int(game.death_fade*255)<<24) | 0x00FFFFFF;
				else
					color = WHITE;

				if((color & 0xFF000000) != 0)
					GUI.DrawSpriteFull(game.tCzern, color);

				// obrazek i tekst
				if(game.death_screen > 1)
				{
					if(game.death_screen == 2)
						color = (int(game.death_fade*255)<<24) | 0x00FFFFFF;
					else
						color = WHITE;

					if((color & 0xFF000000) != 0)
					{
						D3DSURFACE_DESC desc;
						V(game.tRip->GetLevelDesc(0, &desc));

						GUI.DrawSprite(game.tRip, Center(desc.Width, desc.Height), color);

						cstring text = Format(game.death_solo ? txDeathAlone : txDeath, game.pc->kills, game.total_kills-game.pc->kills);
						cstring text2 = Format("%s\n\n%s", text, game.death_screen == 3 ? txPressEsc : "\n");
						RECT rect = { 0, 0, GUI.wnd_size.x, GUI.wnd_size.y };
						GUI.DrawText(GUI.default_font, text2, DT_CENTER|DT_BOTTOM, color, rect);
					}
				}
			}

			return;
		}

		// koniec gry
		if(game.koniec_gry)
		{
			// czarne t�o
			int color;
			if(game.death_fade < 1.f)
				color = (int(game.death_fade*255)<<24) | 0x00FFFFFF;
			else
				color = WHITE;

			GUI.DrawSpriteFull(game.tCzern, color);

			// obrazek
			D3DSURFACE_DESC desc;
			V(game.tEmerytura->GetLevelDesc(0, &desc));
			GUI.DrawSprite(game.tEmerytura, Center(desc.Width, desc.Height), color);

			// tekst
			cstring text = Format(txGameTimeout, game.pc->kills, game.total_kills-game.pc->kills);
			cstring text2 = Format("%s\n\n%s", text, game.death_fade >= 1.f ? txPressEsc : "\n");
			RECT rect = { 0, 0, GUI.wnd_size.x, GUI.wnd_size.y };
			GUI.DrawText(GUI.default_font, text2, DT_CENTER|DT_BOTTOM, color, rect);

			return;
		}

		// celownik
		if(game.pc->unit->action == A_SHOOT)
			GUI.DrawSprite(game.tCelownik, Center(32, 32));

		// obw�dka b�lu
		if(game.pc->dmgc > 0.f)
			GUI.DrawSpriteFull(game.tObwodkaBolu, COLOR_RGBA(255, 255, 255, (int)clamp<float>(game.pc->dmgc/game.pc->unit->hp*5*255, 0.f, 255.f)));

		if(game.debug_info && !game.IsLocal())
			game.debug_info = false;
		if(game.debug_info)
		{
			vector<Unit*>& units = *game.GetContext(*game.pc->unit).units;
			for(vector<Unit*>::iterator it = units.begin(), end = units.end(); it != end; ++it)
			{
				Unit& u = **it;
				if(!u.IsAlive())
					continue;

				VEC3 pos = u.visual_pos;
				pos.y += u.GetUnitHeight();
				if(u.IsAI())
				{
					AIController& ai = *u.ai;
					GUI.DrawText3D(GUI.default_font, Format("%s (%s)\nB:%d, F:%d, LVL:%d\nA:%s %.2f\n%s, %d %.2f %d", u.GetName(), u.data->id, u.busy, u.frozen, u.level,
						str_ai_state[ai.state], ai.timer, str_ai_idle[ai.idle_action], ai.city_wander ? 1 : 0, ai.loc_timer, ai.unit->atak_w_biegu ? 1 : 0),
						DT_OUTLINE, WHITE, pos, max((*it)->GetHpp(), 0.f));
				}
				else
				{
					GUI.DrawText3D(GUI.default_font, Format("%s (%s)\nB:%d, F:%d, A:%d", u.GetName(), u.data->id, u.busy, u.frozen, u.player->action),
						DT_OUTLINE, WHITE, pos, max((*it)->GetHpp(), 0.f));
				}
			}
		}

		// napis nad wybranym obiektem/postaci�
		switch(game.before_player)
		{
		case BP_UNIT:
			if(!game.debug_info)
			{
				Unit* u = game.before_player_ptr.unit;
				bool dont_draw = false;
				for(vector<UnitView>::iterator it = game.unit_views.begin(), end = game.unit_views.end(); it != end; ++it)
				{
					if(it->unit == u)
					{
						if(it->time > UNIT_VIEW_B || it->time < -UNIT_VIEW_A)
							dont_draw = true;
						break;
					}
				}
				if(!dont_draw)
				{
					float hpp;
					if(!u->IsAlive() && !u->IsFollower())
						hpp = -1.f;
					else
						hpp = max(u->GetHpp(), 0.f);
					GUI.DrawText3D(GUI.default_font, u->GetName(), DT_OUTLINE, WHITE, u->GetUnitTextPos(), hpp);
				}
			}
			break;
		case BP_CHEST:
			{
				VEC3 pos = game.before_player_ptr.chest->pos;
				pos.y += 0.75f;
				GUI.DrawText3D(GUI.default_font, txChest, DT_OUTLINE, WHITE, pos);
			}
			break;
		case BP_DOOR:
			{
				VEC3 pos = game.before_player_ptr.door->pos;
				pos.y += 1.75f;
				GUI.DrawText3D(GUI.default_font, game.before_player_ptr.door->locked == LOCK_NONE ? txDoor : txDoorLocked, DT_OUTLINE, WHITE, pos);
			}
			break;
		case BP_ITEM:
			{
				GroundItem& item = *game.before_player_ptr.item;
				Animesh* mesh;
				if(IS_SET(item.item->flags, ITEM_GROUND_MESH))
					mesh = item.item->ani;
				else
					mesh = game.aWorek;
				VEC3 pos = item.pos;
				pos.y += mesh->head.bbox.v2.y;
				cstring text;
				if(item.count == 1)
					text = item.item->name.c_str();
				else
					text = Format("%s (%d)", item.item->name.c_str(), item.count);
				GUI.DrawText3D(GUI.default_font, text, DT_OUTLINE, WHITE, pos);
			}
			break;
		case BP_USEABLE:
			{
				Useable& u = *game.before_player_ptr.useable;
				BaseUsable& bu = g_base_usables[u.type];
				VEC3 pos = u.pos;
				pos.y += u.GetMesh()->head.radius;
				GUI.DrawText3D(GUI.default_font, bu.name, DT_OUTLINE, WHITE, pos);
			}
			break;
		}

		// pobliscy wrogowie / sojusznicy
		if(!game.debug_info)
		{
			for(vector<UnitView>::iterator it = game.unit_views.begin(), end = game.unit_views.end(); it != end; ++it)
			{
				int alpha;

				// 0.0 -> 0.1 niewidoczne
				// 0.1 -> 0.2 alpha 0->255
				// -0.2 -> -0.1 widoczne
				// -0.1 -> 0.0 alpha 255->0
				if(it->time > UNIT_VIEW_A)
				{
					if(it->time > UNIT_VIEW_B)
						alpha = 255;
					else
						alpha = int((it->time-UNIT_VIEW_A)*255*UNIT_VIEW_MUL);
				}
				else if(it->time < 0.f)
				{
					if(it->time < -UNIT_VIEW_A)
						alpha = 255;
					else
						alpha = int(-it->time*255*UNIT_VIEW_MUL);
				}
				else
					alpha = 0;

				if(alpha)
				{
					GUI.DrawText3D(GUI.default_font, it->unit->GetName(), DT_OUTLINE, game.IsEnemy(*it->unit, *game.pc->unit) ? COLOR_RGBA(255, 0, 0, alpha) : COLOR_RGBA(0, 255, 0, alpha),
						it->last_pos, max(it->unit->GetHpp(), 0.f));
				}
			}
		}

		// dymki z tekstem
		DrawSpeechBubbles();

		// dialog
		if(game.dialog_context.dialog_mode)
		{
			INT2 dsize(GUI.wnd_size.x-256-8, 104);
			INT2 offset((GUI.wnd_size.x-dsize.x)/2, 32);
			GUI.DrawItem(tDialog, offset, dsize, 0xAAFFFFFF, 16);

			RECT r = { offset.x+6, offset.y+6, offset.x+dsize.x-12, offset.y+dsize.y-4 };
			if(game.dialog_context.show_choices)
			{
				int off = int(scrollbar.offset);

				// zaznaczenie
				RECT r_img = { offset.x, offset.y+game.dialog_context.choice_selected*GUI.default_font->height-off+6, offset.x+dsize.x-16 };
				r_img.bottom = r_img.top + GUI.default_font->height;
				if(r_img.bottom >= r.top && r_img.top < r.bottom)
				{
					if(r_img.top < r.top)
						r_img.top = r.top;
					else if(r_img.bottom > r.bottom)
						r_img.bottom = r.bottom;
					GUI.DrawSpriteRect(game.tEquipped, r_img, 0xAAFFFFFF);
				}

				// tekst
				LocalString s;
				uint size;
				if(game.IsLocal())
				{
					size = game.dialog_context.choices.size();
					for(uint i = 0; i<game.dialog_context.choices.size(); ++i)
					{
						s += game.dialog_context.choices[i].msg;
						s += '\n';
					}
				}
				else
				{
					size = game.dialog_choices.size();
					for(uint i = 0; i<game.dialog_choices.size(); ++i)
					{
						s += game.dialog_choices[i];
						s += '\n';
					}
				}
				RECT r2 = { r.left, r.top-off, r.right, r.bottom-off };
				GUI.DrawText(GUI.default_font, s, 0, BLACK, r2, &r);

				// pasek przewijania
				scrollbar.Draw();
			}
			else if(game.dialog_context.dialog_text)
				GUI.DrawText(GUI.default_font, game.dialog_context.dialog_text, DT_CENTER|DT_VCENTER, BLACK, r);
		}

		// pobierz buffy
		int buffs;
		if(game.IsLocal())
			buffs = game.pc->unit->GetBuffs();
		else
			buffs = game.GetPlayerInfo(game.pc).buffs;

		/*static bool have_manabar = true;
		if(Key.PressedRelease('B'))
			have_manabar = !have_manabar;*/
		const bool have_manabar = false;

		// healthbar
		float hp_scale = float(GUI.wnd_size.x) / 800;
		MATRIX mat;
		float hpp = clamp(game.pc->unit->hp / game.pc->unit->hpmax, 0.f, 1.f);
		RECT part = { 0, 0, LONG(hpp*256), 16 };
		int hp_offset = (have_manabar ? 35 : 17);
		D3DXMatrixTransformation2D(&mat, NULL, 0.f, &VEC2(hp_scale, hp_scale), NULL, 0.f, &VEC2(0.f, float(GUI.wnd_size.y)-hp_scale*hp_offset));
		if(part.right > 0)
			GUI.DrawSprite2(!IS_SET(buffs, BUFF_POISON) ? tHpBar : tPoisonedHpBar, &mat, &part, NULL, WHITE);
		GUI.DrawSprite2(tBar, &mat, NULL, NULL, WHITE);

		// manabar
		if(have_manabar)
		{
			float mpp = 1.f;
			part.right = LONG(mpp*256);
			D3DXMatrixTransformation2D(&mat, NULL, 0.f, &VEC2(hp_scale, hp_scale), NULL, 0.f, &VEC2(0.f, float(GUI.wnd_size.y)-hp_scale*17));
			if(part.right > 0)
				GUI.DrawSprite2(tManaBar, &mat, &part, NULL, WHITE);
			GUI.DrawSprite2(tBar, &mat, NULL, NULL, WHITE);
		}

		// buffs
		for(BuffImage& img : buff_images)
		{
			D3DXMatrixTransformation2D(&mat, NULL, 0.f, &VEC2(buff_scale, buff_scale), NULL, 0.f, &img.pos);
			GUI.DrawSprite2(img.tex, &mat, NULL, NULL, WHITE);
		}

		float scale;
		int offset;

		int img_size = 64 * GUI.wnd_size.x / 1920;
		offset = img_size + 2;
		scale = float(img_size)/64;

		// shortcuts
		INT2 spos(256.f*hp_scale+offset, GUI.wnd_size.y-offset);
		for(int i = 0; i<10; ++i)
		{
			D3DXMatrixTransformation2D(&mat, NULL, 0.f, &VEC2(scale, scale), NULL, 0.f, &VEC2(float(spos.x), float(spos.y)));
			GUI.DrawSprite2(tShortcut, &mat, NULL, NULL, WHITE);
			spos.x += offset;
		}

		// sidebar
		if(sidebar > 0.f)
		{
			int max = (int)SideButtonId::Max;
			if(game.IsOnline())
				--max;
			int total = offset * max;
			spos.y = GUI.wnd_size.y - (GUI.wnd_size.y - total) / 2 - offset;
			for(int i = 0; i < max; ++i)
			{
				TEX t;
				if(sidebar_state[i] == 0)
					t = tShortcut;
				else if(sidebar_state[i] == 1)
					t = tShortcutHover;
				else
					t = tShortcutDown;
				D3DXMatrixTransformation2D(&mat, NULL, 0.f, &VEC2(scale, scale), NULL, 0.f, &VEC2(float(GUI.wnd_size.x) - sidebar * offset, float(spos.y - i*offset)));
				GUI.DrawSprite2(t, &mat, NULL, NULL, WHITE);
				GUI.DrawSprite2(tSideButton[i], &mat, NULL, NULL, WHITE);
			}
		}

		// �ciemnianie
		if(game.fallback_co != -1)
		{
			int alpha;
			if(game.fallback_t < 0.f)
				alpha = int((1.f+game.fallback_t)*255);
			else
				alpha = int((1.f-game.fallback_t)*255);

			GUI.DrawSpriteFull(game.tCzern, COLOR_RGBA(255,255,255,alpha));
		}
	}
	
	nd_pass = !nd_pass;
}

//=================================================================================================
void GameGui::Update(float dt)
{
	nd_pass = !nd_pass;
	if(!nd_pass)
		return;

	TooltipGroup group = TooltipGroup::Invalid;
	int id = -1;

	UpdateSpeechBubbles(dt);

	if(!GUI.HaveDialog())
	{
		if(Key.PressedRelease(VK_MENU))
			use_cursor = !use_cursor;
	}

	if(Key.Down(VK_ESCAPE))
		use_cursor = false;

	Game& game = Game::Get();
	const bool have_manabar = false;
	float hp_scale = float(GUI.wnd_size.x) / 800;
	int hp_offset = (have_manabar ? 35 : 17);

	// buffs
	int buffs;
	if(game.IsLocal())
		buffs = game.pc->unit->GetBuffs();
	else
		buffs = game.GetPlayerInfo(game.pc).buffs;

	buff_scale = GUI.wnd_size.x / 1024.f;
	float off = buff_scale * 33;
	float buf_posy = float(GUI.wnd_size.y - 5) - off - hp_scale * hp_offset;
	INT2 buff_size(int(buff_scale*32), int(buff_scale*32));

	buff_images.clear();

	if(IS_SET(buffs, BUFF_POISON))
	{
		buff_images.push_back(BuffImage(VEC2(2, buf_posy), game.tBuffPoison, BUFF_POISON));
		buf_posy -= off;
	}

	if(IS_SET(buffs, BUFF_ALCOHOL))
	{
		buff_images.push_back(BuffImage(VEC2(2, buf_posy), game.tBuffAlcohol, BUFF_ALCOHOL));
		buf_posy -= off;
	}

	if(IS_SET(buffs, BUFF_REGENERATION))
	{
		buff_images.push_back(BuffImage(VEC2(2, buf_posy), game.tBuffRegeneration, BUFF_REGENERATION));
		buf_posy -= off;
	}

	if(IS_SET(buffs, BUFF_NATURAL))
	{
		buff_images.push_back(BuffImage(VEC2(2, buf_posy), game.tBuffNatural, BUFF_NATURAL));
		buf_posy -= off;
	}

	if(IS_SET(buffs, BUFF_FOOD))
	{
		buff_images.push_back(BuffImage(VEC2(2, buf_posy), game.tBuffFood, BUFF_FOOD));
		buf_posy -= off;
	}

	for(BuffImage& img : buff_images)
	{
		if(PointInRect(GUI.cursor_pos, INT2(img.pos), buff_size))
		{
			group = TooltipGroup::Buff;
			id = img.id;
			break;
		}
	}

	float scale;
	int offset;

	int img_size = 64 * GUI.wnd_size.x / 1920;
	offset = img_size + 2;
	scale = float(img_size)/64;

	// sidebar
	int max = (int)SideButtonId::Max;
	if(game.IsOnline())
		--max;

	sidebar_state[(int)SideButtonId::Inventory] = (game.inventory->visible ? 2 : 0);
	sidebar_state[(int)SideButtonId::Journal] = (game.journal->visible ? 2 : 0);
	sidebar_state[(int)SideButtonId::Stats] = (game.stats->visible ? 2 : 0);
	sidebar_state[(int)SideButtonId::Team] = (game.team_panel->visible ? 2 : 0);
	sidebar_state[(int)SideButtonId::Minimap] = (game.minimap->visible ? 2 : 0);
	sidebar_state[(int)SideButtonId::Active] = 0;
	sidebar_state[(int)SideButtonId::Talk] = 0;
	sidebar_state[(int)SideButtonId::Menu] = 0;

	bool anything = use_cursor;
	if(game.gp_trade->visible)
		anything = true;
	if(!anything)
	{
		for(int i = 0; i < (int)SideButtonId::Max; ++i)
		{
			if(sidebar_state[i] == 2)
			{
				anything = true;
				break;
			}
		}
	}

	if(anything)
		sidebar += dt * 5;
	else
		sidebar -= dt * 5;
	sidebar = clamp(sidebar, 0.f, 1.f);

	if(sidebar > 0.f && !GUI.HaveDialog())
	{
		int total = offset * max;
		int sposy = GUI.wnd_size.y - (GUI.wnd_size.y - total) / 2 - offset;
		for(int i = 0; i < max; ++i)
		{
			if(PointInRect(GUI.cursor_pos, INT2(int(float(GUI.wnd_size.x) - sidebar * offset), sposy - i * offset), INT2(img_size, img_size)))
			{
				group = TooltipGroup::Sidebar;
				id = i;

				if(sidebar_state[i] == 0)
					sidebar_state[i] = 1;
				if(!GUI.HaveDialog() && Key.PressedRelease(VK_LBUTTON))
				{
					switch((SideButtonId)i)
					{
					case SideButtonId::Menu:
						game.ShowMenu();
						use_cursor = false;
						break;
					case SideButtonId::Team:
						game.ShowPanel(OpenPanel::Team);
						break;
					case SideButtonId::Minimap:
						game.ShowPanel(OpenPanel::Minimap);
						if(game.minimap->visible)
							use_cursor = true;
						break;
					case SideButtonId::Journal:
						game.ShowPanel(OpenPanel::Journal);
						break;
					case SideButtonId::Inventory:
						game.ShowPanel(OpenPanel::Inventory);
						break;
					case SideButtonId::Active:
						game.ShowPanel(OpenPanel::Action);
						break;
					case SideButtonId::Stats:
						game.ShowPanel(OpenPanel::Stats);
						break;
					case SideButtonId::Talk:
						game.mp_box->visible = !game.mp_box->visible;
						break;
					}
				}
			}
		}
	}

	tooltip.Update(dt, (int)group, id);
}

//=================================================================================================
bool GameGui::NeedCursor() const
{
	return Game::Get().dialog_context.dialog_mode || use_cursor;
}

//=================================================================================================
void GameGui::Event(GuiEvent e)
{
	if(e == GuiEvent_Show || e == GuiEvent_WindowResize)
	{
		INT2 dsize(GUI.wnd_size.x-256-8,104);
		INT2 offset((GUI.wnd_size.x-dsize.x)/2, 32);
		scrollbar.size = INT2(12,104);
		scrollbar.global_pos = scrollbar.pos = INT2(dsize.x+offset.x-16, offset.y);
		dialog_pos = offset;
		dialog_size = dsize;
	}
}

//=================================================================================================
void GameGui::DrawSpeechBubbles()
{
	Game& game = Game::Get();

	for(vector<SpeechBubble*>::iterator it = speech_bbs.begin(), end = speech_bbs.end(); it != end; ++it)
	{
		SpeechBubble& sb = **it;

		VEC3 pos;
		if(sb.unit)
			pos = sb.last_pos = sb.unit->GetHeadPoint();
		else
			pos = sb.last_pos;

		if(distance(game.pc->unit->visual_pos, pos) > 20.f || !game.CanSee(game.pc->unit->pos, sb.last_pos))
		{
			sb.visible = false;
			continue;
		}

		INT2 pt;
		if(GUI.To2dPoint(pos, pt))
		{
			sb.visible = true;
			if(sb.time > 0.25f)
			{
				int a1, a2;
				if(sb.time >= 0.5f)
				{
					a1 = 0x80FFFFFF;
					a2 = 0xFFFFFFFF;
				}
				else
				{
					float alpha = (min(sb.time, 0.5f)-0.25f)*4;
					a1 = COLOR_RGBA(255,255,255,int(alpha*0x80));
					a2 = COLOR_RGBA(255,255,255,int(alpha*0xFF));
				}
				if(pt.x < sb.size.x/2)
					pt.x = sb.size.x/2;
				else if(pt.x > GUI.wnd_size.x-sb.size.x/2)
					pt.x = GUI.wnd_size.x-sb.size.x/2;
				if(pt.y < sb.size.y/2)
					pt.y = sb.size.y/2;
				else if(pt.y > GUI.wnd_size.y-sb.size.y/2)
					pt.y = GUI.wnd_size.y-sb.size.y/2;
				RECT rect = {pt.x-sb.size.x/2,pt.y-sb.size.y/2};
				rect.right = rect.left + sb.size.x;
				rect.bottom = rect.top + sb.size.y;

				GUI.DrawItem(game.tBubble, INT2(rect.left, rect.top), sb.size, a1);
				GUI.DrawText(GUI.fSmall, sb.text, DT_CENTER|DT_VCENTER, a2, rect);
			}
		}
		else
			sb.visible = false;
	}
}

//=================================================================================================
void GameGui::UpdateSpeechBubbles(float dt)
{
	bool removes = false;

	dt *= Game::Get().speed;

	for(vector<SpeechBubble*>::iterator it = speech_bbs.begin(), end = speech_bbs.end(); it != end; ++it)
	{
		SpeechBubble& sb = **it;
		sb.length -= dt;
		if(sb.length > 0.f)
		{
			if(sb.visible)
				sb.time = min(sb.time + dt*2, 0.75f);
			else
				sb.time = max(sb.time - dt*2, 0.f);
		}
		else
		{
			sb.time -= dt;
			if(sb.time <= 0.25f)
			{
				if(sb.unit)
				{
					sb.unit->talking = false;
					sb.unit->bubble = NULL;
					// fix na crash, powody dla kt�rych ani jest NULLem nie s� znane :S
					if(sb.unit->ani)
						sb.unit->ani->need_update = true;
				}
				delete *it;
				*it = NULL;
				removes = true;
			}
		}
	}

	if(removes)
		RemoveNullElements(speech_bbs);
}

//=================================================================================================
void GameGui::AddSpeechBubble(Unit* unit, cstring text)
{
	assert(unit && text);

	// usu� stary
	if(unit->bubble)
	{
		RemoveElement(speech_bbs, unit->bubble);
		unit->bubble->text.clear();
	}
	else
		unit->bubble = new SpeechBubble;

	// oblicz rozmiar
	INT2 s = GUI.fSmall->CalculateSize(text);
	int total = s.x;
	int lines = 1 + total/400;

	// dodaj
	unit->bubble->text = text;
	unit->bubble->unit = unit;
	unit->bubble->size = INT2(total/lines+20, s.y*lines+20);
	unit->bubble->time = 0.f;
	unit->bubble->length = 1.5f+float(strlen(text))/20;
	unit->bubble->visible = false;
	unit->bubble->last_pos = unit->GetHeadPoint();

	speech_bbs.push_back(unit->bubble);

	unit->talking = true;
	unit->talk_timer = 0.f;
}

//=================================================================================================
void GameGui::Reset()
{
	speech_bbs.clear();
	Event(GuiEvent_Show);
	use_cursor = false;
	sidebar = 0.f;
}

//=================================================================================================
bool GameGui::UpdateChoice(DialogContext& ctx, int choices)
{
	INT2 dsize(GUI.wnd_size.x-256-8,104);
	INT2 offset((GUI.wnd_size.x-dsize.x)/2, 32+6);
	Game& game = Game::Get();

	// element pod kursorem
	int cursor_choice = -1;
	if(GUI.cursor_pos.x >= offset.x && GUI.cursor_pos.x < offset.x+dsize.x-16 && GUI.cursor_pos.y >= offset.y && GUI.cursor_pos.y < offset.y+dsize.y-12)
	{
		int w = (GUI.cursor_pos.y-offset.y+int(scrollbar.offset))/GUI.default_font->height;
		if(w < choices)
			cursor_choice = w;
	}
	
	// zmiana zaznaczonego elementu myszk�
	if(GUI.cursor_pos != game.dialog_cursor_pos)
	{
		game.dialog_cursor_pos = GUI.cursor_pos;
		if(cursor_choice != -1)
			ctx.choice_selected = cursor_choice;
	}

	// strza�ka w g�r�/d�
	bool moved = false;
	if(ctx.choice_selected != 0 && game.KeyPressedReleaseAllowed(GK_MOVE_FORWARD))
	{
		--ctx.choice_selected;
		moved = true;
	}
	if(ctx.choice_selected != choices-1 && game.KeyPressedReleaseAllowed(GK_MOVE_BACK))
	{
		++ctx.choice_selected;
		moved = true;
	}
	if(moved && choices > 5)
	{
		scrollbar.offset = float(GUI.default_font->height*(ctx.choice_selected-2));
		if(scrollbar.offset < 0.f)
			scrollbar.offset = 0.f;
		else if(scrollbar.offset+scrollbar.part > scrollbar.total)
			scrollbar.offset = float(scrollbar.total-scrollbar.part);
	}

	// wyb�r opcji dialogowej z klawiatury (1,2,3,..,9)
	if(game.AllowKeyboard() && !Key.Down(VK_SHIFT))
	{
		for(int i=0; i<=min(8,choices); ++i)
		{
			if(Key.PressedRelease((byte)'1'+i))
			{
				ctx.choice_selected = i;
				return true;
			}
		}
	}

	// wybieranie enterem/esc/spacj�
	if(game.KeyPressedReleaseAllowed(GK_SELECT_DIALOG))
		return true;
	else if(ctx.dialog_esc != -1 && game.AllowKeyboard() && Key.PressedRelease(VK_ESCAPE))
	{
		ctx.choice_selected = ctx.dialog_esc;
		return true;
	}

	// wybieranie klikni�ciem
	if(game.AllowMouse() && cursor_choice != -1 && Key.PressedRelease(VK_LBUTTON))
	{
		ctx.choice_selected = cursor_choice;
		return true;
	}

	// aktualizacja paska przewijania
	scrollbar.mouse_focus = focus;
	if(Key.Focus() && PointInRect(GUI.cursor_pos, dialog_pos, dialog_size) && scrollbar.ApplyMouseWheel())
		game.dialog_cursor_pos = INT2(-1,-1);
	scrollbar.Update(0.f);

	return false;
}

//=================================================================================================
void GameGui::UpdateScrollbar(int choices)
{
	scrollbar.part = 104-12;
	scrollbar.offset = 0.f;
	scrollbar.total = choices * GUI.default_font->height;
	scrollbar.LostFocus();
}

//=================================================================================================
void GameGui::GetTooltip(TooltipController*, int _group, int id)
{
	TooltipGroup group = (TooltipGroup)_group;

	if(group == TooltipGroup::Invalid)
	{
		tooltip.anything = false;
		return;
	}
	else
	{
		tooltip.anything = true;
		tooltip.img = NULL;
		tooltip.big_text.clear();
		tooltip.small_text.clear();
		
		if(group == TooltipGroup::Buff)
		{
			switch(id)
			{
			case BUFF_POISON:
				tooltip.text = txBuffPoison;
				break;
			case BUFF_ALCOHOL:
				tooltip.text = txBuffAlcohol;
				break;
			case BUFF_REGENERATION:
				tooltip.text = txBuffRegeneration;
				break;
			case BUFF_NATURAL:
				tooltip.text = txBuffNatural;
				break;
			case BUFF_FOOD:
				tooltip.text = txBuffFood;
				break;
			}
		}
		else if(group == TooltipGroup::Sidebar)
		{
			GAME_KEYS gk;
			Game& game = Game::Get();

			switch((SideButtonId)id)
			{
			case SideButtonId::Menu:
				tooltip.text = Format("%s (%s)", txMenu, game.controls->key_text[VK_ESCAPE]);
				return;
			case SideButtonId::Team:
				gk = GK_TEAM_PANEL;
				break;
			case SideButtonId::Minimap:
				gk = GK_MINIMAP;
				break;
			case SideButtonId::Journal:
				gk = GK_JOURNAL;
				break;
			case SideButtonId::Inventory:
				gk = GK_INVENTORY;
				break;
			case SideButtonId::Active:
				gk = GK_ACTION_PANEL;
				break;
			case SideButtonId::Stats:
				gk = GK_STATS;
				break;
			case SideButtonId::Talk:
				gk = GK_TALK_BOX;
				break;
			}

			GameKey& k = GKey[gk];
			if(k[0] == VK_NONE && k[1] == VK_NONE)
				tooltip.text = k.text;
			else if(k[0] != VK_NONE && k[1] != VK_NONE)
				tooltip.text = Format("%s (%s, %s)", k.text, game.controls->key_text[k[0]], game.controls->key_text[k[1]]);
			else
			{
				byte key = (k[0] == VK_NONE ? k[1] : k[0]);
				tooltip.text = Format("%s (%s)", k.text, game.controls->key_text[key]);
			}
		}
	}
}
