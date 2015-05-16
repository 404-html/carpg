#include "Pch.h"
#include "Base.h"
#include "PickServerPanel.h"
#include "Language.h"
#include "KeyStates.h"
#include "Const.h"
#include "Game.h"
#include "Wersja.h"

extern const uint g_build;

//=================================================================================================
PickServerPanel::PickServerPanel(const DialogInfo& info) : Dialog(info)
{
	size = INT2(524,340);
	bts.resize(2);

	txUnknownPacket = Str("unknownPacket");
	txUnknownResponse = Str("unknownResponse");
	txUnknownResponse2 = Str("unknownResponse2");
	txBrokenResponse = Str("brokenResponse");

	bts[0].size = INT2(180,44);
	bts[0].pos = INT2(336,30);
	bts[0].id = GuiEvent_Custom+BUTTON_OK;
	bts[0].text = Str("join");
	bts[0].parent = this;

	bts[1].size = INT2(180,44);
	bts[1].pos = INT2(336,80);
	bts[1].id = GuiEvent_Custom+BUTTON_CANCEL;
	bts[1].text = GUI.txCancel;
	bts[1].parent = this;

	grid.pos = INT2(8,8);
	grid.size = INT2(320,300);
	grid.event = GridEvent(this, &PickServerPanel::GetCell);
	grid.AddColumn(Grid::IMGSET, 50);
	grid.AddColumn(Grid::TEXT_COLOR, 100, Str("players"));
	grid.AddColumn(Grid::TEXT_COLOR, 150, Str("name2"));
	grid.Init();
}

//=================================================================================================
void PickServerPanel::Draw(ControlDrawData*)
{
	// t�o
	GUI.DrawSpriteFull(tBackground, COLOR_RGBA(255,255,255,128));

	// panel
	GUI.DrawItem(tDialog, global_pos, size, COLOR_RGBA(255,255,255,222), 16);

	// przyciski
	for(int i=0; i<2; ++i)
		bts[i].Draw();

	// grid
	grid.Draw();
}

//=================================================================================================
void PickServerPanel::Update(float dt)
{
	// update gui
	for(int i=0; i<2; ++i)
	{
		bts[i].mouse_focus = focus;
		bts[i].Update(dt);
	}

	grid.focus = focus;
	grid.Update(dt);

	if(!focus)
		return;

	if(Key.Focus() && Key.PressedRelease(VK_ESCAPE))
	{
		Event((GuiEvent)(GuiEvent_Custom+BUTTON_CANCEL));
		return;
	}

	// ping servers
	ping_timer -= dt;
	if(ping_timer < 0.f)
	{
		ping_timer = 1.f;
		game->peer->Ping("255.255.255.255", (word)game->mp_port, true);
	}

	// listen for packets
	RakNet::Packet* packet;
	for(packet=game->peer->Receive(); packet; game->peer->DeallocatePacket(packet), packet=game->peer->Receive())
	{
		switch(packet->data[0])
		{
		case ID_UNCONNECTED_PONG:
			{
				BitStream s(packet->data+5, packet->length-5, false);
				char sign[2];
				if(!ReadStruct(s, sign))
				{
					WARN(Format("Unknown response from %s: %s.", packet->systemAddress.ToString(), PacketToString(packet)));
					break;
				}

				if(sign[0] != 'C' || sign[1] != 'A')
				{
					WARN(Format("Unknown response from %s, it's not CaRpg server: %s.", packet->systemAddress.ToString(), PacketToString(packet)));
					break;
				}

				uint wersja, build;
				byte gracze, gracze_max, flagi;
				string nazwa;

				if(	!s.Read(wersja) ||
					!s.Read(gracze) ||
					!s.Read(gracze_max) ||
					!s.Read(flagi) ||
					!ReadString1(s, nazwa))
				{
					WARN(Format("Broken response from %s: %s.", packet->systemAddress.ToString(), PacketToString(packet)));
					break;
				}

				if(!s.Read(build))
					build = 0;

				bool valid_version = (wersja == WERSJA && build == g_build);

				// szukaj serwera w bazie
				bool jest = false;
				int index = 0;
				for(vector<ServerData>::iterator it = servers.begin(), end = servers.end(); it != end; ++it, ++index)
				{
					if(it->adr == packet->systemAddress)
					{
						jest = true;
						// aktualizuj
						it->name = nazwa;
						it->players = gracze;
						it->max_players = gracze_max;
						it->flags = flagi;
						it->timer = 0.f;
						it->valid_version = valid_version;

						if(game->pick_autojoin && it->players != it->max_players && it->valid_version)
						{
							bts[0].state = Button::NONE;
							game->pick_autojoin = false;
							grid.selected = index;
							Event(GuiEvent(GuiEvent_Custom+BUTTON_OK));
						}

						break;
					}
				}

				if(!jest)
				{
					// nie ma, dodaj
					ServerData& sd = Add1(servers);
					sd.name = nazwa;
					sd.players = gracze;
					sd.max_players = gracze_max;
					sd.adr = packet->systemAddress;
					sd.flags = flagi;
					sd.timer = 0.f;
					sd.valid_version = valid_version;
					grid.AddItem();

					if(game->pick_autojoin && sd.players != sd.max_players && sd.valid_version)
					{
						bts[0].state = Button::NONE;
						game->pick_autojoin = false;
						grid.selected = servers.size()-1;
						Event(GuiEvent(GuiEvent_Custom+BUTTON_OK));
					}
				}
			}
			break;
		default:
			LOG(Format(txUnknownPacket, packet->data[0], packet->systemAddress.ToString(), PacketToString(packet)));
			break;
		}
	}

	// update servers
	int index = 0;
	for(vector<ServerData>::iterator it = servers.begin(), end = servers.end(); it != end;)
	{
		it->timer += dt;
		if(it->timer >= 2.f)
		{
			grid.RemoveItem(index);
			it = servers.erase(it);
			end = servers.end();
		}
		else
		{
			++it;
			++index;
		}
	}

	if(grid.selected == -1 || !servers[grid.selected].valid_version)
		bts[0].state = Button::DISABLED;
	else if(bts[0].state == Button::DISABLED)
		bts[0].state = Button::NONE;
}

//=================================================================================================
void PickServerPanel::Event(GuiEvent e)
{
	if(e == GuiEvent_Show || e == GuiEvent_WindowResize)
	{
		if(e == GuiEvent_Show)
			visible = true;
		pos = global_pos = (GUI.wnd_size - size)/2;
		for(int i=0; i<2; ++i)
			bts[i].global_pos = global_pos + bts[i].pos;
		grid.Move(global_pos);
	}
	else if(e == GuiEvent_Close)
	{
		visible = false;
		grid.LostFocus();
	}
	else if(e == GuiEvent_LostFocus)
		grid.LostFocus();
	else if(e == GuiEvent_Custom+BUTTON_OK)
		event(BUTTON_OK);
	else if(e == GuiEvent_Custom+BUTTON_CANCEL)
		event(BUTTON_CANCEL);
}

//=================================================================================================
void PickServerPanel::Show()
{
	try
	{
		game->InitClient();
	}
	catch(cstring err)
	{
		GUI.SimpleDialog(err, game->main_menu);
		return;
	}

	LOG("Pinging servers.");
	game->peer->Ping("255.255.255.255", (word)game->mp_port, true);

	ping_timer = 1.f;
	servers.clear();
	grid.Reset();
	
	GUI.ShowDialog(this);
}

//=================================================================================================
void PickServerPanel::GetCell(int item, int column, Cell& cell)
{
	ServerData& server = servers[item];

	if(column == 0)
	{
		vector<TEX>& imgs = *cell.imgset;
		if(IS_SET(server.flags, SERVER_PASSWORD))
			imgs.push_back(game->tIcoHaslo);
		if(IS_SET(server.flags, SERVER_SAVED))
			imgs.push_back(game->tIcoZapis);
	}
	else
	{
		cell.text_color->color = (server.valid_version ? BLACK : RED);
		if(column == 1)
			cell.text_color->text = Format("%d/%d", server.players, server.max_players);
		else
			cell.text_color->text = server.name.c_str();
	}
}
