#include "MyWindow.h"
#include <iostream>

MyWindow::MyWindow()
{
    set_title("AeroShell");
    set_default_size(800, 800);

    // Set up the main window
    box.set_spacing(15);
    btn1.set_margin(10);
    btn2.set_margin(10);
    btn3.set_margin(10);
    exit_button.set_margin(10);

    btn1.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_networking_clicked));
    btn2.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_file_management_clicked));
    btn3.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_system_information_clicked));
    exit_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_exit_clicked));

    cli_button.set_margin(10);
    cli_button.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_cli_clicked));

    box.append(cli_button);

    box.append(btn1);
    box.append(btn2);
    box.append(btn3);
    box.append(exit_button);
    set_child(box);
}

void MyWindow::on_cli_clicked()
{
    CLIWindow *cli_window = new CLIWindow();
    cli_window->show();
}

void MyWindow::on_networking_clicked()
{
    NetworkingWindow *networking_window = new NetworkingWindow();
    networking_window->show();
}

void MyWindow::on_file_management_clicked()
{
    FileManagementWindow *file_management_window = new FileManagementWindow();
    file_management_window->show();
}

void MyWindow::on_system_information_clicked()
{
    SystemInformationWindow *system_information_window = new SystemInformationWindow();
    system_information_window->show();
}

void MyWindow::on_exit_clicked()
{
    std::cout << "Closing the application..." << std::endl;
    close();
}