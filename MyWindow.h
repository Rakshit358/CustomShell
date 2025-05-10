#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <gtkmm.h>
#include <map>
#include <string>

class CLIWindow : public Gtk::Window
{
public:
    CLIWindow();

private:
    Gtk::Box box{Gtk::Orientation::VERTICAL};
    Gtk::Entry command_entry;
    Gtk::Button run_button{"Run"};
    Gtk::ScrolledWindow scrolled_window;
    Gtk::TextView output_text_view;

    void on_run_clicked();
};

// NetworkingWindow: This window displays networking commands and their output.
class NetworkingWindow : public Gtk::Window
{
public:
    NetworkingWindow();

private:
    // Member variables:
    Gtk::Label output_label;             // Widget for displaying command output.
    Gtk::ScrolledWindow scrolled_window; // Makes the output scrollable.
    Gtk::Entry *ping_entry;              // We'll create and manage this via Gtk::make_managed.
    Gtk::Entry *trace_route_entry;

    // Signal handlers:
    void on_ipconfig_clicked();
    void on_help_clicked();
    void on_exit_clicked();
    void on_ping_clicked();
    void on_trace_route_clicked();
};

// Other window declarations:
class FileManagementWindow : public Gtk::Window
{
public:
    FileManagementWindow();

private:
    Gtk::Label output_label;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Entry *mkdir_entry;
    Gtk::Entry *type_entry;
    // Gtk::Entry *cd_entry;

    void on_pwd_clicked();
    void on_mkdir_clicked();
    void on_dir_clicked();
    void on_type_clicked();
    void on_exit_clicked();
    // void on_cd_clicked();
};

class SystemInformationWindow : public Gtk::Window
{
public:
    SystemInformationWindow();

private:
    // Output Area
    Gtk::Label output_heading_label;
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Label output_label;

    // System Information Buttons
    Gtk::Button *btn_hostname;
    Gtk::Button *btn_os_details;
    Gtk::Button *btn_cpu_info;
    Gtk::Button *btn_memory_info;
    Gtk::Button *btn_disk_info;
    Gtk::Button *btn_exit;

    // Signal Handlers
    void on_hostname_clicked();
    void on_os_details_clicked();
    void on_cpu_info_clicked();
    void on_memory_info_clicked();
    void on_disk_info_clicked();
    void on_click_exit_window();
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
    Gtk::Button cli_button{"Open CLI"};
    Gtk::Box box{Gtk::Orientation::VERTICAL};

    void on_cli_clicked();
    void on_networking_clicked();
    void on_file_management_clicked();
    void on_system_information_clicked();
    void on_exit_clicked();
};

#endif // MYWINDOW_H