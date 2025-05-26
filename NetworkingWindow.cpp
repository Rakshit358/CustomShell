#include "MyWindow.h"
#include <iostream>
#include <cstdio> // For popen, fgets, and pclose
#include <thread>
#include <glibmm/main.h> // For Glib::signal_idle

// A helper function that executes a command using popen and returns its complete output.
std::string execute_command(const std::string &command)
{
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
        return "Error executing command.";
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

NetworkingWindow::NetworkingWindow()
{
    set_title("Networking Commands");
    set_resizable(true);
    set_default_size(1200, 1200);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    vbox->set_margin(15);

    auto header_label = Gtk::make_managed<Gtk::Label>("Networking Panel");
    header_label->get_style_context()->add_class("header");
    vbox->append(*header_label);

    output_label.set_text("Command output will appear here...");
    output_label.set_wrap(true);
    output_label.set_hexpand(true);
    output_label.set_vexpand(true);
    output_label.get_style_context()->add_class("output-area");

    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_expand(true);
    scrolled_window.set_child(output_label);

    auto output_frame = Gtk::make_managed<Gtk::Frame>("Output");
    output_frame->set_child(scrolled_window);
    output_frame->set_margin_bottom(10);
    vbox->append(*output_frame);

    auto grid = Gtk::make_managed<Gtk::Grid>();
    grid->set_column_spacing(12);
    grid->set_row_spacing(10);

    auto btn_ipconfig = Gtk::make_managed<Gtk::Button>("ipconfig");
    btn_ipconfig->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_ipconfig_clicked));
    grid->attach(*btn_ipconfig, 0, 0, 1, 1);

    ping_entry = Gtk::make_managed<Gtk::Entry>();
    ping_entry->set_placeholder_text("Target for Ping");
    auto btn_ping = Gtk::make_managed<Gtk::Button>("Ping");
    btn_ping->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_ping_clicked));
    grid->attach(*btn_ping, 0, 1, 1, 1);
    grid->attach(*ping_entry, 1, 1, 1, 1);

    trace_route_entry = Gtk::make_managed<Gtk::Entry>();
    trace_route_entry->set_placeholder_text("Target for Trace Route");
    auto btn_trace = Gtk::make_managed<Gtk::Button>("Trace Route");
    btn_trace->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_trace_route_clicked));
    grid->attach(*btn_trace, 0, 2, 1, 1);
    grid->attach(*trace_route_entry, 1, 2, 1, 1);

    vbox->append(*grid);

    auto btn_copy = Gtk::make_managed<Gtk::Button>("Copy Output");
    btn_copy->signal_clicked().connect([this]() {
        auto clipboard = Gdk::Display::get_default()->get_clipboard();
        clipboard->set_text(output_label.get_text());
    });

    auto btn_exit = Gtk::make_managed<Gtk::Button>("Exit");
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_exit_clicked));

    vbox->append(*btn_copy);
    vbox->append(*btn_exit);

    set_child(*vbox);
}

void NetworkingWindow::on_ipconfig_clicked()
{
    // (This implementation remains as before.)
    std::string command = "ipconfig"; // For Linux, consider "ifconfig" or "ip a"
    std::cout << "Executing ipconfig command..." << std::endl;
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

void NetworkingWindow::on_ping_clicked()
{
    std::string target = ping_entry->get_text();
    if (target.empty())
    {
        output_label.set_text("Please enter a target host or IP address.");
        return;
    }

    // Build the ping command with parameters to limit packets.
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "ping -n 4 " + target;
#else
    std::string command = "ping -c 4 " + target;
#endif

    std::cout << "Executing ping command: " << command << std::endl;

    // Run the command asynchronously.
    std::thread([this, command]()
                {
        std::string result = execute_command(command);
        // Post the result back to the UI thread.
        Glib::signal_idle().connect_once([this, result]() {
            // output_label.set_line_wrap(true);
            output_label.set_text(result);
        }); })
        .detach();
}

void NetworkingWindow::on_trace_route_clicked()
{
    std::string target = trace_route_entry->get_text();
    if (target.empty())
    {
        output_label.set_text("Please enter a target host or IP address.");
        return;
    }

    // Build the tracert/traceroute command.
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "tracert " + target;
#else
    std::string command = "traceroute " + target;
#endif

    std::cout << "Executing trace route command: " << command << std::endl;

    // Run the command asynchronously.
    std::thread([this, command]()
                {
        std::string result = execute_command(command);
        // Post the result back to the UI thread.
        Glib::signal_idle().connect_once([this, result]() {
            // output_label.set_line_wrap(true);
            output_label.set_text(result);
        }); })
        .detach();
}


void NetworkingWindow::on_exit_clicked()
{
    std::cout << "Closing Networking window..." << std::endl;
    close();
}