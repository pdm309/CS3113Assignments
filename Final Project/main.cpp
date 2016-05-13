
/*
Template
*/


#include "GameClass.h"
//
int main(int argc, char *argv[])
{
	GameClass app = GameClass(1);
	while (!app.processEvents()) {
	}
	
	GameClass app2 = GameClass(2);
	while (!app2.processEvents()) {
	}
	
	GameClass app3 = GameClass(3);
	while (!app3.processEvents()) {
	}
		
	
	return 0;
}