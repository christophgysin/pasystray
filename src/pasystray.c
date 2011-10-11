#include <gtk/gtk.h>

#include "menu_info.h"
#include "systray.h"
#include "pulseaudio.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    menu_infos_t* mis = menu_infos_create();

    systray_create(mis);

    pulseaudio_init(mis);
    pulseaudio_connect();
    pulseaudio_start();

    gtk_main();

    menu_infos_destroy(mis);
    return 0;
}
