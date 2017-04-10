/** @file: Console.cpp
	@author Erik Fryscok
	@date 2016-03-01
	@brief Source code for Console class
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#include <Console.hpp>

using namespace ConData;

// Constructor - Save original console state
Console::Console() {
	api.getConsoleTitle(original.title, static_cast<ulong_t>(original.title.size()));
	api.getConsoleMode(original.consoleMode);
	api.getConsoleScreenBufferInfo(original.csbi);

	original.buffer.resize(original.csbi.dwSize.X * original.csbi.dwSize.Y);

	SmallRect bufferRect = SmallRect();
	bufferRect.Bottom = original.csbi.dwSize.Y - 1;
	bufferRect.Right = original.csbi.dwSize.X - 1;

	api.readConsoleOutput(original.buffer, original.csbi.dwSize, original.bufferCoord, bufferRect);
	api.getConsoleCursorInfo(original.cci);
}

// Destructor - Restore original console state
Console::~Console() {
	api.setConsoleTitle(original.title);
	api.getConsoleMode(original.consoleMode);
	api.setConsoleWindowInfo(true, { 0 });
	api.setConsoleScreenBufferSize(original.csbi.dwSize);
	api.setConsoleWindowInfo(true, original.csbi.srWindow);

	SmallRect bufferRect = SmallRect();
	bufferRect.Bottom = original.csbi.dwSize.Y - 1;
	bufferRect.Right = original.csbi.dwSize.X - 1;

	api.writeConsoleOutput(original.buffer, original.csbi.dwSize, original.bufferCoord, bufferRect);
	api.setConsoleCursorInfo(original.cci);
	api.setConsoleCursorPosition(original.csbi.dwCursorPosition);
}

/**	@fn auto Console::resize(ushort_t width, ushort_t height) -> void
	@brief Resize the console window and buffer
	@param width [in] New width of window and buffer size
	@param height [in] New height of window and buffer size
	*/
auto Console::resize(ushort_t width, ushort_t height) -> void {
	SmallRect sr = SmallRect();
	api.setConsoleWindowInfo(true, sr);

	Coord bufferSize = Coord();
	bufferSize.X = width;
	bufferSize.Y = height;
	api.setConsoleScreenBufferSize(bufferSize);

	ConsoleScreenBufferInfo sbi = ConsoleScreenBufferInfo();
	api.getConsoleScreenBufferInfo(sbi);
	sr.Top = sr.Left = 0;
	width = std::min(static_cast<short_t>(width), sbi.dwMaximumWindowSize.X);
	height = std::min(static_cast<short_t>(height), sbi.dwMaximumWindowSize.Y);
	sr.Right = width - 1;
	sr.Bottom = height - 1;
	api.setConsoleWindowInfo(true, sr);
}

/**	@fn auto Console::restoreConsoleState(ConsoleState saveState) -> void
	@brief Set the state of the console
	@param saveState [in] New console state
	*/
auto Console::restoreConsoleState(ConsoleState saveState) {
	api.setConsoleTitle(saveState.title);
	api.getConsoleMode(saveState.consoleMode);
	api.setConsoleWindowInfo(true, { 0 });
	api.setConsoleScreenBufferSize(saveState.csbi.dwSize);
	api.setConsoleWindowInfo(true, saveState.csbi.srWindow);

	SmallRect bufferRect = SmallRect();
	bufferRect.Bottom = saveState.csbi.dwSize.Y - 1;
	bufferRect.Right = saveState.csbi.dwSize.X - 1;

	api.writeConsoleOutput(saveState.buffer, saveState.csbi.dwSize, saveState.bufferCoord, bufferRect);
	api.setConsoleCursorInfo(saveState.cci);
	api.setConsoleCursorPosition(saveState.csbi.dwCursorPosition);
}

/**	@fn auto Console::saveConsoleState() -> ConsoleState
	@brief Creates a save state of the console
	@return Current console state
	*/
auto Console::saveConsoleState() {
	ConsoleState saveState = ConsoleState();
	api.getConsoleTitle(saveState.title, static_cast<ulong_t>(saveState.title.size()));
	api.getConsoleMode(saveState.consoleMode);
	api.getConsoleScreenBufferInfo(saveState.csbi);

	original.buffer.resize(saveState.csbi.dwSize.X * saveState.csbi.dwSize.Y);

	SmallRect bufferRect = SmallRect();
	bufferRect.Bottom = saveState.csbi.dwSize.Y - 1;
	bufferRect.Right = saveState.csbi.dwSize.X - 1;

	api.readConsoleOutput(saveState.buffer, saveState.csbi.dwSize, saveState.bufferCoord, bufferRect);
	api.getConsoleCursorInfo(saveState.cci);
	return saveState;
}