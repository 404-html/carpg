/*
 * Kodowanie Windows-1250, koniec wiersza CR+LF, test: Za��� g�l� ja��
 * Error - Klasy wyj�tk�w do obs�ugi b��d�w
 * Dokumentacja: Patrz plik doc/Error.txt
 * Copyleft (C) 2007 Adam Sawicki
 * Licencja: GNU LGPL
 * Kontakt: mailto:sawickiap@poczta.onet.pl , http://regedit.gamedev.pl/
 */
#pragma once

// Forward declarations
typedef long HRESULT;
#include <windows.h>

namespace common
{

// Klasa bazowa b��d�w
class Error
{
private:
	std::vector<string> m_Msgs;

protected:
	// Use this constructor if you need to compute something before
	// creating first error message and can't call constructor of this
	// base class on the constructor initialization list
	Error() { }

public:
	// Push a message on the stack
	// Use __FILE__ for file and __LINE__ for line
	void Push(const string &msg, const string &file = "", int line = 0);
	// Zwraca pe�ny opis b��du w jednym �a�cuchu
	// Indent - �a�cuch od kt�rego ma si� zaczyna� ka�dy komunikat (wci�cie).
	// EOL - �a�cuch oddzielaj�cy komunikaty (koniec wiersza). Nie do��czany na ko�cu.
	void GetMessage_(string *Msg, const string &Indent = "", const string &EOL = "\r\n") const;

	// General error creation
	Error(const string &msg, const string &file = "", int line = 0)
	{
		Push(msg, file, line);
	}
};

// Tworzy b��d Windows API na podstawie GetLastError
class Win32Error : public Error
{
public:
	Win32Error(const string &msg = "", const string &file = "", int line = 0);
};

} // namespace common

// Use to push a message on the call stack
#define ERR_ADD_FUNC(exception) { (exception).Push(__FUNCSIG__, __FILE__, __LINE__); }
// Guard a function
#define ERR_TRY        { try {
#define ERR_CATCH(msg) } catch(Error &e) { e.Push((msg), __FILE__, __LINE__); throw; } }
#define ERR_CATCH_FUNC } catch(Error &e) { ERR_ADD_FUNC(e); throw; } }
