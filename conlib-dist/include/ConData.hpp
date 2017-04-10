/** @file: ConData.hpp
	@author Erik Fryscok
	@date 2016-02-05
	@brief Contains class and type definitions for the ConApi class
	@details Hides all the structs and typedefs from the Windows console api
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#pragma once
#include <Windows.h>
#include <initializer_list>
#include <vector>
#include <string>

namespace ConData {
	class Coord : public COORD {
	public:
		// Constructors
		Coord() { X = Y = 0; }
		Coord(const COORD& convert) { X = convert.X; Y = convert.Y; }
		Coord(std::initializer_list<SHORT> init);
	};
	class SmallRect : public SMALL_RECT {
	public:
		// Constructors
		SmallRect() { Top = Left = Bottom = Right = 0; }
		SmallRect(const SMALL_RECT& convert) { Top = convert.Top; Left = convert.Left; Bottom = convert.Bottom; Right = convert.Right; }
		SmallRect(std::initializer_list<SHORT> init);
	};
	class ConsoleCursorInfo : public CONSOLE_CURSOR_INFO {};
	class ConsoleFontInfoEx : public CONSOLE_FONT_INFOEX {};
	class ConsoleScreenBufferInfo : public CONSOLE_SCREEN_BUFFER_INFO {};
	class CharInfo : public CHAR_INFO {};
	class InputRecord : public INPUT_RECORD {};

	using bool_t = BOOL;
	using achar_t = CHAR;
	using uchar_t = WCHAR;
	using short_t = SHORT;
	using ushort_t = WORD;
	using ulong_t = DWORD;
	using string_t = std::string;
	using ustring_t = std::wstring;
	using charbuffer_t = std::vector<CharInfo>;
	using inputbuffer_t = std::vector<InputRecord>;

	class ConsoleState {
	public:
		// Member variables
		ConsoleScreenBufferInfo csbi;
		ConsoleCursorInfo cci;
		ConsoleFontInfoEx cfi;
		Coord bufferCoord;
		ulong_t consoleMode;
		charbuffer_t buffer;
		ustring_t title;
	};
}