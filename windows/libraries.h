#pragma once

/* Ensure SDL libraries get linked in the wacky way I learned how to in VS. */

/* NOTE: Ensure you change the directories in the project.
   Or emulate my DevLibs dir structure, your choice really. */

/* You will also need to manually copy the DLLs over, I'm lazy. */

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_image.lib")
