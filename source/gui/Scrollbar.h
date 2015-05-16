// pasek przewijania
#pragma once

//-----------------------------------------------------------------------------
#include "Control.h"

//-----------------------------------------------------------------------------
class Scrollbar : public Control
{
public:
	Scrollbar(bool hscrollbar=false);
	void Draw(ControlDrawData* cdd=NULL);
	void Update(float dt);

	void LostFocus();
	// porusza scrollbar myszk�, zwraca czy cokolwiek si� zmieni�o
	bool ApplyMouseWheel();
	inline void Scrollbar::SetValue(float p)
	{
		offset = float(total-part)*p;
	}
	inline float GetValue() const
	{
		return offset/float(total-part);
	}

	int total, part, change;
	float offset;
	bool clicked, hscrollbar, manual_change;
	INT2 click_pt;
	static TEX tex, tex2;
};
