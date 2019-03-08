#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

#include "pch.h"
#include <vector>

typedef std::vector<SignaturePart> Signature;
typedef struct {
	LPCSTR knownBytes;
	DWORD nBytes;
	BOOL unknownIsTarget;
} SignaturePart;


class Initializer
{
public:
	Initializer(DWORD csgoPid)
	{
		this->csgoHandle = findCsgoHandle(csgoPid);
		this->clientPanoHandle = findClientPanoHandle(csgoHandle);
		this->entityPtr = findEntityPtr(csgoHandle, clientPanoHandle);
	}
private:
	HANDLE csgoHandle;
	HMODULE clientPanoHandle;
	LPVOID entityPtr;

	LPBYTE findSiggedTarget(HANDLE csgoHandle, HMODULE moduleHandle, Signature sig)
	{
		// Travel the module until target is found.
		BOOL validSection = true;
		for (LPBYTE candidatePtr = (LPBYTE)moduleHandle; candidatePtr > 0; ++candidatePtr)
		{
			LPBYTE target = NULL;
			// Check each signature part, if success, then target was found
			for (SignaturePart sigPart : sig)
			{
				// Allocate memory for candidate byte array
				LPBYTE candidateCpPtr = new BYTE[sigPart.nBytes];
				// Read candidate byte array into allocated memory
				ReadProcessMemory(csgoHandle, candidatePtr, candidateCpPtr, sigPart.nBytes, NULL);
				// Compare all known bytes, if fail then set target to zero and break
				while (*sigPart.knownBytes)
				{
					if (*sigPart.knownBytes != *candidateCpPtr)
					{
						validSection = false;
						break;
					}
					++sigPart.knownBytes;
					++candidateCpPtr;
					--sigPart.nBytes;
				}
				// If this unknown is the target, record it
				if (sigPart.unknownIsTarget)
				{
					target = new BYTE[sigPart.nBytes];
					for (sigPart.nBytes; sigPart.nBytes > 0; --sigPart.nBytes)
					{
						*target = *candidateCpPtr;
						++candidateCpPtr;
						++target;
					}
				}
				delete[] candidateCpPtr;
				if (!validSection)
				{
					// Delete any previously found target
					if (target) delete[] target;
					break;
				}
			}
			if (target) return target;
		}
	}

	HANDLE findCsgoHandle(DWORD csgoPid)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, 0, csgoPid);
	}
	// I got this function from the Microsoft website and made some modifications
	// URL to original: https://docs.microsoft.com/en-us/windows/desktop/psapi/enumerating-all-modules-for-a-process
	HMODULE findClientPanoHandle(HANDLE csgoHandle)
	{
		HMODULE hMods[1024];
		DWORD cbNeeded;
		unsigned int i;
		// Get a list of all the modules in this process.
		if (EnumProcessModules(csgoHandle, hMods, sizeof(hMods), &cbNeeded))
		{
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				// Get the base name of the module.
				if (GetModuleBaseName(csgoHandle, hMods[i], szModName,
					sizeof(szModName) / sizeof(TCHAR)))
				{
					// Check whether module is client_panorama.dll.
					if (!_tcscmp(szModName, TEXT("client_panorama.dll")))
					{
						// Return the handle value.
						return hMods[i];
					}
				}
			}
		}
		return NULL;
	}
	DWORD findFromSig(HANDLE csgoHandle, HMODULE moduleHandle, Signature signature)
	{}
	LPVOID findEntityPtr(HANDLE csgoHandle, HMODULE clientPanoHandle, DWORD entityDoublePtrOffset)
	{}
};
