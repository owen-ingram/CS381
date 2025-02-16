#define RAYGUI_IMPLEMENTATION
#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "raygui.h" 
#include "VolumeControl.h"  
#include <raylib.h>         
#include "raylib-cpp.hpp"

extern Sound pingSound;  
Sound pingSound;

void PingButton() {
    PlaySound(pingSound);
}
