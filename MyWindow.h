#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>
#include <map>
#include <string>

class MyWindow : public Gtk::Window
{
public:
    MyWindow();

private:
    Gtk::Button btn1{"Run ipconfig"};
    Gtk::Button btn2{"Check current working directory"};
    Gtk::Button btn3{"Exit"};
    Gtk::Label output_label;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Box box{Gtk::Orientation::VERTICAL};

    std::map<std::string, std::string> command_map;

    void on_button_clicked(Gtk::Button *clicked_button);
    void on_exit_clicked();
};

#endif // MYWINDOW_H
