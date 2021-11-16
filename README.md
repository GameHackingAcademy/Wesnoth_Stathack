# Wesnoth Stathack
Referenced in https://gamehacking.academy/lesson/4/1.

A stathack for Wesnoth 1.14.9 that displays the second player's gold whenever the Terrain Description box is shown.

This is done through the use of a code cave. When injected, the DLL modifies the function that displays the terrain description and changes the code to jump to the code cave function defined in the DLL. The code cave function then saves the registers, gets the second player's gold, and writes the value into the buffer used by the game to display the Terrain Description text. It then jumps back to the Terrain Description method and displays the original description with the gold prepended to it.

This must be injected into the Wesnoth process to work. One way to do this is to use a DLL injector. Another way is to enable AppInit_DLLs in the registry.
