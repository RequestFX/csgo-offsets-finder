// Made by RequestFX#1541

#pragma once

#include <iostream>

#include <Windows.h>
#include <TlHelp32.h>

namespace SigScanner {
	bool MemoryCompare(const BYTE* bData, const BYTE* bSig, const char* szMask) {
		for (; *szMask; szMask++, bData++, bSig++)
			if (*szMask == 'x' && *bData != *bSig)
				return false;
		return true;
	}

	uintptr_t FindSignature(int procID, module mod, const char* sig, const char* mask) {
		BYTE* data = new BYTE[mod.dwSize];
		SIZE_T bytesRead;

		Toolhelp32ReadProcessMemory(procID, (LPVOID)(mod.dwBase), data, mod.dwSize, &bytesRead);

		for (uintptr_t i = 0; i < mod.dwSize; i++) {
			if (MemoryCompare((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
				delete[] data;
				return i; // returns offset only
			}
		}
		delete[] data;
#ifndef NDEBUG
		std::cout << "\nFailed to find Signature: " << sig << " Mask: " << mask << "\n";
#endif
		return NULL;
	}

	uintptr_t FindSignature(int procID, module mod, const char* sig, const char* mask, uintptr_t rpmVal) {

		if (uintptr_t addr = FindSignature(procID, mod, sig, mask)) {
			addr += mod.dwBase + rpmVal; // add base
			uintptr_t buffer;
			Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, &buffer, sizeof(buffer), NULL);
			return buffer - mod.dwBase; // returns offset only
		}
		return NULL;
	}
};