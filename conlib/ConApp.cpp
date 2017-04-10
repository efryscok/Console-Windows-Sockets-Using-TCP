/** @file: ConApp.cpp
	@author Erik Fryscok
	@date 2016-03-01
	@brief Source code for ConApp class
	@note Project 2 - Singleton (INFO-5100 - Patterns in Software 2)
	*/

#include <ConApp.hpp>

// Singleton pointer
ConApp* ConApp::thisApp = nullptr;

// Entry point - main function
int main(int argc, char* argv[]) try {
	return ConApp::_main(argc, argv);
}
catch (const ConError &e) {
	MessageBoxA(NULL, e.msg().c_str(), "Runtime Error", MB_OK);
}
catch (const std::exception &e) {
	MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK);
}

// Entry point - app function
int ConApp::_main(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i)
		thisApp->_args.push_back(argv[i]);

	return thisApp->execute();
}