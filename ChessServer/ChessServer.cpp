#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <map>
#include <string>
#include <thread>
#include <algorithm>
#include <array>

#include "..\Buffer\Buffer.h"
#include "..\Chess\piece.hpp"

#pragma comment(lib, "Ws2_32.lib")

bool turn = WHITE;
std::array<std::array<piece, 8>, 8> chessBoard;
std::map<int, std::map<int, int>> takenPieces;
std::map<int, int> blackTakenCounters;
std::map<int, int> whiteTakenCounters;

// Global variables used by the chat server
struct addrinfo *result = NULL, hints;
bool isExit = false;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
Buffer buffer(0);
WSADATA wsaData;
SOCKET ListenSocket = INVALID_SOCKET, WhiteSocket = INVALID_SOCKET, BlackSocket = INVALID_SOCKET, AuthSocket = INVALID_SOCKET;

// Methods used by the chat server
void acceptClient();
bool checkDirection(direction dir, int sRow, int sCol, int dRow, int dCol);
bool checkKnight(direction dir, int sRow, int sCol, int dRow, int dCol);
void closeSockets();
void createSockets();
direction getDirection(int sRow, int sCol, int dRow, int dCol);
bool getPromote(int colour);
bool inCheck(int colour);
bool inCheck(int colour, int row, int col);
void initializeWinsock();
void inputLoop();
void processMessage(SOCKET socket);
bool processMove(int srcRow, int srcCol, int destRow, int destCol);
void sendMessage(const SOCKET& skt, const std::string& msg);
void shutdownServer();

// Main
int main() {
	// Initial setup
	initializeWinsock();
	createSockets();

	chessBoard[0][0] = piece(BLACK, ROOK);
	chessBoard[1][0] = piece(BLACK, KNIGHT);
	chessBoard[2][0] = piece(BLACK, BISHOP);
	chessBoard[3][0] = piece(BLACK, QUEEN);
	chessBoard[4][0] = piece(BLACK, KING);
	chessBoard[5][0] = piece(BLACK, BISHOP);
	chessBoard[6][0] = piece(BLACK, KNIGHT);
	chessBoard[7][0] = piece(BLACK, ROOK);
	chessBoard[0][1] = piece(BLACK, PAWN);
	chessBoard[1][1] = piece(BLACK, PAWN);
	chessBoard[2][1] = piece(BLACK, PAWN);
	chessBoard[3][1] = piece(BLACK, PAWN);
	chessBoard[4][1] = piece(BLACK, PAWN);
	chessBoard[5][1] = piece(BLACK, PAWN);
	chessBoard[6][1] = piece(BLACK, PAWN);
	chessBoard[7][1] = piece(BLACK, PAWN);
	chessBoard[0][6] = piece(WHITE, PAWN);
	chessBoard[1][6] = piece(WHITE, PAWN);
	chessBoard[2][6] = piece(WHITE, PAWN);
	chessBoard[3][6] = piece(WHITE, PAWN);
	chessBoard[4][6] = piece(WHITE, PAWN);
	chessBoard[5][6] = piece(WHITE, PAWN);
	chessBoard[6][6] = piece(WHITE, PAWN);
	chessBoard[7][6] = piece(WHITE, PAWN);
	chessBoard[0][7] = piece(WHITE, ROOK);
	chessBoard[1][7] = piece(WHITE, KNIGHT);
	chessBoard[2][7] = piece(WHITE, BISHOP);
	chessBoard[3][7] = piece(WHITE, QUEEN);
	chessBoard[4][7] = piece(WHITE, KING);
	chessBoard[5][7] = piece(WHITE, BISHOP);
	chessBoard[6][7] = piece(WHITE, KNIGHT);
	chessBoard[7][7] = piece(WHITE, ROOK);

	std::cout << "[INFO] Type 'exit' to quit" << std::endl;

	// Create the input thread
	std::thread inputThread(inputLoop);

	// Receive messages from clients
	int iResult = 1;
	do {
		// Current client is the listener so accept new connection
		acceptClient();

		// Otherwise receive a message from the client
		if (recv(WhiteSocket, recvbuf, recvbuflen, 0) > 0) {
			processMessage(WhiteSocket);
		}
		if (recv(BlackSocket, recvbuf, recvbuflen, 0) > 0) {
			processMessage(BlackSocket);
		}
	} while (!isExit && iResult > 0);

	// End of program cleanup
	inputThread.join();
	shutdownServer();
	return EXIT_SUCCESS;
}// Accept a client's connection and add to list of sockets

// Accept a client's connection and add to list of sockets
void acceptClient() {
	if (turn && WhiteSocket == INVALID_SOCKET) {
		WhiteSocket = accept(ListenSocket, NULL, NULL);

		if (WhiteSocket != INVALID_SOCKET) {
			std::string msg = "colour";

			Buffer buffer(0);
			short bufferLength = (sizeof(short) * 3) + (short)msg.length() + (sizeof(short));

			buffer.writeShort16BE(bufferLength);
			buffer.writeStringBE(msg);
			buffer.writeShort16BE(turn);

			int iResult = send(WhiteSocket, buffer.toCharArray(), bufferLength, 0);
			if (iResult == SOCKET_ERROR) {
				std::cout << "[ERROR] send failed: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
				_getch();
				//closesocket(WhiteSocket);
			}
			else {
				std::cout << "[INFO] Bytes Sent: " << iResult << std::endl;
			}

			turn = BLACK;
		}
	}
	else if (!turn && BlackSocket == INVALID_SOCKET){
		BlackSocket = accept(ListenSocket, NULL, NULL);

		if (BlackSocket != INVALID_SOCKET) {
			std::string msg = "colour";

			Buffer buffer(0);
			short bufferLength = (sizeof(short) * 3) + (short)msg.length() + (sizeof(short));

			buffer.writeShort16BE(bufferLength);
			buffer.writeStringBE(msg);
			buffer.writeShort16BE(turn);

			int iResult = send(BlackSocket, buffer.toCharArray(), bufferLength, 0);
			if (iResult == SOCKET_ERROR) {
				std::cout << "[ERROR] send failed: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
				_getch();
				//closesocket(WhiteSocket);
			}
			else {
				std::cout << "[INFO] Bytes Sent: " << iResult << std::endl;
			}

			turn = WHITE;
		}
	}
}

bool checkDirection(direction dir, int sRow, int sCol, int dRow, int dCol) {
	piece sPiece = chessBoard[sRow][sCol];

	switch (dir) {
	case NORTH:
		for (int i = sCol + 1; i <= dCol; ++i) {
			if (sPiece.colour != chessBoard[dRow][i].colour && i == dCol) {
				return true;
			}
			else if (chessBoard[dRow][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case NORTHEAST:
		for (int i = sCol + 1, j = sRow + 1; i <= dCol && j <= dRow; ++i, ++j) {
			if (sPiece.colour != chessBoard[j][i].colour && i == dCol && j == dRow) {
				return true;
			}
			else if (chessBoard[j][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case EAST:
		for (int i = sRow + 1; i <= dRow; ++i) {
			if (sPiece.colour != chessBoard[i][dCol].colour && i == dRow) {
				return true;
			}
			else if (chessBoard[i][dCol].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case SOUTHEAST:
		for (int i = sCol - 1, j = sRow + 1; i >= dCol && j <= dRow; --i, ++j) {
			if (sPiece.colour != chessBoard[j][i].colour && i == dCol && j == dRow) {
				return true;
			}
			else if (chessBoard[j][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case SOUTH:
		for (int i = sCol - 1; i >= dCol; --i) {
			if (sPiece.colour != chessBoard[dRow][i].colour && i == dCol) {
				return true;
			}
			else if (chessBoard[dRow][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case SOUTHWEST:
		for (int i = sCol - 1, j = sRow - 1; i >= dCol && j >= dRow; --i, --j) {
			if (sPiece.colour != chessBoard[j][i].colour && i == dCol && j == dRow) {
				return true;
			}
			else if (chessBoard[j][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case WEST:
		for (int i = sRow - 1; i >= dRow; --i) {
			if (sPiece.colour != chessBoard[i][dCol].colour && i == dRow) {
				return true;
			}
			else if (chessBoard[i][dCol].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	case NORTHWEST:
		for (int i = sCol + 1, j = sRow - 1; i <= dCol && j >= dRow; ++i, --j) {
			if (sPiece.colour != chessBoard[j][i].colour && i == dCol && j == dRow) {
				return true;
			}
			else if (chessBoard[j][i].empty()) {
				continue;
			}
			else {
				return false;
			}
		}
		break;
	}

	return false;
}

bool checkKnight(direction dir, int sRow, int sCol, int dRow, int dCol) {
	switch (dir) {
	case NORTHEAST:
		if ((sCol + 2 == dCol && sRow + 1 == dRow) || (sCol + 1 == dCol && sRow + 2 == dRow)) {
			return true;
		}
		break;
	case SOUTHEAST:
		if ((sCol - 2 == dCol && sRow + 1 == dRow) || (sCol - 1 == dCol && sRow + 2 == dRow)) {
			return true;
		}
		break;
	case NORTHWEST:
		if ((sCol + 2 == dCol && sRow - 1 == dRow) || (sCol + 1 == dCol && sRow - 2 == dRow)) {
			return true;
		}
		break;
	case SOUTHWEST:
		if ((sCol - 2 == dCol && sRow - 1 == dRow) || (sCol - 1 == dCol && sRow - 2 == dRow)) {
			return true;
		}
		break;
	}

	return false;
}

// Close all client sockets connected to the server
void closeSockets() {
	closesocket(ListenSocket);
	closesocket(WhiteSocket);
	closesocket(BlackSocket);
	closesocket(AuthSocket);
}

// Create the listening socket and bind it
void createSockets() {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		std::cout << "[ERROR] getaddrinfo failed: " << std::endl << "Press any key to continue . . . ";
		_getch();
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] Resolved server address and port" << std::endl;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "[ERROR] socket failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		freeaddrinfo(result);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] Created listening socket" << std::endl;
	}

	u_long iMode = 1;
	ioctlsocket(ListenSocket, FIONBIO, &iMode);

	// Setup the TCP listening socket
	if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		std::cout << "[ERROR] bind failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] Bound the listening socket" << std::endl;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "[ERROR] listen failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		closesocket(ListenSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] Successfully setup the listening socket" << std::endl;
	}
}

direction getDirection(int sRow, int sCol, int dRow, int dCol) {
	if (dRow == sRow && dCol > sCol) {
		return NORTH;
	}
	if (dCol > sCol && dRow > sRow) {
		return NORTHEAST;
	}
	if (dCol == sCol && dRow > sRow) {
		return EAST;
	}
	if (dCol < sCol && dRow > sRow) {
		return SOUTHEAST;
	}
	if (dRow == sRow && dCol < sCol) {
		return SOUTH;
	}
	if (dCol < sCol && dRow < sRow) {
		return SOUTHWEST;
	}
	if (dCol == sCol && dRow < sRow) {
		return WEST;
	}
	if (dCol > sCol && dRow < sRow) {
		return NORTHWEST;
	}
	return NONE;
}

bool getPromote(int colour) {
	for (int i = KING; i <= PAWN; ++i) {
		if (takenPieces[colour][i] > 0) {
			--takenPieces[colour][i];
			if (colour) {
				--blackTakenCounters[i];
			}
			else {
				--whiteTakenCounters[i];
			}
			return i;
		}
	}
	return 5;
}

bool inCheck(int colour) {
	for (int r = 0; r < 8; ++r) {
		for (int c = 0; c < 8; ++c) {
			if (chessBoard[r][c].colour == colour && chessBoard[r][c].type == KING) {
				return inCheck(colour, r, c);
			}
		}
	}
	return true;
}

bool inCheck(int colour, int row, int col) {
	int kRow, kCol;

	// Left
	for (int r = row - 1; r >= 0; --r) {
		if (chessBoard[r][col].empty()) {
			continue;
		}
		else if (chessBoard[r][col].colour != colour && (chessBoard[r][col].type == QUEEN || chessBoard[r][col].type == ROOK)) {
			return true;
		}
		else {
			break;
		}
	}

	// Right
	for (int r = row + 1; r <= 7; ++r) {
		if (chessBoard[r][col].empty()) {
			continue;
		}
		else if (chessBoard[r][col].colour != colour && (chessBoard[r][col].type == QUEEN || chessBoard[r][col].type == ROOK)) {
			return true;
		}
		else {
			break;
		}
	}

	// Up
	for (int c = col + 1; c <= 7; ++c) {
		if (chessBoard[row][c].empty()) {
			continue;
		}
		else if (chessBoard[row][c].colour != colour && (chessBoard[row][c].type == QUEEN || chessBoard[row][c].type == ROOK)) {
			return true;
		}
		else {
			break;
		}
	}

	// Down
	for (int c = col - 1; c >= 0; --c) {
		if (chessBoard[row][c].empty()) {
			continue;
		}
		else if (chessBoard[row][c].colour != colour && (chessBoard[row][c].type == QUEEN || chessBoard[row][c].type == ROOK)) {
			return true;
		}
		else {
			break;
		}
	}

	// LeftUp
	for (int c = col + 1, r = row - 1; c <= 7 && r >= 0; ++c, --r) {
		if (chessBoard[r][c].empty()) {
			continue;
		}
		else if (chessBoard[r][c].colour != colour && (chessBoard[r][c].type == QUEEN || chessBoard[r][c].type == BISHOP)) {
			return true;
		}
		else {
			break;
		}
	}

	// RightUp
	for (int c = col + 1, r = row + 1; c <= 7 && r <= 7; ++c, ++r) {
		if (chessBoard[r][c].empty()) {
			continue;
		}
		else if (chessBoard[r][c].colour != colour && (chessBoard[r][c].type == QUEEN || chessBoard[r][c].type == BISHOP)) {
			return true;
		}
		else {
			break;
		}
	}

	// LeftDown
	for (int c = col - 1, r = row - 1; c >= 0 && r >= 0; --c, --r) {
		if (chessBoard[r][c].empty()) {
			continue;
		}
		else if (chessBoard[r][c].colour != colour && (chessBoard[r][c].type == QUEEN || chessBoard[r][c].type == BISHOP)) {
			return true;
		}
		else {
			break;
		}
	}

	// RightDown
	for (int c = col - 1, r = row + 1; c >= 0 && r <= 7; --c, ++r) {
		if (chessBoard[r][c].empty()) {
			continue;
		}
		else if (chessBoard[r][c].colour != colour && (chessBoard[r][c].type == QUEEN || chessBoard[r][c].type == BISHOP)) {
			return true;
		}
		else {
			break;
		}
	}

	// Knight Up 2 Right 1
	kCol = col + 2;
	kRow = row + 1;
	if (kRow <= 7 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Right 2 Up 1
	kRow = row + 2;
	kCol = col + 1;
	if (kRow <= 7 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Right 2 Down 1
	kRow = row + 2;
	kCol = col - 1;
	if (kRow <= 7 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Down 2 Right 1
	kCol = col - 2;
	kRow = row + 1;
	if (kRow <= 7 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Down 2 Left 1
	kCol = col - 2;
	kRow = row - 1;
	if (kRow >= 0 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Left 2 Down 1
	kRow = row - 2;
	kCol = col - 1;
	if (kRow >= 0 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Left 2 Up 1
	kRow = row - 2;
	kCol = col + 1;
	if (kRow >= 0 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	// Knight Up 2 Left 1
	kRow = row - 1;
	kCol = col + 2;
	if (kRow >= 0 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KNIGHT) {
		return true;
	}

	if (row + 1 <= 7 && col + 1 <= 7 && chessBoard[row + 1][col + 1].colour != colour && chessBoard[row + 1][col + 1].type == PAWN) {
		return true;
	}

	if (row - 1 >= 0 && col + 1 <= 7 && chessBoard[row - 1][col + 1].colour != colour && chessBoard[row - 1][col + 1].type == PAWN) {
		return true;
	}

	if (col == 4 && chessBoard[row][6].colour != colour && chessBoard[row][6].type == PAWN) {
		return true;
	}

	// King Up
	kCol = col + 1;
	if (kCol <= 7 && chessBoard[row][kCol].colour != colour && chessBoard[row][kCol].type == KING) {
		return true;
	}

	// King Down
	kCol = col - 1;
	if (kCol >= 0 && chessBoard[row][kCol].colour != colour && chessBoard[row][kCol].type == KING) {
		return true;
	}

	// King Right
	kRow = row + 1;
	if (kRow <= 7 && chessBoard[kRow][col].colour != colour && chessBoard[kRow][col].type == KING) {
		return true;
	}

	// King Left
	kRow = row - 1;
	if (kRow >= 0 && chessBoard[kRow][col].colour != colour && chessBoard[kRow][col].type == KING) {
		return true;
	}

	// King UpRight
	kRow = row + 1;
	kCol = col + 1;
	if (kRow <= 7 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KING) {
		return true;
	}

	// King UpLeft
	kRow = row - 1;
	kCol = col + 1;
	if (kRow >= 0 && kCol <= 7 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KING) {
		return true;
	}

	// King DownRight
	kRow = row + 1;
	kCol = col - 1;
	if (kRow <= 7 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KING) {
		return true;
	}

	// King DownLeft
	kRow = row - 1;
	kCol = col - 1;
	if (kRow >= 0 && kCol >= 0 && chessBoard[kRow][kCol].colour != colour && chessBoard[kRow][kCol].type == KING) {
		return true;
	}

	return false;
}

// Initialize Winsock
void initializeWinsock() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "[ERROR] WSAStartup failed" << std::endl << "Press any key to continue . . . ";
		_getch();
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "[INFO] WinSock initialized" << std::endl;
	}
}

// Loop thread function used to quit the program
void inputLoop() {
	while (!isExit) {
		std::string message;
		std::getline(std::cin, message);

		if (message == "exit") {
			isExit = true;
		}
	}
}

void processMessage(SOCKET socket) {
	// Clear and load the buffer with the message
	buffer.clear();
	buffer.loadBuffer(recvbuf, 512);

	// Read data from buffer object
	buffer.readShort16BE();
	std::string messageString = buffer.readStringBE(buffer.readShort16BE());
	int srcRow = buffer.readShort16BE();
	int srcCol = buffer.readShort16BE();
	int destRow = buffer.readShort16BE();
	int destCol = buffer.readShort16BE();

	bool moveIsGood = false;

	// Send the message and determine the state of the client
	if (messageString != "connected") {
		if (turn && socket == WhiteSocket) {
			moveIsGood = processMove(srcRow, srcCol, destRow, destCol);
		}
		else if (!turn && socket == BlackSocket) {
			moveIsGood = processMove(srcRow, srcCol, destRow, destCol);
		}
		else {
			sendMessage(socket, "Not your turn");
		}
	}

	if (moveIsGood) {
		piece tmp = chessBoard[destRow][destCol];
		chessBoard[destRow][destCol] = chessBoard[srcRow][srcCol];
		chessBoard[srcRow][srcCol] = piece();
		if (!inCheck(chessBoard[destRow][destCol].colour)) {
			if (!tmp.empty()) {
				++takenPieces[tmp.colour][tmp.type];
				if (tmp.colour) {
					++blackTakenCounters[tmp.type];
				}
				else {
					++whiteTakenCounters[tmp.type];
				}
			}
			chessBoard[destRow][destCol].hasMoved = true;
			turn = !turn;
			if (turn == WHITE) {
				for (int row = 0; row < 8; ++row) {
					chessBoard[row][5].canTakeInPassing = false;
				}
			}
			else {
				for (int row = 0; row < 8; ++row) {
					chessBoard[row][2].canTakeInPassing = false;
				}
			}
		}

		sendMessage(WhiteSocket, messageString);
		sendMessage(BlackSocket, messageString);
	}
}

bool processMove(int srcRow, int srcCol, int destRow, int destCol) {
	direction dir = getDirection(srcRow, srcCol, destRow, destCol);
	piece srcPiece = chessBoard[srcRow][srcCol];
	piece destPiece = chessBoard[destRow][destCol];
	int castleCol = srcPiece.colour * 7;

	switch (srcPiece.type) {
	case KING:
		if (!inCheck(srcPiece.colour, destRow, destCol)) {
			if ((srcPiece.colour != destPiece.colour) && (destCol == srcCol + 1 || destCol == srcCol - 1 || destCol == srcCol) && (destRow == srcRow + 1 || destRow == srcRow - 1 || destRow == srcRow)) {
				return true;
			}
			else if (!srcPiece.hasMoved && !chessBoard[7][castleCol].hasMoved && destCol == castleCol && destRow == 6 && chessBoard[5][castleCol].empty() && chessBoard[6][castleCol].empty() && !inCheck(srcPiece.colour, srcRow, srcCol) && !inCheck(srcPiece.colour, 5, castleCol)) {
				chessBoard[5][castleCol] = piece(srcPiece.colour, ROOK, true);
				chessBoard[7][castleCol] = piece();
				return true;
			}
			else if (!srcPiece.hasMoved && !chessBoard[0][castleCol].hasMoved && destCol == castleCol && destRow == 2 && chessBoard[2][castleCol].empty() && chessBoard[3][castleCol].empty() && !inCheck(srcPiece.colour, srcRow, srcCol) && !inCheck(srcPiece.colour, 2, castleCol) && !inCheck(srcPiece.colour, 3, castleCol)) {
				chessBoard[0][castleCol] = piece();
				chessBoard[1][castleCol] = piece();
				chessBoard[3][castleCol] = piece(srcPiece.colour, ROOK, true);
				return true;
			}
		}
		break;
	case QUEEN:
		return checkDirection(dir, srcRow, srcCol, destRow, destCol);
		break;
	case ROOK:
		if (dir == NORTH || dir == EAST || dir == SOUTH || dir == WEST) {
			return checkDirection(dir, srcRow, srcCol, destRow, destCol);
		}
		break;
	case BISHOP:
		if (dir == NORTHEAST || dir == SOUTHEAST || dir == NORTHWEST || dir == SOUTHWEST) {
			return checkDirection(dir, srcRow, srcCol, destRow, destCol);
		}
		break;
	case KNIGHT:
		if (srcPiece.colour != destPiece.colour) {
			return checkKnight(dir, srcRow, srcCol, destRow, destCol);
		}
		break;
	case PAWN:
		if (destCol == srcCol + ((srcPiece.colour == WHITE) ? -1 : 1) && (((destPiece.empty() && destRow == srcRow)) || (srcPiece.colour != destPiece.colour && (destRow == srcRow + 1 || destRow == srcRow - 1) && !destPiece.empty()))) {
			if (destCol == 0 || destCol == 7) {
				if (!inCheck(srcPiece.colour)) {
					chessBoard[srcRow][srcCol] = piece(srcPiece.colour, getPromote(srcPiece.colour), true);
					return true;
				}
			}
			return true;
		}
		else if (destCol == srcCol + ((srcPiece.colour == WHITE) ? -2 : 2) && destCol == ((srcPiece.colour == WHITE) ? 4 : 3) && (srcPiece.colour != destPiece.colour || (destPiece.empty() && destRow == srcRow))) {
			chessBoard[destRow][destCol + ((srcPiece.colour == WHITE) ? 1 : -1)].canTakeInPassing = true;
			return true;
		}
		else if (destCol == srcCol + ((srcPiece.colour == WHITE) ? -1 : 1) && destPiece.empty() && chessBoard[srcRow - (srcRow - destRow)][((srcPiece.colour == WHITE) ? 3 : 4)].colour != srcPiece.colour && chessBoard[destRow][destCol].canTakeInPassing == true && destRow == srcRow - (srcRow - destRow)) {
			++takenPieces[chessBoard[srcRow - (srcRow - destRow)][4].colour][chessBoard[srcRow - (srcRow - destRow)][4].type];
			if (chessBoard[srcRow - (srcRow - destRow)][4].colour) {
				++blackTakenCounters[chessBoard[srcRow - (srcRow - destRow)][4].type];
			}
			else {
				++whiteTakenCounters[chessBoard[srcRow - (srcRow - destRow)][4].type];
			}
			chessBoard[destRow][destCol - ((srcPiece.colour == WHITE) ? -1 : 1)] = piece();
			//chessboardButtons[destRow * 8 + (destCol - ((srcPiece.colour == WHITE) ? 1 : -1))]->Text = Convert::ToString(wchar_t(chessBoard[destRow][destCol - ((srcPiece.colour == WHITE) ? 1 : -1)].unicode));
			if (!inCheck(srcPiece.colour)) {
				return true;
			}
			else {
				--takenPieces[chessBoard[srcRow - (srcRow - destRow)][4].colour][chessBoard[srcRow - (srcRow - destRow)][4].type];
				if (chessBoard[srcRow - (srcRow - destRow)][4].colour) {
					--blackTakenCounters[chessBoard[srcRow - (srcRow - destRow)][4].type];
				}
				else {
					--whiteTakenCounters[chessBoard[srcRow - (srcRow - destRow)][4].type];
				}
				chessBoard[srcRow - (srcRow - destRow)][((srcPiece.colour == WHITE) ? 3 : 4)] = srcPiece;
			}
		}
		break;
	default:
		break;
	}

	return false;
}

// Send the received message to all relevant clients
void sendMessage(const SOCKET& skt, const std::string& msg) {
	// Setup buffer
	Buffer buffer(0);
	short bufferLength = (sizeof(short) * 2) + (short)msg.length() + (sizeof(short) * 64) * 2;

	// Write to the buffer
	buffer.writeShort16BE(bufferLength);
	buffer.writeStringBE(msg);
	
	for (int iRow = 0; iRow < 8; ++iRow) {
		for (int iCol = 0; iCol < 8; ++iCol) {
			buffer.writeShort16BE(chessBoard[iRow][iCol].unicode);
		}
	}

	// Send the buffer
	int iResult = send(WhiteSocket, buffer.toCharArray(), bufferLength, 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "[ERROR] send failed: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		//closesocket(WhiteSocket);
	}
	else {
		std::cout << "[INFO] Bytes Sent: " << iResult << std::endl;
	}

	iResult = send(BlackSocket, buffer.toCharArray(), bufferLength, 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "[ERROR] send failed: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		//closesocket(BlackSocket);
	}
	else {
		std::cout << "[INFO] Bytes Sent: " << iResult << std::endl;
	}
}

// Close all sockets, cleanup winsock and exit program
void shutdownServer() {
	// Shutdown the connection since we're done
	if (shutdown(WhiteSocket, SD_SEND) == SOCKET_ERROR) {
		std::cout << "[ERROR] shutdown failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		closeSockets();
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	if (shutdown(BlackSocket, SD_SEND) == SOCKET_ERROR) {
		std::cout << "[ERROR] shutdown failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		closeSockets();
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	if (shutdown(AuthSocket, SD_SEND) == SOCKET_ERROR) {
		std::cout << "[ERROR] shutdown failed with error: " << WSAGetLastError() << std::endl << "Press any key to continue . . . ";
		_getch();
		closeSockets();
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// Cleanup
	closeSockets();
	WSACleanup();
	exit(EXIT_SUCCESS);
}