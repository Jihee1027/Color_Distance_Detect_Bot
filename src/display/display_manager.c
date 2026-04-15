#include "display/display_manager.h"
#include "display/display_gfx.h"
#include "display/display_theme.h"
#include "display/screens/start_screen.h"
#include "display/screens/color_screen.h"
#include "display/screens/loading_screen.h"
#include "display/screens/drive_screen.h" 

void display_manager_render(display_data_t *data) {
    switch (data->screen) {
        case DISPLAY_SCREEN_START:
            start_screen_render(data);
            break;

        case DISPLAY_SCREEN_COLOR:
            color_screen_render(data);
            break;

        case DISPLAY_SCREEN_LOADING:
            loading_screen_render(data);
            break;

        case DISPLAY_SCREEN_DRIVE:
            drive_screen_render(data);   
            break;
        case DISPLAY_SCREEN_MAIN:
        default:
            break;
    }
}