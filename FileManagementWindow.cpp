#include "MyWindow.h"
#include <iostream>
#include <cstdio>
#include <thread>


FileManagementWindow::FileManagementWindow()
{
    set_title("File Management commands");
    set_default_size(1000, 1000);
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    auto btn_pwd = Gtk::make_managed<Gtk::Button>("Check working directory");

    auto label_mkdir = Gtk::make_managed<Gtk::Label>("Enter directory name:");
    mkdir_entry = Gtk::make_managed<Gtk::Entry>();
    auto btn_mkdir = Gtk::make_managed<Gtk::Button>("Make new directory");

    auto btn_dir = Gtk::make_managed<Gtk::Button>("Check all directories");

    auto label_type = Gtk::make_managed<Gtk::Label>("Enter file name:");
    type_entry = Gtk::make_managed<Gtk::Entry>();
    auto btn_type = Gtk::make_managed<Gtk::Button>("Display file contents");

   /* auto label_cd = Gtk::make_managed<Gtk::Label>("Enter path:");
    cd_entry = Gtk::make_managed<Gtk::Entry>();
    auto btn_cd = Gtk::make_managed<Gtk::Button>("Change directory");*/

    auto btn_exit = Gtk::make_managed<Gtk::Button>("Exit");

    btn_pwd->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_pwd_clicked));
    btn_mkdir->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_mkdir_clicked));
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_exit_clicked));
    btn_dir->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_dir_clicked));
    btn_type->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_type_clicked));
   // btn_cd->signal_clicked().connect(sigc::mem_fun(*this, &FileManagementWindow::on_cd_clicked));


    output_label.set_text("Command Output Will Be Displayed Here....");
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(output_label);
    scrolled_window.set_expand(true);

    vbox->append(*btn_pwd);

    vbox->append(*btn_dir);

    vbox->append(*label_mkdir);
    vbox->append(*mkdir_entry);
    vbox->append(*btn_mkdir);

    vbox->append(*label_type);
    vbox->append(*type_entry);
    vbox->append(*btn_type);

   /* vbox->append(*label_cd);
    vbox->append(*cd_entry);
    vbox->append(*btn_cd);*/
    
    vbox->append(scrolled_window);
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
    if(target.empty())
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
