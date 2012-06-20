Mupen64Plus-PB by CatalystG
---------------------------

Based on the HP Touchpad port by Metricity.

Usage
------
You will need to modify the Makefiles to point to the correct compiler and libraries. SDL is currently only used for audio, though references are throughout other parts of the code. They can mostly be replaced. PlayBook SDL does not support Open GLES 2.0 which the emulator uses.

Loading Games
--------------
-To create the necessary folder structure, first run Mupen64Plus-PB before trying to add roms.
-You should be shown an empty dialog box.
-Press "Cancel" and the application will exit.
-Next, connect your PlayBook to your computer or enable wireless sharing.
-Open "My Computer" and select the BlackBerry network drive.
-Navigate to misc/n64/roms/ and copy your legally obtained games here.
-The dialog for picking games will also allow you to change the video plugin or disable the sound. At a minimum, a rom and at most one video plugin need to be used.

Hi-Res Textures:
---------------- 
-To load Hi-Res textures, run the target game first to create the needed folders.
-Find the texture packs online and copy into /misc/n64/hires_texture
-Start up the game and wait for it to load as it will takes longer.

Save/Load States:
-----------------
-Save/load state buttons are located in the top left/right corners. To hide these, tap the top middle of the screen. Each game has its own save state.