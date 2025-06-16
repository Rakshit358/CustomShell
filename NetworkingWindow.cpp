#include "MyWindow.h"
#include <iostream>
#include <cstdio> // For popen, fgets, and pclose
#include <thread>
#include <glibmm/main.h>

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

    //vbox->append(*grid);

    sys_exec_times_ms.resize(50, 0);

    sys_graph_area.set_content_width(250);
    sys_graph_area.set_content_height(150);
    sys_graph_area.set_hexpand(true);

    sys_graph_area.set_draw_func([this](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (sys_exec_times_ms.empty()) return;

        cr->set_line_width(1.0);
        cr->set_source_rgb(0.5, 0.5, 0.5); 

        cr->move_to(30, 10);
        cr->line_to(30, height - 20);
        cr->stroke();

        cr->move_to(30, height - 20);
        cr->line_to(width - 10, height - 20);
        cr->stroke();

        cr->set_source_rgb(0, 0, 0);
       // cr->select_font_face("Sans", Cairo::FontSlant::NORMAL, Cairo::FontWeight::NORMAL);
        cr->set_font_size(12);
        cr->move_to(10, 12);
        cr->show_text("ms");

        for (int i = 0; i <= 4; ++i) {
            int y_val = i * (height - 30) / 4;
            int label_val = 200 - i * 50; 
            cr->move_to(5, y_val + 20);
            cr->show_text(std::to_string(label_val));
        }

        cr->set_source_rgb(1.0, 0.4, 0.3);
        cr->set_line_width(2.0);

        cr->move_to(30, height - 20 - std::min(height - 40.0, static_cast<double>(sys_exec_times_ms[0])));
        for (size_t i = 1; i < sys_exec_times_ms.size(); ++i) {
                if (sys_exec_times_ms.empty()) return;
                double x = 30 + i * ((width - 40) / static_cast<double>(sys_exec_times_ms.size()));
                double y = height - 20 - std::min(height - 40.0, static_cast<double>(sys_exec_times_ms[i]));
                cr->line_to(x, y);
        }
        cr->stroke();
    });


    auto hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    hbox->append(*grid);
    hbox->append(sys_graph_area);
    vbox->append(*hbox);

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
    auto start = std::chrono::steady_clock::now();
    std::string command = "ipconfig";
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
    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text(result);
    sys_exec_times_ms.push_back(ms);
        if (sys_exec_times_ms.size() > 50)
            sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void NetworkingWindow::on_ping_clicked()
{
    auto start = std::chrono::steady_clock::now();
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
    std::thread([this, command, start]()
                {
        std::string result = execute_command(command);
        // Post the result back to the UI thread.
        Glib::signal_idle().connect_once([this, result,start]() {
            // output_label.set_line_wrap(true);
            auto end = std::chrono::steady_clock::now();
            int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            output_label.set_text(result);
            sys_exec_times_ms.push_back(ms);
            if (sys_exec_times_ms.size() > 50)
                sys_exec_times_ms.erase(sys_exec_times_ms.begin());

            sys_graph_area.queue_draw();
        }); })
        .detach();
}

void NetworkingWindow::on_trace_route_clicked()
{
    auto start = std::chrono::steady_clock::now();
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
    std::thread([this, command, start]()
                {
        std::string result = execute_command(command);
        // Post the result back to the UI thread.
        Glib::signal_idle().connect_once([this, result, start]() {
            // output_label.set_line_wrap(true);
            auto end = std::chrono::steady_clock::now();
            int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            output_label.set_text(result);
            sys_exec_times_ms.push_back(ms);
            if (sys_exec_times_ms.size() > 50)
                sys_exec_times_ms.erase(sys_exec_times_ms.begin());

            sys_graph_area.queue_draw();
        }); })
        .detach();
}


void NetworkingWindow::on_exit_clicked()
{
    std::cout << "Closing Networking window..." << std::endl;
    close();
}