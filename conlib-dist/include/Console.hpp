/** @file: Console.hpp
	@author Erik Fryscok
	@date 2016-02-05
	@brief Wraps the ConApi class
	@details Makes it easier for the user to use the Windows console api and allows for saving and restoring of console states
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#pragma once
#include <ConApi.hpp>
#undef min
#include <algorithm>

class Console {
private:
	// Member variables
	ConApi api;
	ConData::ConsoleState original;

public:
	// Constructor/Destructor
	Console();
	~Console();

	// Inline functions
	inline auto getCursorPosition() { ConData::ConsoleScreenBufferInfo consoleScreenBufferInfo; api.getConsoleScreenBufferInfo(consoleScreenBufferInfo); return consoleScreenBufferInfo.dwCursorPosition; }
	inline auto getTitleAscii() { ConData::string_t title; api.getConsoleTitle(title, static_cast<ConData::ulong_t>(title.size())); return title; }
	inline auto getTitleUnicode() { ConData::ustring_t title; api.getConsoleTitle(title, static_cast<ConData::ulong_t>(title.size())); return title; }
	inline auto readConsoleInput(ConData::inputbuffer_t &buffer) { ConData::ulong_t numEvents; api.readConsoleInput(buffer, static_cast<ConData::ulong_t>(buffer.size()), numEvents); return numEvents; }
	inline auto readConsoleOutput(ConData::charbuffer_t &buffer, ConData::SmallRect readRegion) { api.readConsoleOutput(buffer, { (ConData::short_t)(readRegion.Right + 1 - readRegion.Left),(ConData::short_t)(readRegion.Bottom + 1 - readRegion.Top) }, { 0 }, readRegion); }
	inline auto setCellAttribute(ConData::ushort_t attribute, ConData::Coord location) { ConData::ulong_t numAttrs; api.fillConsoleOutputAttribute(attribute, 1, location, numAttrs); }
	inline auto setCellAttribute(ConData::ushort_t attribute, ConData::Coord location, ConData::ulong_t length) { api.fillConsoleOutputAttribute(attribute, length, location, length); }
	inline auto setCellCharacter(ConData::achar_t character, ConData::Coord location) { ConData::ulong_t numChars; api.fillConsoleOutputCharacter(character, 1, location, numChars); }
	inline auto setCellCharacter(ConData::uchar_t character, ConData::Coord location) { ConData::ulong_t numChars; api.fillConsoleOutputCharacter(character, 1, location, numChars); }
	inline auto setCellCharacter(ConData::achar_t character, ConData::Coord location, ConData::ulong_t length) { api.fillConsoleOutputCharacter(character, length, location, length); }
	inline auto setCellCharacter(ConData::uchar_t character, ConData::Coord location, ConData::ulong_t length) { api.fillConsoleOutputCharacter(character, length, location, length); }
	inline auto setCellContents(ConData::ushort_t attribute, ConData::achar_t character, ConData::Coord location) { ConData::ulong_t num; api.fillConsoleOutputAttribute(attribute, 1, location, num); api.fillConsoleOutputCharacter(character, 1, location, num); };
	inline auto setCellContents(ConData::ushort_t attribute, ConData::uchar_t character, ConData::Coord location) { ConData::ulong_t num; api.fillConsoleOutputAttribute(attribute, 1, location, num); api.fillConsoleOutputCharacter(character, 1, location, num); };
	inline auto setCellContents(ConData::ushort_t attribute, ConData::achar_t character, ConData::Coord location, ConData::ulong_t length) { api.fillConsoleOutputAttribute(attribute, length, location, length); api.fillConsoleOutputCharacter(character, length, location, length); };
	inline auto setCellContents(ConData::ushort_t attribute, ConData::uchar_t character, ConData::Coord location, ConData::ulong_t length) { api.fillConsoleOutputAttribute(attribute, length, location, length); api.fillConsoleOutputCharacter(character, length, location, length); };
	inline auto setCtrlHandler(ConData::bool_t(&handlerRoutine)(ConData::ulong_t ctrlType)) { api.setConsoleControlHandler(handlerRoutine, true); }
	inline auto setCurrentFont(ConData::ConsoleFontInfoEx consoleFontInfoEx) { api.setCurrentConsoleFontEx(consoleFontInfoEx); }
	inline auto setCursorPosition(ConData::Coord cursorPosition) { api.setConsoleCursorPosition(cursorPosition); }
	inline auto setCursorVisible(ConData::bool_t visible) { ConData::ConsoleCursorInfo newCCI = original.cci; newCCI.bVisible = visible; api.setConsoleCursorInfo(newCCI); }
	inline auto setInputMode(ConData::ulong_t mode) { api.setConsoleMode(mode); }
	inline auto setTitle(ConData::string_t title) { api.setConsoleTitle(title); }
	inline auto setTitle(ConData::ustring_t title) { api.setConsoleTitle(title); }

	// Multi-Step functions
	auto resize(ConData::ushort_t width, ConData::ushort_t height) -> void;
	auto restoreConsoleState(ConData::ConsoleState saveState);
	auto saveConsoleState();
};