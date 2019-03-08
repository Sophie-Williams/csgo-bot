// CsgoBot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include <windows.h>
#include "Recon.h"

int main()
{
	// Get CSGO's PID.
	DWORD csgoPid;
    std::cout << "What is CSGO's PID? \n"; 
	std::cin >> csgoPid;

	// Get a handle to CSGO.

	// Get client_panorama.dll base address.
	DWORD clientPanoBase = getClientPanoBase(csgoHandle);

	// Get entityPtrOffset.
	DWORD entityPtrOffset = getEntityPtrOffset(csgoHandle);

	// Calculate entityPtr.
	DWORD entityPtr = clientPanoBase + entityPtrOffset;

	// Dereference entityPtr to get entityBase.
	DWORD entityBase;
	ReadProcessMemory(csgoHandle, (LPCVOID)entityPtr, &entityBase, sizeof(entityBase), 0);

	return 0;
}
