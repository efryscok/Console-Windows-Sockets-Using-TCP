/** @file: ConError.cpp
	@author Erik Fryscok
	@date 2016-03-01
	@brief Source code for ConError class
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#include <ConError.hpp>

/**	@fn ConError::string_type ConError::msg() const
	@brief Creates an error message containing the code, message, file, and line where error occurred
	@return Formatted error message
	*/
ConError::string_type ConError::msg() const {
	char* msg;
	auto c = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK, NULL, code(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
	string_type strMsg = (c == 0) ? "unknown" : msg;
	LocalFree(msg);

	std::ostringstream oss;
	oss << "Error: " << code() << "\n";
	oss << strMsg << "\n";
	oss << "In: " << file() << "\n";
	oss << "Line: " << line() << "\n";
	return oss.str();
}