// Made by RequestFX#1541

#pragma once

#include "SigScanner.hpp"
#include "Game.hpp"
#include "Mem.hpp"

class NetvarsClass {
private:
	uintptr_t dwGetAllClasses;

	class RecvTable {
	public:
		std::string getTableName() {
			char tableName[128];
			mem->readString(mem->readMem<uintptr_t>((uintptr_t)this + 0xC), tableName, sizeof(tableName));
			return std::string(tableName);
		}
		void* getProperty(int i) { // RecvProp*
			return (void*)(mem->readMem<uintptr_t>((uintptr_t)this) + 0x3C * i);
		}
		int getMaxProp() {
			return mem->readMem<int>((uintptr_t)this + 0x4);
		}
	};
	class ClientClass {
	public:
		void* getTable() { // RecvTable*
			return mem->readMem<void*>((uintptr_t)this + 0xC);
		}
		void* getNextClass() { // ClientClass*
			return mem->readMem<void*>((uintptr_t)this + 0x10);
		}
	};
	class RecvProp {
	public:

		std::string getVarName() {
			char vName[128];
			mem->readString(mem->readMem<uintptr_t>((uintptr_t)this), vName, sizeof(vName));
			return std::string(vName);
		}
		int getOffset() {
			return mem->readMem<int>((uintptr_t)this + 0x2C);
		}
		void* getDataTable() { // RecvTable*
			return mem->readMem<void*>((uintptr_t)this + 0x28);
		}
	};

	uintptr_t checkProps(RecvTable* dataTable, std::string netVarName) {
		for (int i = 0; i < dataTable->getMaxProp(); i++) {
			auto pRecvProp = reinterpret_cast<RecvProp*>(dataTable->getProperty(i));
			auto varName = pRecvProp->getVarName();

			if (isdigit(varName[0])) continue;

			if (netVarName.compare(varName) == 0)
				return pRecvProp->getOffset();

			if (auto dataTable2 = reinterpret_cast<RecvTable*>(pRecvProp->getDataTable())) // if isn't null
				if (auto offset = checkProps(dataTable2, netVarName)) // if isn't null
					return offset;
		}
		return NULL;
	}

public:
	inline static Mem* mem;

	NetvarsClass(uintptr_t base, Mem* mem) {
		dwGetAllClasses = base;
		this->mem = mem;
	}

	uintptr_t NETVAR(std::string netVarName, std::string className) {
		if (auto pClass = reinterpret_cast<ClientClass*>(dwGetAllClasses)) // if isn't null
			for (; pClass != NULL; pClass = reinterpret_cast<ClientClass*>(pClass->getNextClass())) // loop through classes until null
				if (auto table = reinterpret_cast<RecvTable*>(pClass->getTable())) // if isn't null
					if (table->getTableName().compare(className) == 0)
						if (auto offset = checkProps(table, netVarName)) // if isn't null
							return offset;
#ifndef NDEBUG
		std::cout << "\nFailed to init prop: " << netVarName << " table: " << className << "\n";
#endif
		return NULL;
	}
};

class Vals {
private:
	Game* game;
	Mem* mem;
public:

	void initSignatures() {
		vals::signatures::dwClientState = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xA1\x00\x00\x00\x00\x33\xD2\x6A\x00\x6A\x00\x33\xC9\x89\xB0", "x????xxxxxxxxxx", 0x1);
		vals::signatures::dwClientState_GetLocalPlayer = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x8B\x80\x00\x00\x00\x00\x40\xC3", "xx????xx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_IsHLTV = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x80\xBF\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x32\xDB", "xx?????xx????xx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_Map = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x05\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xA1", "x????xxxxxxxxx", 0x1) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_MapDirectory = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xB8\x00\x00\x00\x00\xC3\x05\x00\x00\x00\x00\xC3", "x????xx????x", 0x7) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_MaxPlayer = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xA1\x00\x00\x00\x00\x8B\x80\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\x55\x8B\xEC\x8A\x45\x08", "x????xx????xxxxxxxxxxx", 0x7) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_PlayerInfo = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x8B\x89\x00\x00\x00\x00\x85\xC9\x0F\x84\x00\x00\x00\x00\x8B\x01", "xx????xxxx????xx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_State = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x83\xB8\x00\x00\x00\x00\x00\x0F\x94\xC0\xC3", "xx?????xxxx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::dwClientState_ViewAngles = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xF3\x0F\x11\x80\x00\x00\x00\x00\xD9\x46\x04\xD9\x05", "xxxx????xxxxx", 0x4) + game->engineaddr.dwBase;
		vals::signatures::clientstate_delta_ticks = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xC7\x87\x00\x00\x00\x00\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x83\xC4\x08", "xx????????xx????xxx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::clientstate_last_outgoing_command = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x8B\x8F\x00\x00\x00\x00\x8B\x87\x00\x00\x00\x00\x41", "xx????xx????x", 0x2) + game->engineaddr.dwBase;
		vals::signatures::clientstate_choked_commands = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x8B\x87\x00\x00\x00\x00\x41", "xx????x", 0x2) + game->engineaddr.dwBase;
		vals::signatures::clientstate_net_channel = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x8B\x8F\x00\x00\x00\x00\x8B\x01\x8B\x40\x18", "xx????xxxxx", 0x2) + game->engineaddr.dwBase;
		vals::signatures::dwEntityList = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8", "x????xxxxx????xx", 0x1);
		vals::signatures::dwForceAttack = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x89\x0D\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x8B\xF2\x8B\xC1\x83\xCE\x04", "xx????xx????xxxxxxx", 0x2);
		vals::signatures::dwForceAttack2 = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x89\x0D\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x8B\xF2\x8B\xC1\x83\xCE\x04", "xx????xx????xxxxxxx", 0x2) + 0xC;
		vals::signatures::dwForceBackward = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x51\x53\x8A\x5D\x08", "xxxxxxxx", 0x11F);
		vals::signatures::dwForceForward = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x51\x53\x8A\x5D\x08", "xxxxxxxx", 0xF5);
		vals::signatures::dwForceJump = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8B\x0D\x00\x00\x00\x00\x8B\xD6\x8B\xC1\x83\xCA\x02", "xx????xxxxxxx", 0x2);
		vals::signatures::dwForceLeft = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x51\x53\x8A\x5D\x08", "xxxxxxxx", 0x1D1);
		vals::signatures::dwForceRight = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x51\x53\x8A\x5D\x08", "xxxxxxxx", 0x200);
		vals::signatures::dwGameDir = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x68\x00\x00\x00\x00\x8D\x85\x00\x00\x00\x00\x50\x68\x00\x00\x00\x00\x68", "x????xx????xx????x", 0x1);
		vals::signatures::dwGameRulesProxy = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xA1\x00\x00\x00\x00\x85\xC0\x0F\x84\x00\x00\x00\x00\x80\xB8\x00\x00\x00\x00\x00\x74\x7A", "x????xxxx????xx?????xx", 0x1);
		vals::signatures::dwGetAllClasses = mem->readMem<uintptr_t>(game->gameaddr.dwBase + SigScanner::FindSignature(mem->procID, game->gameaddr, "\xA1\x00\x00\x00\x00\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xA1\x00\x00\x00\x00\xB9", "x????xxxxxxxxxxxx????x", 0x1)) - game->gameaddr.dwBase;
		vals::signatures::dwGlobalVars = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xFF\x50\x08\x85\xC0", "x????x????xxxxx", 0x1);
		vals::signatures::dwGlowObjectManager = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xA1\x00\x00\x00\x00\xA8\x01\x75\x4B", "x????xxxx", 0x1) + 0x4;
		vals::signatures::dwInput = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xB9\x00\x00\x00\x00\xF3\x0F\x11\x04\x24\xFF\x50\x10", "x????xxxxxxxx", 0x1);
		vals::signatures::dwInterfaceLinkList = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8B\x35\x00\x00\x00\x00\x57\x85\xF6\x74\x00\x8B\x7D\x08\x8B\x4E\x04\x8B\xC7\x8A\x11\x3A\x10", "xx????xxxx?xxxxxxxxxxxx");
		vals::signatures::dwLocalPlayer = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "xxx????xx????xxxxxxxxx", 0x3) + 0x4;
		vals::signatures::dwMouseEnable = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xB9\x00\x00\x00\x00\xFF\x50\x34\x85\xC0\x75\x10", "x????xxxxxxx", 0x1) + 0x30;
		vals::signatures::dwMouseEnablePtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xB9\x00\x00\x00\x00\xFF\x50\x34\x85\xC0\x75\x10", "x????xxxxxxx", 0x1);
		vals::signatures::dwPlayerResource = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8B\x3D\x00\x00\x00\x00\x85\xFF\x0F\x84\x00\x00\x00\x00\x81\xC7", "xx????xxxx????xx", 0x2);
		vals::signatures::dwRadarBase = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xA1\x00\x00\x00\x00\x8B\x0C\xB0\x8B\x01\xFF\x50\x00\x46\x3B\x35\x00\x00\x00\x00\x7C\xEA\x8B\x0D", "x????xxxxxxx?xxx????xxxx", 0x1);
		vals::signatures::dwSensitivity = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x81\xF9\x00\x00\x00\x00\x75\x1D\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x44\x24\x00\x8B\x44\x24\x0C\x35\x00\x00\x00\x00\x89\x44\x24\x0C", "xx????xxxxxx????xxxxx?xxxxx????xxxx", 0x2) + 0x2C;
		vals::signatures::dwSensitivityPtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x81\xF9\x00\x00\x00\x00\x75\x1D\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x44\x24\x00\x8B\x44\x24\x0C\x35\x00\x00\x00\x00\x89\x44\x24\x0C", "xx????xxxxxx????xxxxx?xxxxx????xxxx", 0x2);
		vals::signatures::dwSetClanTag = SigScanner::FindSignature(mem->procID, game->engineaddr, "\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
		vals::signatures::dwViewMatrix = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x0F\x10\x05\x00\x00\x00\x00\x8D\x85\x00\x00\x00\x00\xB9", "xxx????xx????x", 0x3) + 0xB0;
		vals::signatures::dwWeaponTable = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xB9\x00\x00\x00\x00\x6A\x00\xFF\x50\x08\xC3", "x????xxxxxx", 0x1);
		vals::signatures::dwWeaponTableIndex = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x39\x86\x00\x00\x00\x00\x74\x06\x89\x86\x00\x00\x00\x00\x8B\x86", "xx????xxxx????xx", 0x2) + game->gameaddr.dwBase;
		vals::signatures::dwYawPtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x81\xF9\x00\x00\x00\x00\x75\x16\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x45\x00\x81\x75\x00\x00\x00\x00\x00\xEB\x0A\x8B\x01\x8B\x40\x30\xFF\xD0\xD9\x5D\x0C\x8B\x55\x08", "xx????xxxxxx????xxxx?xx?????xxxxxxxxxxxxxxx", 0x2);
		vals::signatures::dwZoomSensitivityRatioPtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x81\xF9\x00\x00\x00\x00\x75\x1A\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x45\x00\x8B\x45\xF4\x35\x00\x00\x00\x00\x89\x45\xFC\xEB\x0A\x8B\x01\x8B\x40\x30\xFF\xD0\xD9\x5D\xFC\xA1", "xx????xxxxxx????xxxx?xxxx????xxxxxxxxxxxxxxxx", 0x2);
		vals::signatures::dwbSendPackets = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xB3\x01\x8B\x01\x8B\x40\x10\xFF\xD0\x84\xC0\x74\x0F\x80\xBF\x00\x00\x00\x00\x00\x0F\x84", "xxxxxxxxxxxxxxx?????xx") + 0x1;
		vals::signatures::dwppDirect3DDevice9 = SigScanner::FindSignature(mem->procID, game->shaderdx9addr, "\xA1\x00\x00\x00\x00\x50\x8B\x08\xFF\x51\x0C", "x????xxxxxx", 0x1);
		vals::signatures::m_pStudioHdr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8B\xB6\x00\x00\x00\x00\x85\xF6\x74\x05\x83\x3E\x00\x75\x02\x33\xF6\xF3\x0F\x10\x44\x24", "xx????xxxxxxxxxxxxxxxx", 0x2) + game->gameaddr.dwBase;
		vals::signatures::m_yawClassPtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x81\xF9\x00\x00\x00\x00\x75\x16\xF3\x0F\x10\x05\x00\x00\x00\x00\xF3\x0F\x11\x45\x00\x81\x75\x00\x00\x00\x00\x00\xEB\x0A\x8B\x01\x8B\x40\x30\xFF\xD0\xD9\x5D\x0C\x8B\x55\x08", "xx????xxxxxx????xxxx?xx?????xxxxxxxxxxxxxxx", 0x2);
		vals::signatures::m_pitchClassPtr = SigScanner::FindSignature(mem->procID, game->gameaddr, "\xA1\x00\x00\x00\x00\x89\x74\x24\x28", "x????xxxx", 0x1);
		vals::signatures::interface_engine_cvar = SigScanner::FindSignature(mem->procID, game->vstdLibaddr, "\x8B\x0D\x00\x00\x00\x00\xC7\x05", "xx????xx", 0x2);
		vals::signatures::convar_name_hash_table = SigScanner::FindSignature(mem->procID, game->vstdLibaddr, "\x8B\x3C\x85", "xxx", 0x3);
		vals::signatures::m_bDormant = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8A\x81\x00\x00\x00\x00\xC3\x32\xC0", "xx????xxx", 0x2) + 0x8 + game->gameaddr.dwBase;
		vals::signatures::model_ambient_min = SigScanner::FindSignature(mem->procID, game->engineaddr, "\xF3\x0F\x10\x0D\x00\x00\x00\x00\xF3\x0F\x11\x4C\x24\x00\x8B\x44\x24\x20\x35\x00\x00\x00\x00\x89\x44\x24\x0C", "xxxx????xxxxx?xxxxx????xxxx", 0x4);
		vals::signatures::set_abs_angles = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx");
		vals::signatures::set_abs_origin = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxx");
		vals::signatures::is_c4_owner = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x56\x8B\xF1\x85\xF6\x74\x31", "xxxxxxx");
		vals::signatures::force_update_spectator_glow = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x74\x07\x8B\xCB\xE8\x00\x00\x00\x00\x83\xC7\x10", "xxxxx????xxx");
		vals::signatures::anim_overlays = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x8B\x89\x00\x00\x00\x00\x8D\x0C\xD1", "xx????xxx", 0x2) + game->gameaddr.dwBase;
		vals::signatures::m_flSpawnTime = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x89\x86\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x80\xBE\x00\x00\x00\x00\x00", "xx????x????xx?????", 0x2) + game->gameaddr.dwBase;
		vals::signatures::find_hud_element = SigScanner::FindSignature(mem->procID, game->gameaddr, "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x57\x8B\xF9\x33\xF6\x39\x77\x28", "xxxxxxxxxxxxxxxx") + game->gameaddr.dwBase;
	}

	void initNetvars() {
		NetvarsClass* NetvarManager = new NetvarsClass(game->gameaddr.dwBase + vals::signatures::dwGetAllClasses, mem);

		vals::netvars::m_ArmorValue = NetvarManager->NETVAR("m_ArmorValue", "DT_CSPlayer");
		vals::netvars::m_Collision = NetvarManager->NETVAR("m_Collision", "DT_BasePlayer");
		vals::netvars::m_CollisionGroup = NetvarManager->NETVAR("m_CollisionGroup", "DT_CSPlayer");
		vals::netvars::m_Local = NetvarManager->NETVAR("m_Local", "DT_BasePlayer");
		vals::netvars::m_MoveType = NetvarManager->NETVAR("m_nRenderMode", "DT_CSPlayer") + 0x1;
		vals::netvars::m_OriginalOwnerXuidHigh = NetvarManager->NETVAR("m_OriginalOwnerXuidHigh", "DT_BaseAttributableItem");
		vals::netvars::m_OriginalOwnerXuidLow = NetvarManager->NETVAR("m_OriginalOwnerXuidLow", "DT_BaseAttributableItem");
		vals::netvars::m_aimPunchAngle = NetvarManager->NETVAR("m_aimPunchAngle", "DT_BasePlayer");
		vals::netvars::m_aimPunchAngleVel = NetvarManager->NETVAR("m_aimPunchAngleVel", "DT_BasePlayer");
		vals::netvars::m_bGunGameImmunity = NetvarManager->NETVAR("m_bGunGameImmunity", "DT_CSPlayer");
		vals::netvars::m_bHasDefuser = NetvarManager->NETVAR("m_bHasDefuser", "DT_CSPlayer");
		vals::netvars::m_bHasHelmet = NetvarManager->NETVAR("m_bHasHelmet", "DT_CSPlayer");
		vals::netvars::m_bInReload = NetvarManager->NETVAR("m_flNextPrimaryAttack", "DT_BaseCombatWeapon") + 0x6D;
		vals::netvars::m_bIsDefusing = NetvarManager->NETVAR("m_bIsDefusing", "DT_CSPlayer");
		vals::netvars::m_bIsScoped = NetvarManager->NETVAR("m_bIsScoped", "DT_CSPlayer");
		vals::netvars::m_bSpotted = NetvarManager->NETVAR("m_bSpotted", "DT_BaseEntity");
		vals::netvars::m_bSpottedByMask = NetvarManager->NETVAR("m_bSpottedByMask", "DT_BaseEntity");
		vals::netvars::m_dwBoneMatrix = NetvarManager->NETVAR("m_nForceBone", "DT_BaseAnimating") + 0x1C;
		vals::netvars::m_fAccuracyPenalty = NetvarManager->NETVAR("m_fAccuracyPenalty", "DT_WeaponCSBase");
		vals::netvars::m_fFlags = NetvarManager->NETVAR("m_fFlags", "DT_CSPlayer");
		vals::netvars::m_flFallbackWear = NetvarManager->NETVAR("m_flFallbackWear", "DT_BaseAttributableItem");
		vals::netvars::m_flFlashDuration = NetvarManager->NETVAR("m_flFlashDuration", "DT_CSPlayer");
		vals::netvars::m_flFlashMaxAlpha = NetvarManager->NETVAR("m_flFlashMaxAlpha", "DT_CSPlayer");
		vals::netvars::m_flNextPrimaryAttack = NetvarManager->NETVAR("m_flNextPrimaryAttack", "DT_BaseCombatWeapon");
		vals::netvars::m_hActiveWeapon = NetvarManager->NETVAR("m_hActiveWeapon", "DT_BasePlayer");
		vals::netvars::m_hMyWeapons = NetvarManager->NETVAR("m_hActiveWeapon", "DT_BasePlayer") - 256;
		vals::netvars::m_hObserverTarget = NetvarManager->NETVAR("m_hObserverTarget", "DT_BasePlayer");
		vals::netvars::m_hOwner = NetvarManager->NETVAR("m_hOwner", "DT_PredictedViewModel");
		vals::netvars::m_hOwnerEntity = NetvarManager->NETVAR("m_hOwnerEntity", "DT_CSPlayer");
		vals::netvars::m_iAccountID = NetvarManager->NETVAR("m_iAccountID", "DT_BaseAttributableItem");
		vals::netvars::m_iClip1 = NetvarManager->NETVAR("m_iClip1", "DT_BaseCombatWeapon");
		vals::netvars::m_iCompetitiveRanking = NetvarManager->NETVAR("m_iCompetitiveRanking", "DT_CSPlayerResource");
		vals::netvars::m_iCompetitiveWins = NetvarManager->NETVAR("m_iCompetitiveWins", "DT_CSPlayerResource");
		vals::netvars::m_iCrosshairId = NetvarManager->NETVAR("m_bHasDefuser", "DT_CSPlayer") + 0x5C;
		vals::netvars::m_iEntityQuality = NetvarManager->NETVAR("m_iEntityQuality", "DT_BaseAttributableItem");
		vals::netvars::m_iFOVStart = NetvarManager->NETVAR("m_iFOVStart", "DT_CSPlayer");
		vals::netvars::m_iFOV = NetvarManager->NETVAR("m_iFOV", "DT_CSPlayer");
		vals::netvars::m_iGlowIndex = NetvarManager->NETVAR("m_flFlashDuration", "DT_CSPlayer") + 0x18;
		vals::netvars::m_iHealth = NetvarManager->NETVAR("m_iHealth", "DT_BasePlayer");
		vals::netvars::m_iItemDefinitionIndex = NetvarManager->NETVAR("m_iItemDefinitionIndex", "DT_BaseCombatWeapon");
		vals::netvars::m_iItemIDHigh = NetvarManager->NETVAR("m_iItemIDHigh", "DT_BaseAttributableItem");
		vals::netvars::m_iObserverMode = NetvarManager->NETVAR("m_iObserverMode", "DT_BasePlayer");
		vals::netvars::m_iShotsFired = NetvarManager->NETVAR("m_iShotsFired", "DT_CSPlayer");
		vals::netvars::m_iState = NetvarManager->NETVAR("m_iState", "DT_BaseCombatWeapon");
		vals::netvars::m_iTeamNum = NetvarManager->NETVAR("m_iTeamNum", "DT_BasePlayer");
		vals::netvars::m_lifeState = NetvarManager->NETVAR("m_lifeState", "DT_CSPlayer");
		vals::netvars::m_nFallbackPaintKit = NetvarManager->NETVAR("m_nFallbackPaintKit", "DT_BaseAttributableItem");
		vals::netvars::m_nFallbackSeed = NetvarManager->NETVAR("m_nFallbackSeed", "DT_BaseAttributableItem");
		vals::netvars::m_nFallbackStatTrak = NetvarManager->NETVAR("m_nFallbackStatTrak", "DT_BaseAttributableItem");
		vals::netvars::m_nForceBone = NetvarManager->NETVAR("m_nForceBone", "DT_BaseAnimating");
		vals::netvars::m_nTickBase = NetvarManager->NETVAR("m_nTickBase", "DT_BasePlayer");
		vals::netvars::m_rgflCoordinateFrame = NetvarManager->NETVAR("m_CollisionGroup", "DT_CSPlayer") - 48;
		vals::netvars::m_szCustomName = NetvarManager->NETVAR("m_szCustomName", "DT_BaseAttributableItem");
		vals::netvars::m_szLastPlaceName = NetvarManager->NETVAR("m_szLastPlaceName", "DT_CSPlayer");
		vals::netvars::m_vecOrigin = NetvarManager->NETVAR("m_vecOrigin", "DT_BasePlayer");
		vals::netvars::m_vecVelocity = NetvarManager->NETVAR("m_vecVelocity[0]", "DT_CSPlayer");
		vals::netvars::m_vecViewOffset = NetvarManager->NETVAR("m_vecViewOffset[0]", "DT_CSPlayer");
		vals::netvars::m_viewPunchAngle = NetvarManager->NETVAR("m_viewPunchAngle", "DT_BasePlayer");
		vals::netvars::m_thirdPersonViewAngles = NetvarManager->NETVAR("deadflag", "DT_CSPlayer") + 0x4;
		vals::netvars::m_clrRender = NetvarManager->NETVAR("m_clrRender", "DT_BaseEntity");
		vals::netvars::m_flC4Blow = NetvarManager->NETVAR("m_flC4Blow", "DT_PlantedC4");
		vals::netvars::m_flTimerLength = NetvarManager->NETVAR("m_flTimerLength", "DT_PlantedC4");
		vals::netvars::m_flDefuseLength = NetvarManager->NETVAR("m_flDefuseLength", "DT_PlantedC4");
		vals::netvars::m_flDefuseCountDown = NetvarManager->NETVAR("m_flDefuseCountDown", "DT_PlantedC4");
		vals::netvars::cs_gamerules_data = NetvarManager->NETVAR("cs_gamerules_data", "DT_CSGameRulesProxy"); // returns 0x0
		vals::netvars::m_SurvivalRules = NetvarManager->NETVAR("m_SurvivalRules", "DT_CSGameRulesProxy");
		vals::netvars::m_SurvivalGameRuleDecisionTypes = NetvarManager->NETVAR("m_SurvivalGameRuleDecisionTypes", "DT_CSGameRulesProxy");
		vals::netvars::m_bIsValveDS = NetvarManager->NETVAR("m_bIsValveDS", "DT_CSGameRulesProxy");
		vals::netvars::m_bFreezePeriod = NetvarManager->NETVAR("m_bFreezePeriod", "DT_CSGameRulesProxy");
		vals::netvars::m_bBombPlanted = NetvarManager->NETVAR("m_bBombPlanted", "DT_CSGameRulesProxy");
		vals::netvars::m_bIsQueuedMatchmaking = NetvarManager->NETVAR("m_bIsQueuedMatchmaking", "DT_CSGameRulesProxy");
		vals::netvars::m_flSimulationTime = NetvarManager->NETVAR("m_flSimulationTime", "DT_CSPlayer");
		vals::netvars::m_flLowerBodyYawTarget = NetvarManager->NETVAR("m_flLowerBodyYawTarget", "DT_CSPlayer");
		vals::netvars::m_angEyeAnglesX = NetvarManager->NETVAR("m_angEyeAngles[0]", "DT_CSPlayer");
		vals::netvars::m_angEyeAnglesY = NetvarManager->NETVAR("m_angEyeAngles[1]", "DT_CSPlayer");
		vals::netvars::m_flNextAttack = NetvarManager->NETVAR("m_flNextAttack", "DT_CSPlayer");
		vals::netvars::m_iMostRecentModelBoneCounter = NetvarManager->NETVAR("m_nForceBone", "DT_CSPlayer") + 0x4;
		vals::netvars::m_flLastBoneSetupTime = NetvarManager->NETVAR("m_nSequence", "DT_BaseAnimating") + 0x68;
		vals::netvars::m_bStartedArming = NetvarManager->NETVAR("m_bStartedArming", "DT_WeaponC4");
		vals::netvars::m_bUseCustomBloomScale = NetvarManager->NETVAR("m_bUseCustomBloomScale", "DT_EnvTonemapController");
		vals::netvars::m_bUseCustomAutoExposureMin = NetvarManager->NETVAR("m_bUseCustomAutoExposureMin", "DT_EnvTonemapController");
		vals::netvars::m_bUseCustomAutoExposureMax = NetvarManager->NETVAR("m_bUseCustomAutoExposureMax", "DT_EnvTonemapController");
		vals::netvars::m_flCustomBloomScale = NetvarManager->NETVAR("m_flCustomBloomScale", "DT_EnvTonemapController");
		vals::netvars::m_flCustomAutoExposureMin = NetvarManager->NETVAR("m_flCustomAutoExposureMin", "DT_EnvTonemapController");
		vals::netvars::m_flCustomAutoExposureMax = NetvarManager->NETVAR("m_flCustomAutoExposureMax", "DT_EnvTonemapController");
		vals::netvars::m_hViewModel = NetvarManager->NETVAR("m_hViewModel[0]", "DT_CSPlayer");
		vals::netvars::m_iViewModelIndex = NetvarManager->NETVAR("m_iViewModelIndex", "DT_BaseCombatWeapon");
		vals::netvars::m_nModelIndex = NetvarManager->NETVAR("m_nModelIndex", "DT_BaseCombatWeapon");

		delete NetvarManager;
	}

	void initVals(Game* game, Mem* mem) {
		this->game = game;
		this->mem = mem;
		initSignatures();
		initNetvars();
	}
};