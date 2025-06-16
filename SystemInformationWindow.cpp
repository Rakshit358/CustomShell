#include "MyWindow.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <gdkmm/clipboard.h>


SystemInformationWindow::SystemInformationWindow()
{
    set_title("System Information");
    set_resizable(true);
    set_default_size(800, 600);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    vbox->set_margin(15);

    auto header_label = Gtk::make_managed<Gtk::Label>("System Information Panel");
    header_label->get_style_context()->add_class("header");
    vbox->append(*header_label);

    // Output area
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

    // Grid layout for buttons
    auto grid = Gtk::make_managed<Gtk::Grid>();
    grid->set_column_spacing(12);
    grid->set_row_spacing(10);

    btn_hostname = Gtk::make_managed<Gtk::Button>("Hostname");
    btn_os_details = Gtk::make_managed<Gtk::Button>("OS Details");
    btn_cpu_info = Gtk::make_managed<Gtk::Button>("CPU Info");
    btn_memory_info = Gtk::make_managed<Gtk::Button>("Memory Info");
    btn_disk_info = Gtk::make_managed<Gtk::Button>("Disk Info");
    btn_exit = Gtk::make_managed<Gtk::Button>("Exit");

    btn_hostname->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_hostname_clicked));
    btn_os_details->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_os_details_clicked));
    btn_cpu_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_cpu_info_clicked));
    btn_memory_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_memory_info_clicked));
    btn_disk_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_disk_info_clicked));
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_click_exit_window));

    grid->attach(*btn_hostname,     0, 0, 1, 1);
    grid->attach(*btn_os_details,   1, 0, 1, 1);
    grid->attach(*btn_cpu_info,     0, 1, 1, 1);
    grid->attach(*btn_memory_info,  1, 1, 1, 1);
    grid->attach(*btn_disk_info,    0, 2, 2, 1);

    // Graph
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

    vbox->append(*btn_copy);
    vbox->append(*btn_exit);
    set_child(*vbox);
}

void SystemInformationWindow::on_click_exit_window()
{
    close();
}

void SystemInformationWindow::on_hostname_clicked()
{
    auto start = std::chrono::steady_clock::now();

    std::string command = "hostname";
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            result += buffer;
        pclose(pipe);
    } else {
        result = "Error fetching hostname.";
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text("Hostname:\n" + result);
    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void SystemInformationWindow::on_os_details_clicked()
{
    auto start = std::chrono::steady_clock::now();

    std::string command;

    #if defined(_WIN32) || defined(_WIN64)
    command = "systeminfo"; // Windows
    #else
    std::string command = "uname -a"; // Linux/macOS
    #endif
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            result += buffer;
        pclose(pipe);
    } else {
        result = "Error fetching OS details.";
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text("OS Details:\n" + result);
    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void SystemInformationWindow::on_cpu_info_clicked()
{
    auto start = std::chrono::steady_clock::now();

    std::string command;

    #if defined(_WIN32) || defined(_WIN64)
    command = "wmic cpu get name, NumberOfCores, MaxClockSpeed"; // Windows CPU info
    #else
    command = "lscpu"; // Linux/macOS command for CPU details
    #endif
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            result += buffer;
        pclose(pipe);
    } else {
        result = "Error fetching CPU information.";
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text("CPU Info:\n" + result);
    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void SystemInformationWindow::on_memory_info_clicked()
{
    auto start = std::chrono::steady_clock::now();

    std::string command;

    #if defined(_WIN32) || defined(_WIN64)
    command = "wmic OS get FreePhysicalMemory, TotalVisibleMemorySize"; // Windows memory stats
    #else
    command = "free -h"; // Linux/macOS command for memory usage
    #endif
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            result += buffer;
        pclose(pipe);
    } else {
        result = "Error fetching memory information.";
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text("Memory Info:\n" + result);
    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void SystemInformationWindow::on_disk_info_clicked()
{
    auto start = std::chrono::steady_clock::now();

    std::string command;

    #if defined(_WIN32) || defined(_WIN64)
    command = "wmic logicaldisk get size,freespace,caption"; // Windows disk info
    #else
    command = "df -h"; // Linux/macOS command for disk space usage
    #endif
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
            result += buffer;
        pclose(pipe);
    } else {
        result = "Error fetching disk information.";
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text("Disk Info:\n" + result);
    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}
