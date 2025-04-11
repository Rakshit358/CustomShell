#include "MyWindow.h"
#include <iostream>

SystemInformationWindow::SystemInformationWindow()
{
    set_title("System Information");
    set_default_size(400, 300);
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>("Here is the system information section!");
    set_child(*label);
}