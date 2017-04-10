#include "ChessClient.h"

// main()
int ChessClient::execute() {
	ConData::ConsoleFontInfoEx cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 30;
	cfi.dwFontSize.Y = 30;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = 0;
	std::wcscpy(cfi.FaceName, L"NSimSun");
	setCurrentFont(cfi);

	// Setup the window size
	width = 26;
	height = 26;
	resize(width, height);

	// Setup chat window
	setCellAttribute(this->outputBackground, { 0,0 }, width * height);
	setCursorVisible(false);

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

	ConData::short_t darkColour = ConFlags::BACK_GREY;
	ConData::short_t lightColour = ConFlags::BACK_WHITE;

	bool startDark = true;

	for (int iCol = 0; iCol < 8; ++iCol) {
		if (startDark) {
			chessColours[0][iCol] = lightColour;
			chessColours[1][iCol] = darkColour;
			chessColours[2][iCol] = lightColour;
			chessColours[3][iCol] = darkColour;
			chessColours[4][iCol] = lightColour;
			chessColours[5][iCol] = darkColour;
			chessColours[6][iCol] = lightColour;
			chessColours[7][iCol] = darkColour;
			startDark = false;
		}
		else {
			chessColours[0][iCol] = darkColour;
			chessColours[1][iCol] = lightColour;
			chessColours[2][iCol] = darkColour;
			chessColours[3][iCol] = lightColour;
			chessColours[4][iCol] = darkColour;
			chessColours[5][iCol] = lightColour;
			chessColours[6][iCol] = darkColour;
			chessColours[7][iCol] = lightColour;
			startDark = true;
		}
	}

	drawBoard();

	setCellCharacter('a', { 1,24 });
	setCellCharacter('b', { 4,24 });
	setCellCharacter('c', { 7,24 });
	setCellCharacter('d', { 10,24 });
	setCellCharacter('e', { 13,24 });
	setCellCharacter('f', { 16,24 });
	setCellCharacter('g', { 19,24 });
	setCellCharacter('h', { 22,24 });

	setCellCharacter('8', { 24,1 });
	setCellCharacter('7', { 24,4 });
	setCellCharacter('6', { 24,7 });
	setCellCharacter('5', { 24,10 });
	setCellCharacter('4', { 24,13 });
	setCellCharacter('3', { 24,16 });
	setCellCharacter('2', { 24,19 });
	setCellCharacter('1', { 24,22 });

	isConnected = true;
	std::thread sendThread(&ChessClient::sendingThread, &app);

	// Initial setup
	initializeWinsock();
	createSocket();
	connectServer();
	message = "connected";
	sendMessage();

	// Loop for receiving messages from the server
	int iResult = 0;
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

		// Check the result of the receive before attempting to write it to console
		if (iResult > 0) {
			writeMessage();
		}
		else if (iResult == 0) {
			write("[INFO] Connection closed");
		}
		else {
			write("[ERROR] recv failed: " + WSAGetLastError());
		}
	} while (iResult > 0);

	// Cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	sendThread.join();
	return EXIT_SUCCESS;
}

std::string ChessClient::getPieceString(piece pce) {
	std::string colourType;

	int color = pce.colour;
	int type = pce.type;

	switch (color) {
	case WHITE:
		colourType = "White ";
		break;
	case BLACK:
		colourType = "Black ";
		break;
	}

	switch (type) {
	case KING:
		colourType += "King";
		break;
	case QUEEN:
		colourType += "Queen";
		break;
	case ROOK:
		colourType += "Rook";
		break;
	case BISHOP:
		colourType += "Bishop";
		break;
	case KNIGHT:
		colourType += "Knight";
		break;
	case PAWN:
		colourType += "Pawn";
		break;
	}

	return colourType;
}

std::string ChessClient::getSquareString(int row, int col) {
	std::string square = "";
	square += char(row + 97);
	switch (col) {
	case 0:
		square += '8';
		break;
	case 1:
		square += '7';
		break;
	case 2:
		square += '6';
		break;
	case 3:
		square += '5';
		break;
	case 4:
		square += '4';
		break;
	case 5:
		square += '3';
		break;
	case 6:
		square += '2';
		break;
	case 7:
		square += '1';
		break;
	}
	return square;
}

void ChessClient::boardClick(ConData::short_t row, ConData::short_t col) {
	if (row >= 8 || col >= 8 || chessBoard[row][col].empty() && !selectMove) {
		this->write("No Piece Selected!");
	}
	else if (!selectMove) {
		if (chessBoard[row][col].colour == playerColour) {
			pieceLocation = { row,col };
			selectMove = true;
			message = getPieceString(chessBoard[row][col]);
			this->write(message);
		}
		else {
			this->write(std::string("Pick A ") + ((playerColour == WHITE) ? "White" : "Black") + " Piece!");
		}
	}
	else {
		moveLocation = { row,col };
		selectMove = false;

		message += " to " + getSquareString(row, col);
		sendMessage();
	}
}

void ChessClient::connectServer() {
	// Connect to server.
	if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		write("Unable to connect to server!");
		//write("Press any key to continue . . . ");
		_getch();
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	isConnected = true;
}

void ChessClient::createSocket() {
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	if (getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result) != 0) {
		write("getaddrinfo failed: ");
		//write("Press any key to continue . . . ");
		_getch();
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		write("Error at socket(): " + WSAGetLastError());
		//write("Press any key to continue . . . ");
		_getch();
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}

void ChessClient::drawBoard() {
	for (int iRow = 0, iRowOffset = 0; iRow < 8; ++iRow, iRowOffset += 2) {
		for (int iCol = 0, iColOffset = 0; iCol < 8; ++iCol, iColOffset += 2) {
			setCellCharacter(ConData::uchar_t(chessBoard[iRow][iCol].unicode), { ConData::short_t(iRow + iRowOffset + 1),ConData::short_t(iCol + iColOffset + 1) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 0),ConData::short_t(iCol + iColOffset + 0) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 1),ConData::short_t(iCol + iColOffset + 0) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 2),ConData::short_t(iCol + iColOffset + 0) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 0),ConData::short_t(iCol + iColOffset + 1) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 1),ConData::short_t(iCol + iColOffset + 1) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 2),ConData::short_t(iCol + iColOffset + 1) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 0),ConData::short_t(iCol + iColOffset + 2) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 1),ConData::short_t(iCol + iColOffset + 2) });
			setCellAttribute(ConData::short_t(chessColours[iRow][iCol]), { ConData::short_t(iRow + iRowOffset + 2),ConData::short_t(iCol + iColOffset + 2) });
		}
	}
}

void ChessClient::initializeWinsock() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		write("WSAStartup failed");
		//write("Press any key to continue . . . ");
		_getch();
		exit(EXIT_FAILURE);
	}
}

void ChessClient::sendingThread() {
	ConData::inputbuffer_t inBuffer(128);
	KEY_EVENT_RECORD ker;
	MOUSE_EVENT_RECORD mer;
	ConData::achar_t shift = '~';
	ConData::Coord cursorMove;

	while (isConnected) {
		// Get user test from standard input
		ConData::ulong_t numEvent = readConsoleInput(inBuffer);

		for (size_t iEvent = 0; iEvent < numEvent; ++iEvent) {
			switch (inBuffer[iEvent].EventType) {
			case ConFlags::MOUSE:
				mer = inBuffer[iEvent].Event.MouseEvent;
				switch (mer.dwEventFlags) {
				case 0:
					if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						int cRow = mer.dwMousePosition.X / 3;
						int cCol = mer.dwMousePosition.Y / 3;

						boardClick(cRow, cCol);
					}
				}
			}
		}
	}
}

void ChessClient::sendMessage() {
	// Clear the buffer and calculate the buffer length
	buffer.clear();
	short bufferLength = (sizeof(short) * 2) + (short)message.length() + (sizeof(short) * 4) * 2;

	// Write all data to the buffer
	buffer.writeShort16BE(bufferLength);
	buffer.writeStringBE(message);
	buffer.writeShort16BE(pieceLocation.X);
	buffer.writeShort16BE(pieceLocation.Y);
	buffer.writeShort16BE(moveLocation.X);
	buffer.writeShort16BE(moveLocation.Y);

	// Send the buffer
	int iResult = send(ConnectSocket, buffer.toCharArray(), bufferLength, 0);
	if (iResult == SOCKET_ERROR) {
		write("send failed: " + WSAGetLastError());
		//write("Press any key to continue . . . ");
		_getch();
		closesocket(ConnectSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}

void ChessClient::write(std::string text) {
	setCellCharacter(' ', { 0,25 }, width);

	for (size_t i = 0; i < text.size(); ++i) {
		setCellContents(ConFlags::BLACK, text[i], { ConData::short_t(i),25 });
		setCellAttribute(ConFlags::BACK_WHITE, { ConData::short_t(i),25 });
	}
}

void ChessClient::writeMessage() {
	// Clear and copy the buffer
	buffer.clear();
	buffer.loadBuffer(recvbuf, 512);

	// Buffer size, name of client, and message
	buffer.readShort16BE();
	std::string messageString = buffer.readStringBE(buffer.readShort16BE());

	if (messageString == "colour") {
		playerColour = buffer.readShort16BE();
		write((playerColour == BLACK) ? "Black" : "White");
	}
	else {
		for (int iRow = 0, iRowOffset = 0; iRow < 8; ++iRow, iRowOffset += 2) {
			for (int iCol = 0, iColOffset = 0; iCol < 8; ++iCol, iColOffset += 2) {
				int unicode = buffer.readShort16BE();
				chessBoard[iRow][iCol] = piece(unicode);
				setCellCharacter(ConData::uchar_t(unicode), { ConData::short_t(iRow + iRowOffset + 1),ConData::short_t(iCol + iColOffset + 1) });
			}
		}
		write(messageString);
	}
}