#include <gtk/gtk.h>

#include "menu_info.h"
#include "systray.h"
#include "pulseaudio.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    menu_infos_t* mi = menu_infos_create();
    systray_create(mi);

    pulseaudio_init();
    pulseaudio_connect(mi);
    pulseaudio_start();

    gtk_main();

    menu_infos_destroy(mi);
    return 0;
}
