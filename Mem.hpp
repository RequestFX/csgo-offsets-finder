// Made by RequestFX#1541

#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

struct module {
	uintptr_t dwBase, dwSize;
};

class Mem {
private:
	inline static HANDLE handle;
public:
	inline static uintptr_t procID;

	~Mem() {
		CloseHandle(handle);
	}

	uintptr_t getProcessID(const char* proc) {
		HANDLE hProcessId = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // current process
		uintptr_t procID;
		PROCESSENTRY32 pEntry; // process list
		pEntry.dwSize = sizeof(pEntry);

		do {
			if (!strcmp(pEntry.szExeFile, proc)) { // process from list != current process
				procID = pEntry.th32ProcessID;
				CloseHandle(hProcessId);
				handle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, false, procID);
				return this->procID = procID;
			}

		} while (Process32Next(hProcessId, &pEntry)); // Retrieves information about the next process

		return NULL;
	}

	module getModule(uintptr_t procID, const char* modName) {
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(mEntry);

		module mod = { NULL,NULL }; // set to NULL

		do {
			if (!strcmp(mEntry.szModule, modName)) {
				CloseHandle(hModule);

				mod = { (uintptr_t)mEntry.hModule, mEntry.modBaseSize };
				return mod;
			}
		} while (Module32Next(hModule, &mEntry));

		return mod;
	}

	uintptr_t getAddress(uintptr_t addr, std::vector<uintptr_t> vect) {
		for (unsigned int i = 0; i < vect.size(); i++) {
			Toolhelp32ReadProcessMemory(procID, (BYTE*)addr, &addr, sizeof(addr), 0);
			addr += vect[i];
		}
		return addr;
	}

	template <class val>
	void writeMem(uintptr_t addr, val value) {
		WriteProcessMemory(handle, (LPBYTE*)addr, &value, sizeof(value), NULL);
	}

	template <class val>
	val readMem(uintptr_t addr) {
		val x;
		Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}

	void readString(uintptr_t addr, char* buffer, SIZE_T size) {
		Toolhelp32ReadProcessMemory(procID, (LPBYTE*)addr, buffer, size, NULL);
	}
};