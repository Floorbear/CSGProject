#include <iostream>
//built in
#include "EngineCore/EngineCore.h"

#pragma comment(lib,"EngineCore.lib")


int main()
{
	EngineCore* NewCore = new EngineCore();
	NewCore->Start();
	delete NewCore;
}
