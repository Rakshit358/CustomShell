#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>
#include <map>
#include <string>

// NetworkingWindow: This window displays networking commands and their output.
class NetworkingWindow : public Gtk::Window
{
public:
    NetworkingWindow();

private:
    // Member variables:
    Gtk::Label output_label;             // Widget for displaying command output.
    Gtk::ScrolledWindow scrolled_window; // Makes the output scrollable.

    // Signal handlers:
    void on_ipconfig_clicked();
    void on_help_clicked();
    void on_exit_clicked();
};

// Other window declarations:
class FileManagementWindow : public Gtk::Window
{
public:
    FileManagementWindow();
};

class SystemInformationWindow : public Gtk::Window
{
public:
    SystemInformationWindow();
};

class MyWindow : public Gtk::Window
{
public:
    MyWindow();

private:
    Gtk::Button btn1{"Networking Commands"};
    Gtk::Button btn2{"File Management"};
    Gtk::Button btn3{"System Information"};
    Gtk::Button exit_button{"Exit"};
    Gtk::Box box{Gtk::Orientation::VERTICAL};

    void on_networking_clicked();
    void on_file_management_clicked();
    void on_system_information_clicked();
    void on_exit_clicked();
};

#endif // MYWINDOW_H