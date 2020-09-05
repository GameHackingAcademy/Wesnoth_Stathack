#include <Windows.h>
#include <stdio.h>

DWORD* player_base;
DWORD* game_base;
DWORD* gold;
DWORD ori_call_address = 0x5E9630;

DWORD ret_address = 0x5ED12E;
char gold_byte_array[4] = { 0 };

__declspec(naked) void codecave() {
	__asm {
		pushad
	}

	player_base = (DWORD*)0x017EED18;
	game_base = (DWORD*)(*player_base + 0xA90);
	gold = (DWORD*)(*game_base + 0x274);

	sprintf_s(gold_byte_array, 4, "%d", *gold);

	__asm {
		popad
		pushad
		mov eax, dword ptr ds:[edx]
		mov bl, gold_byte_array[0]
		mov byte ptr ds:[eax], bl
		mov bl, gold_byte_array[1]
		mov byte ptr ds:[eax + 1], bl
		mov bl, gold_byte_array[2]
		mov byte ptr ds:[eax + 2], bl
	}

	_asm {
		popad
		call ori_call_address
		jmp ret_address
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	DWORD old_protect;
	unsigned char* hook_location = (unsigned char*)0x5ED129;

	if (fdwReason == DLL_PROCESS_ATTACH) {
		VirtualProtect((void*)hook_location, 5, PAGE_EXECUTE_READWRITE, &old_protect);
		*hook_location = 0xE9;
		*(DWORD*)(hook_location + 1) = (DWORD)&codecave - ((DWORD)hook_location + 5);
	}

	return true;
}
