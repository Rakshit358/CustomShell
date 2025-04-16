#include "MyWindow.h"
#include <iostream>
#include <cstdio>

SystemInformationWindow::SystemInformationWindow()
{
    auto css_provider = Gtk::CssProvider::create();
    try
    {
        css_provider->load_from_path("style.css");
    }
    catch (const Glib::Error &ex)
    {
        std::cerr << "CSS load error: " << ex.what() << std::endl;
    }

    set_title("System Information");
    set_default_size(800, 600);
    set_margin(10);

    // Main container
    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    vbox->set_margin(10);

    auto btn_container = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    btn_container->set_halign(Gtk::Align::CENTER); // Equivalent to justify-content: center;
    btn_container->set_valign(Gtk::Align::CENTER); // Equivalent to align-items: center;

    // Output heading
    output_heading_label.set_text("System Information Output:");
    output_heading_label.set_margin_bottom(5);

    output_heading_label.get_style_context()->add_class("special-label");

    output_heading_label.get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    vbox->append(output_heading_label);

    // Scrollable output area
    output_label.set_text("Command output will appear here...");
    output_label.set_wrap(true);
    scrolled_window.set_child(output_label);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_expand(true);
    scrolled_window.set_margin_bottom(20);
    // Command buttons
    btn_hostname = Gtk::make_managed<Gtk::Button>("Hostname");
    btn_hostname->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_hostname_clicked));
    btn_os_details = Gtk::make_managed<Gtk::Button>("OS Details");
    btn_os_details->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_os_details_clicked));
    btn_cpu_info = Gtk::make_managed<Gtk::Button>("CPU Info");
    btn_cpu_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_cpu_info_clicked));
    btn_memory_info = Gtk::make_managed<Gtk::Button>("Memory Info");
    btn_memory_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_memory_info_clicked));
    btn_disk_info = Gtk::make_managed<Gtk::Button>("Disk Info");
    btn_disk_info->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_disk_info_clicked));

    btn_exit = Gtk::make_managed<Gtk::Button>("Exit");
    btn_exit->signal_clicked().connect(sigc::mem_fun(*this, &SystemInformationWindow::on_click_exit_window));
    btn_container->append(*btn_hostname);
    btn_container->append(*btn_os_details);
    btn_container->append(*btn_cpu_info);
    btn_container->append(*btn_memory_info);
    btn_container->append(*btn_disk_info);
    vbox->append(*btn_container);
    vbox->append(*btn_exit);
    vbox->append(scrolled_window);

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
