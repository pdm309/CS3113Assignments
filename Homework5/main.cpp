
/*
Template
*/


#include "GameClass.h"
//
int main(int argc, char *argv[])
{
	GameClass app;
	while (!app.processEvents()) {}
	return 0;
}