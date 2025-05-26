#include "MyWindow.h"
#include <iostream>
#include <cstdio>
#include <gdkmm/clipboard.h>

SystemInformationWindow::SystemInformationWindow()
{
    set_title("System Information");
    set_resizable(true);
    set_default_size(1200, 1200);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    vbox->set_margin(10);

    auto header_label = Gtk::make_managed<Gtk::Label>("System Information Tool");
    header_label->get_style_context()->add_class("header");
    vbox->append(*header_label);

    output_heading_label.set_text("System Information Output:");
    output_heading_label.set_margin_bottom(5);
    output_heading_label.get_style_context()->add_class("special-label");
    vbox->append(output_heading_label);

    // Output Area
    output_label.set_text("Command output will appear here...");
    output_label.set_wrap(true);
    output_label.set_hexpand(true);
    output_label.set_vexpand(true);
    output_label.get_style_context()->add_class("output-area");

    scrolled_window.set_child(output_label);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_expand(true);
    scrolled_window.set_margin_bottom(10);

    auto output_frame = Gtk::make_managed<Gtk::Frame>("Output");
    output_frame->set_child(scrolled_window);
    vbox->append(*output_frame);

    // Grid Layout for Buttons
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
    

    grid->attach(*btn_hostname, 0, 0, 1, 1);
    grid->attach(*btn_os_details, 1, 0, 1, 1);
    grid->attach(*btn_cpu_info, 0, 1, 1, 1);
    grid->attach(*btn_memory_info, 1, 1, 1, 1);
    grid->attach(*btn_disk_info, 0, 2, 2, 1);
    vbox->append(*grid);

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

void SystemInformationWindow::on_disk_info_clicked()
{
    std::string command;

#if defined(_WIN32) || defined(_WIN64)
    command = "wmic logicaldisk get size,freespace,caption"; // Windows disk info
#else
    command = "df -h"; // Linux/macOS command for disk space usage
#endif

    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }
    else
    {
        result = "Error fetching disk information.";
    }

    output_label.set_text("Disk Info:\n" + result);
}

void SystemInformationWindow::on_memory_info_clicked()
{
    std::string command;

#if defined(_WIN32) || defined(_WIN64)
    command = "wmic OS get FreePhysicalMemory, TotalVisibleMemorySize"; // Windows memory stats
#else
    command = "free -h"; // Linux/macOS command for memory usage
#endif

    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }
    else
    {
        result = "Error fetching memory information.";
    }

    output_label.set_text("Memory Info:\n" + result);
}

void SystemInformationWindow::on_cpu_info_clicked()
{
    std::string command;

#if defined(_WIN32) || defined(_WIN64)
    command = "wmic cpu get name, NumberOfCores, MaxClockSpeed"; // Windows CPU info
#else
    command = "lscpu"; // Linux/macOS command for CPU details
#endif

    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (pipe)
    {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }
    else
    {
        result = "Error fetching CPU information.";
    }

    output_label.set_text("CPU Info:\n" + result);
}
void SystemInformationWindow::on_hostname_clicked()
{
    std::string command = "hostname"; // Cross-platform
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
    else
    {
        result = "Error fetching hostname.";
    }

    output_label.set_text("Hostname:\n" + result);
}

void SystemInformationWindow::on_os_details_clicked()
{
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "systeminfo"; // Windows
#else
    std::string command = "uname -a"; // Linux/macOS
#endif

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
    else
    {
        result = "Error fetching OS details.";
    }

    output_label.set_text("OS Details:\n" + result);
}
