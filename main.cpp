/*
	A stathack for Wesnoth 1.14.9 that displays the second player's gold whenever the "Terrain Description" box is shown.
	
	This is done through the use of a codecave. When injected, the DLL modifies the function that displays the
	terrain description and changes the code to jump to the codecave function defined in the DLL. The codecave 
	function then saves the registers, gets the second player's gold, and then writes the value into the buffer used
	by the game to display the "Terrain Description" text. It then jumps back to the "Terrain Description" method and displays 
	the original description with the gold prepended to it.
	
	This must be injected into the Wesnoth process to work. One way to do this is to use a DLL injector. 
   	Another way is to enable AppInit_DLLs in the registry.
	
	The offsets and explanation of the codecave are covered in https://gamehacking.academy/lesson/19
*/
#include <Windows.h>
#include <stdio.h>

DWORD* player_base;
DWORD* game_base;
DWORD* gold;

// Original address called by the game
DWORD ori_call_address = 0x5E9630;

DWORD ret_address = 0x5ED12E;

// Buffer to hold the second player's gold value
char gold_byte_array[4] = { 0 };

// Our codecave that program execution will jump to. The declspec naked attribute tells the compiler to not add any function
// headers around the assembled code
__declspec(naked) void codecave() {
	// Asm blocks allow you to write pure assembly
	// In this case, we use it to save all the registers
	__asm {
		pushad
	}

	// Get the second player's gold value based off the base pointer
	player_base = (DWORD*)0x017EED18;
	game_base = (DWORD*)(*player_base + 0xA90);
	gold = (DWORD*)(*game_base + 0x274);

	// Convert the gold value to its ASCII representation
	sprintf_s(gold_byte_array, 4, "%d", *gold);

	// Restore the registers corrupted by sprintf and save them again
	// Then, load the buffer from edx, and place each byte of the second player's gold 
	// value into the buffer
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

	// Restore the registers and then recreate the original instructions that we overwrote
	// After those, jump back to the instruction after the one we overwrote
	_asm {
		popad
		call ori_call_address
		jmp ret_address
	}
}

// When our DLL is attached, unprotect the memory at the code we wish to write at
// Then set the first opcode to E9, or jump
// Calculate the location using the formula: new_location - original_location + 5
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
