#include "MyWindow.h"

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create("org.gtkmm.examples.navigation");
    return app->make_window_and_run<MyWindow>(argc, argv);
}