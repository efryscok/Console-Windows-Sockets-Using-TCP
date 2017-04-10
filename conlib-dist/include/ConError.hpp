/** @file: ConError.hpp
	@author Erik Fryscok
	@date 2016-02-05
	@brief Error handling class used to throw errors with detailed messages
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#pragma once
#include <Windows.h>
#include <sstream>

class ConError {
public:
	// Type definitions
	using id_type = decltype(GetLastError());
	using file_type = const char *;
	using string_type = std::string;

private:
	// Member variables
	int line_;
	id_type code_;
	file_type file_;

public:
	// Constructor
	ConError(int line, file_type file) : code_(GetLastError()), line_(line), file_(file) {}

	// Error functions
	static inline auto throwError() { throw ConError(__LINE__, __FILE__); }
	static inline auto throwError(int res) { if (!res) throw ConError(__LINE__, __FILE__); }

	// Accessors
	inline auto line() const { return line_; }
	inline auto code() const { return code_; }
	inline auto file() const { return file_; }

	// Error message
	auto msg() const -> string_type;
};