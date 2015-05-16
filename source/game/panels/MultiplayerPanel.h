#pragma once

//-----------------------------------------------------------------------------
#include "Dialog2.h"
#include "TextBox.h"

//-----------------------------------------------------------------------------
class MultiplayerPanel : public Dialog
{
public:
	MultiplayerPanel(const DialogInfo& info);
	void Draw(ControlDrawData* cdd=NULL);
	void Update(float dt);
	void Event(GuiEvent e);

	void Show();

	enum ButtonId
	{
		IdJoinLan = GuiEvent_Custom,
		IdJoinIp,
		IdCreate,
		IdLoad,
		IdCancel
	};

	TextBox textbox;
	cstring txMultiplayerGame, txNick, txNeedEnterNick, txEnterValidNick;
};
