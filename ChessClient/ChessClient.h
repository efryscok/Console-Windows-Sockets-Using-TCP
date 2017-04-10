#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <conio.h>
#include <string>
#include <thread>
#include <iostream>
#include <ConApp.hpp>
#include <vector>
#include <array>
#include <map>

#include "..\Buffer\Buffer.h"
#include "..\Chess\piece.hpp"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class ChessClient : public ConApp {
private:
	int recvbuflen;
	char recvbuf[DEFAULT_BUFLEN];
	bool isConnected;
	bool selectMove;
	int playerColour;

	WSADATA wsaData;
	SOCKET ConnectSocket;
	addrinfo* result;
	addrinfo* ptr;
	addrinfo hints;
	Buffer buffer;

	std::string message;

	std::array<std::array<piece, 8>, 8> chessBoard;
	std::array<std::array<ConData::short_t, 8>, 8> chessColours;

	ConData::short_t width;
	ConData::short_t height;
	ConData::Coord pieceLocation;
	ConData::Coord moveLocation;
	ConFlags::Attributes outputBackground;
	ConFlags::Attributes outputForeground;
	ConFlags::Attributes inputBackground;
	ConFlags::Attributes inputForeground;

public:
	ChessClient() :
		recvbuflen(DEFAULT_BUFLEN),
		isConnected(false),
		selectMove(false),
		playerColour(-1),
		pieceLocation({ 0,0 }),
		moveLocation({ 0,0 }),
		ConnectSocket(INVALID_SOCKET),
		result(NULL),
		ptr(NULL),
		buffer(0),
		message("N/A"),
		outputBackground(ConFlags::BACK_WHITE),
		outputForeground(ConFlags::BLACK),
		inputBackground(ConFlags::BACK_GREY),
		inputForeground(ConFlags::BLACK)
	{};

	void boardClick(ConData::short_t row, ConData::short_t col);
	void connectServer();
	void createSocket();
	void drawBoard();
	int execute();
	std::string getPieceString(piece pce);
	std::string getSquareString(int row, int col);
	void initializeWinsock();
	void sendingThread();
	void sendMessage();
	void write(std::string text);
	void writeMessage();
} app;