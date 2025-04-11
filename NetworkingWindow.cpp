#include "MyWindow.h"
#include <iostream>
#include <cstdio> // For popen, fgets, pclose

NetworkingWindow::NetworkingWindow()
{
    set_title("Networking Commands");
    set_default_size(800, 800);

    // Create a vertical box for layout
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);

    // Create the command button(s)
    auto btn_ipconfig = Gtk::make_managed<Gtk::Button>("ipconfig");
    auto btn_help = Gtk::make_managed<Gtk::Button>("Help");
    auto btn_exit = Gtk::make_managed<Gtk::Button>("Exit");

    // Connect the button signals to their handlers
    btn_ipconfig->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_ipconfig_clicked));
    btn_help->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_help_clicked));
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_exit_clicked));

    // Configure the scrolled window to display output from the command
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(output_label);
    scrolled_window.set_expand(true);

    // Pack the widgets in the box: button for ipconfig, then the scrollable output, then help and exit buttons
    vbox->append(*btn_ipconfig);
    vbox->append(scrolled_window);
    vbox->append(*btn_help);
    vbox->append(*btn_exit);

    // Set vbox as the main child for this window
    set_child(*vbox);
}

void NetworkingWindow::on_ipconfig_clicked()
{
    std::string command = "ipconfig"; // Change this if targeting Linux (to "ifconfig" or "ip a")
    std::cout << "Executing ipconfig command..." << std::endl;
    std::string result;

    // Execute the command and capture output
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

    // Set the result as the text for the output label.
    output_label.set_text(result);
}

void NetworkingWindow::on_help_clicked()
{
}

void NetworkingWindow::on_exit_clicked()
{
    std::cout << "Closing Networking window..." << std::endl;
    close();
}