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
    set_default_size(1000, 1000);

    // Create a vertical box for layout.
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);

    // Create the ipconfig, help, and exit buttons.
    auto btn_ipconfig = Gtk::make_managed<Gtk::Button>("ipconfig");
    auto btn_help = Gtk::make_managed<Gtk::Button>("Help");
    auto btn_exit = Gtk::make_managed<Gtk::Button>("Exit");

    // --- Create the Ping controls ---
    auto label_ping = Gtk::make_managed<Gtk::Label>("Enter target for Ping:");
    ping_entry = Gtk::make_managed<Gtk::Entry>(); // Allocate the entry so it can be accessed in on_ping_clicked.
    auto btn_ping = Gtk::make_managed<Gtk::Button>("Ping");

    auto label_trace_route = Gtk::make_managed<Gtk::Label>("Enter target to trace route:");
    trace_route_entry = Gtk::make_managed<Gtk::Entry>();
    auto btn_trace_route = Gtk::make_managed<Gtk::Button>("Trace Route");

    // Connect button signals to their handlers.
    btn_ipconfig->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_ipconfig_clicked));
    btn_ping->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_ping_clicked));
    btn_help->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_help_clicked));
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_exit_clicked));
    btn_trace_route->signal_clicked().connect(sigc::mem_fun(*this, &NetworkingWindow::on_trace_route_clicked));

    output_label.set_text("Command Output Will Be Displayed Here....");

    // Configure the scrolled window to display command output.
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(output_label);
    scrolled_window.set_expand(true);

    // Pack everything into the vertical box.
    vbox->append(*btn_ipconfig);
    // --- Ping controls ---
    vbox->append(*label_ping);
    vbox->append(*ping_entry);
    vbox->append(*btn_ping);

    vbox->append(*label_trace_route);
    vbox->append(*trace_route_entry);
    vbox->append(*btn_trace_route);

    // Display the command output.
    vbox->append(scrolled_window);
    vbox->append(*btn_help);
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

void NetworkingWindow::on_help_clicked()
{
}

void NetworkingWindow::on_exit_clicked()
{
    std::cout << "Closing Networking window..." << std::endl;
    close();
}