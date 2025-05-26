#include "MyWindow.h"
#include <iostream>
#include <cstdio>
#include <thread>

FileManagementWindow::FileManagementWindow()
{
    set_title("File Management");
    set_resizable(true);
    set_default_size(1200, 1200);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    vbox->set_margin(15);

    // Header
    auto header_label = Gtk::make_managed<Gtk::Label>("File Management Panel");
    header_label->get_style_context()->add_class("header");
    vbox->append(*header_label);

    // Output
    output_label.set_text("Command output will appear here...");
    output_label.set_wrap(true);
    output_label.set_hexpand(true);
    output_label.set_vexpand(true);
    output_label.get_style_context()->add_class("output-area");

    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(output_label);
    scrolled_window.set_expand(true);

    auto output_frame = Gtk::make_managed<Gtk::Frame>("Output");
    output_frame->set_child(scrolled_window);
    output_frame->set_margin_bottom(10);
    vbox->append(*output_frame);

    // Grid Layout
    auto grid = Gtk::make_managed<Gtk::Grid>();
    grid->set_column_spacing(12);
    grid->set_row_spacing(10);

    // Row 0 - PWD and DIR
    auto btn_pwd = Gtk::make_managed<Gtk::Button>("Check Working Directory");
    auto btn_dir = Gtk::make_managed<Gtk::Button>("List Directories");
    btn_pwd->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_pwd_clicked));
    btn_dir->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_dir_clicked));
    grid->attach(*btn_pwd, 0, 0, 1, 1);
    grid->attach(*btn_dir, 1, 0, 1, 1);

    // Row 1 - Create Directory
    mkdir_entry = Gtk::make_managed<Gtk::Entry>();
    mkdir_entry->set_placeholder_text("Directory name");
    auto btn_mkdir = Gtk::make_managed<Gtk::Button>("Create Directory");
    btn_mkdir->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_mkdir_clicked));
    grid->attach(*btn_mkdir, 0, 1, 1, 1);
    grid->attach(*mkdir_entry, 1, 1, 1, 1);

    // Row 2 - Display File Contents
    type_entry = Gtk::make_managed<Gtk::Entry>();
    type_entry->set_placeholder_text("File name");
    auto btn_type = Gtk::make_managed<Gtk::Button>("Display File Contents");
    btn_type->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_type_clicked));
    grid->attach(*btn_type, 0, 2, 1, 1);
    grid->attach(*type_entry, 1, 2, 1, 1);

    vbox->append(*grid);

    // Copy and Exit
    auto btn_copy = Gtk::make_managed<Gtk::Button>("Copy Output");
    btn_copy->signal_clicked().connect([this]() {
        auto clipboard = Gdk::Display::get_default()->get_clipboard();
        clipboard->set_text(output_label.get_text());
    });

    auto btn_exit = Gtk::make_managed<Gtk::Button>("Exit");
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_exit_clicked));

    vbox->append(*btn_copy);
    vbox->append(*btn_exit);

    set_child(*vbox);
}

void FileManagementWindow::on_pwd_clicked()
{
    std::string command = "pwd";
    std::cout << "Executing pwd..." << std::endl;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_label.set_text(result);
}

void FileManagementWindow::on_dir_clicked()
{
    std::string command = "dir";
    std::cout << "Executing dir..." << std::endl;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_label.set_text(result);
}

void FileManagementWindow::on_mkdir_clicked()
{
    std::string target = mkdir_entry->get_text();
    if (target.empty())
    {
        output_label.set_text("Please enter path and directory name.");
        return;
    }

    std::string command = "mkdir " + target;
    std::cout << "Executing mkdir..." << std::endl;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_label.set_text(result);
}

/*void FileManagementWindow::on_cd_clicked()
{
    std::string target = cd_entry->get_text();
    if (target.empty())
    {
        output_label.set_text("Please enter path and directory name.");
        return;
    }

    std::string command = "cd" + target;
    std::cout << "Executing cd..." << std::endl;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_label.set_text(result);
}*/

void FileManagementWindow::on_type_clicked()
{
    std::string target = type_entry->get_text();
    if (target.empty())
    {
        output_label.set_text("Please enter file name.");
        return;
    }

    std::string command = "type " + target;
    std::cout << "Executing type..." << std::endl;
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_label.set_text(result);
}

void FileManagementWindow::on_exit_clicked()
{
    std::cout << "Closing File Management window..." << std::endl;
    close();
}
