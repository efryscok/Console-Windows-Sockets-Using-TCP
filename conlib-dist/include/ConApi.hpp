/** @file: ConApi.hpp
	@author Erik Fryscok
	@date 2016-02-05
	@brief Wraps the Windows console api
	@details The ConApi handles the input and output handles so the user doesn't have to and takes c++ datatypes
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#pragma once
#include <ConError.hpp>
#include <ConData.hpp>
#include <ConFlags.hpp>

class ConApi {
private:
	// Member variables
	HANDLE hConInput, hConOutput;

public:
	// Constructor
	ConApi() : hConInput(GetStdHandle(STD_INPUT_HANDLE)), hConOutput(GetStdHandle(STD_OUTPUT_HANDLE)) {}

	// Inline functions
	inline auto fillConsoleOutputAttribute(ConData::ushort_t attribute, ConData::ulong_t length, ConData::Coord writeCoord, ConData::ulong_t &numberOfAttrsWritten) { ConError::throwError(FillConsoleOutputAttribute(hConOutput, attribute, length, writeCoord, &numberOfAttrsWritten)); }
	inline auto fillConsoleOutputCharacter(ConData::achar_t character, ConData::ulong_t length, ConData::Coord writeCoord, ConData::ulong_t &numberOfCharsWritten) { ConError::throwError(FillConsoleOutputCharacterA(hConOutput, character, length, writeCoord, &numberOfCharsWritten)); }
	inline auto fillConsoleOutputCharacter(ConData::uchar_t character, ConData::ulong_t length, ConData::Coord writeCoord, ConData::ulong_t &numberOfCharsWritten) { ConError::throwError(FillConsoleOutputCharacterW(hConOutput, character, length, writeCoord, &numberOfCharsWritten)); }
	inline auto getConsoleCursorInfo(ConData::ConsoleCursorInfo &consoleCursorInfo) { ConError::throwError(GetConsoleCursorInfo(hConOutput, &consoleCursorInfo)); }
	inline auto getConsoleMode(ConData::ulong_t &mode) { ConError::throwError(GetConsoleMode(hConInput, &mode)); }
	inline auto getConsoleScreenBufferInfo(ConData::ConsoleScreenBufferInfo &consoleScreenBufferInfo) { ConError::throwError(GetConsoleScreenBufferInfo(hConOutput, &consoleScreenBufferInfo)); }
	inline auto getConsoleTitle(ConData::string_t &consoleTitle, ConData::ulong_t size) { GetConsoleTitleA(const_cast<LPSTR>(consoleTitle.c_str()), size); }
	inline auto getConsoleTitle(ConData::ustring_t &consoleTitle, ConData::ulong_t size) { GetConsoleTitleW(const_cast<LPWSTR>(consoleTitle.c_str()), size); }
	inline auto readConsoleInput(ConData::inputbuffer_t &buffer, ConData::ulong_t length, ConData::ulong_t &numberOfEventsRead) { ConError::throwError(ReadConsoleInput(hConInput, buffer.data(), length, &numberOfEventsRead)); }
	inline auto readConsoleOutput(ConData::charbuffer_t &buffer, ConData::Coord bufferSize, ConData::Coord bufferCoord, ConData::SmallRect &readRegion) { ConError::throwError(ReadConsoleOutput(hConOutput, buffer.data(), bufferSize, bufferCoord, &readRegion)); }
	inline auto setConsoleControlHandler(ConData::bool_t(&handlerRoutine)(ConData::ulong_t ctrlType), ConData::bool_t add) { ConError::throwError(SetConsoleCtrlHandler((PHANDLER_ROUTINE)handlerRoutine, add)); }
	inline auto setConsoleCursorInfo(const ConData::ConsoleCursorInfo &consoleCursorInfo) { ConError::throwError(SetConsoleCursorInfo(hConOutput, &consoleCursorInfo)); }
	inline auto setConsoleCursorPosition(ConData::Coord cursorPosition) { ConError::throwError(SetConsoleCursorPosition(hConOutput, cursorPosition)); }
	inline auto setConsoleMode(ConData::ulong_t mode) { ConError::throwError(SetConsoleMode(hConInput, mode)); }
	inline auto setConsoleScreenBufferSize(ConData::Coord size) { ConError::throwError(SetConsoleScreenBufferSize(hConOutput, size)); }
	inline auto setConsoleTitle(const ConData::string_t &consoleTitle) { ConError::throwError(SetConsoleTitleA(const_cast<LPCSTR>(consoleTitle.c_str()))); }
	inline auto setConsoleTitle(const ConData::ustring_t &consoleTitle) { ConError::throwError(SetConsoleTitleW(const_cast<LPCWSTR>(consoleTitle.c_str()))); }
	inline auto setConsoleWindowInfo(ConData::bool_t absolute, const ConData::SmallRect &consoleWindow) { ConError::throwError(SetConsoleWindowInfo(hConOutput, absolute, &consoleWindow)); }
	inline auto setCurrentConsoleFontEx(ConData::ConsoleFontInfoEx &consoleFontInfoEx) { ConError::throwError(SetCurrentConsoleFontEx(hConOutput, FALSE, &consoleFontInfoEx)); }
	inline auto writeConsoleOutput(const ConData::charbuffer_t &buffer, ConData::Coord bufferSize, ConData::Coord bufferCoord, ConData::SmallRect &writeRegion) { ConError::throwError(WriteConsoleOutput(hConOutput, buffer.data(), bufferSize, bufferCoord, &writeRegion)); }
};