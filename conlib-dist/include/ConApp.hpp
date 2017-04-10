/** @file: ConApp.hpp
	@author Erik Fryscok
	@date 2016-03-01
	@brief Implements the singleton application pattern
	@details Used to create a console application
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#pragma once

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "conlib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "conlib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "conlib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "conlib-mt-s.lib")
#endif

#include <Console.hpp>

class ConApp : public Console {
private:
	// Type definitions
	using Args = std::vector<std::string>;

private:
	// Member variables
	Args _args;
	static ConApp* thisApp;

protected:
	// Constructor
	ConApp() { thisApp = this; }

	// Inline functions
	inline virtual int execute() { return EXIT_SUCCESS; }
	inline Args const& getArgs() const { return _args; }

private:
	// Main functions
	static int _main(int argc, char* argv[]);
	friend int main(int argc, char* argv[]);
};