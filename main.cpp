#include "MyWindow.h"
#include <gtkmm/cssprovider.h>
#include <gtkmm/stylecontext.h>
#include <gdkmm/display.h>
#include <iostream>

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create("org.gtkmm.example.cliapp");

    auto css_provider = Gtk::CssProvider::create();
    try {
        css_provider->load_from_path("style.css");
        Gtk::StyleContext::add_provider_for_display(
            Gdk::Display::get_default(),
            css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER
        );
    } catch (const Glib::Error& ex) {
        std::cerr << "Failed to load CSS: " << ex.what() << std::endl;
    }

    return app->make_window_and_run<MyWindow>(argc, argv);
}
