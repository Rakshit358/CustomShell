#include "MyWindow.h"
#include <iostream>

FileManagementWindow::FileManagementWindow()
{
    set_title("File Management");
    set_default_size(800, 800);
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>("Here are file management commands!");
    set_child(*label);
}
