#include "Buffer.h"
#include <stdio.h>

//Constructor
Buffer::Buffer(std::size_t size) {
	_buffer.resize(size);
	readIndex = 0;
	writeIndex = 0;
}

//Desctructor
Buffer::~Buffer() {
}

//Write a short 16 to the buffer
void Buffer::writeShort16BE(std::size_t index, short int value) {
	//Grow buffer if new value will pass current size
	if (index + 1 > _buffer.size())
		Buffer::growBuffer(2);

	_buffer[index + 1] = (char)value;
	_buffer[index] = value >> 8;
}

//Set up to write a short 16 to the buffer
void Buffer::writeShort16BE(short int value) {
	writeShort16BE(writeIndex, value);
	writeIndex += 2;
}

//Write a string to the buffer
void Buffer::writeStringBE(std::size_t index, std::string value) {
	//Grow buffer if new value will pass current size
	if (index + value.length() + 2 > _buffer.size())
		Buffer::growBuffer(value.length() + 2);

	//Prefix the length of the string
	writeShort16BE(index, (short)value.size());
	index += 2;

	//Add each character to the buffer
	for (size_t i = 0; i < value.size(); i++) {
		_buffer[index + i] = value.at(i);
	}
}



//Set up to retup to write a string to the buffer
void Buffer::writeStringBE(std::string value) {
	writeStringBE(writeIndex, value);
	writeIndex += value.size() + 2;
}

//Read unsigned short 16 from the buffer
short int Buffer::readShort16BE(std::size_t index) {
	short int value = _buffer[index + 1];
	value |= _buffer[index] << 8;
	return value;
}

//Set up to read unsigned short 16 from the buffer
short int Buffer::readShort16BE() {
	short int value = readShort16BE(readIndex);
	readIndex += 2;
	return value;
}

//Read string from the buffer
std::string Buffer::readStringBE(std::size_t stringLength) {
	std::string  value = readStringBE(readIndex, stringLength);
	readIndex += stringLength;
	return value;
}

//Read string from the buffer
std::string Buffer::readStringBE(std::size_t index, std::size_t stringLength) {
	std::string value;
	for (size_t i = 0; i < stringLength; i++) {
		char temp = _buffer[index + i];
		value.append(1, temp);
	}

	return value;
}



//Get the read index
std::size_t Buffer::getReadIndex() {
	return readIndex;
}

//Get the write index
std::size_t Buffer::getWriteIndex() {
	return writeIndex;
}


//Set the read index
void Buffer::setReadIndex(std::size_t index) {
	readIndex = index;
}

//Set the write index
void Buffer::setWriteIndex(std::size_t index) {
	writeIndex = index;
}



//Reset read and write index and clear buffer
void Buffer::clear() {
	writeIndex = 0;
	readIndex = 0;
	_buffer.clear();
}

//Append to the vectors buffer
void Buffer::growBuffer(size_t growAmount) {
	_buffer.resize(_buffer.size() + growAmount);
}

//Load this buffer from the static buffer used in recieve function
void Buffer::loadBuffer(char recvbuf[512], size_t size) {
	//Grow buffer if new value will pass current size
	if (size + writeIndex > _buffer.size())
		Buffer::growBuffer(size);

	//Add each byte from the static buffer to class memeber buffer
	for (size_t i = 0; i < size; ++i) {
		_buffer.at(writeIndex) = recvbuf[i];
		writeIndex++;
	}
}

//Translate buffer to const char* for send function
const char* Buffer::toCharArray() {
	//Reset the place holder
	sendingString = "";

	int currentBufferLength = writeIndex - readIndex;

	//Add each byte in the buffer to a string for easy conversion to char*
	for (int i = 0; i < currentBufferLength; i++) {
		char temp = _buffer[readIndex + i];
		sendingString.append(1, temp);
	}

	//return a char* for the string
	return sendingString.c_str();
}

//Print the content of the buffer in hex. (Debug tool)
void Buffer::printInHex() {
	//loop over each byte and print it's hex value
	for (std::vector<uint8_t>::iterator it = _buffer.begin(); it != _buffer.end(); ++it) {
		printf("%02x ", *it);
	}
	printf("\n");
}