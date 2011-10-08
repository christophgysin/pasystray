#include <gtk/gtk.h>
#include "systray.h"
#include "pulseaudio.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    systray_create();
    pulseaudio_connect();

    gtk_main();
    return 0;
}
