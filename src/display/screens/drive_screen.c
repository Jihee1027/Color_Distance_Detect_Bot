#include "display/screens/drive_screen.h"
#include "display/screens/loading_screen.h"
#include "display/widgets/car_widget.h"
#include "display/display_theme.h"

static bool drawn = false;

void drive_screen_render(display_data_t *data) {
    if (data->screen != DISPLAY_SCREEN_DRIVE) {
        drawn = false;
        return;
    }
    if (drawn) {
        return;
    }
    drawn = true;
    car_widget_draw(120, 135, 3, COLOR_RED);
}