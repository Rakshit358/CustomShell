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

    // Row 3 - Move/Rename File
    auto mv_source_entry = Gtk::make_managed<Gtk::Entry>();
    mv_source_entry->set_placeholder_text("Source file or directory");
    auto mv_dest_entry = Gtk::make_managed<Gtk::Entry>();
    mv_dest_entry->set_placeholder_text("Destination path or name");
    auto btn_mv = Gtk::make_managed<Gtk::Button>("Move/Rename (mv)");
    btn_mv->signal_clicked().connect([this, mv_source_entry, mv_dest_entry]() {
        auto start = std::chrono::steady_clock::now();
        std::string src = mv_source_entry->get_text();
        std::string dest = mv_dest_entry->get_text();
        if (src.empty() || dest.empty()) {
            output_label.set_text("Please enter both source and destination.");
            return;
        }
        std::string command = "mv '" + src + "' '" + dest + "'";
        std::cout << "Executing mv..." << std::endl;
        std::string result;
        FILE *pipe = popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }
        auto end = std::chrono::steady_clock::now();
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (result.empty()) result = "Move/Rename operation completed.";
        output_label.set_text(result);
        sys_exec_times_ms.push_back(ms);
        if (sys_exec_times_ms.size() > 50)
            sys_exec_times_ms.erase(sys_exec_times_ms.begin());
        sys_graph_area.queue_draw();
    });
    grid->attach(*btn_mv, 0, 3, 1, 1);
    grid->attach(*mv_source_entry, 1, 3, 1, 1);
    grid->attach(*mv_dest_entry, 2, 3, 1, 1);

    // Row 4 - Remove File/Directory
    auto rm_entry = Gtk::make_managed<Gtk::Entry>();
    rm_entry->set_placeholder_text("File or directory to remove");
    auto btn_rm = Gtk::make_managed<Gtk::Button>("Remove (rm)");
    btn_rm->signal_clicked().connect([this, rm_entry]() {
        auto start = std::chrono::steady_clock::now();
        std::string target = rm_entry->get_text();
        if (target.empty()) {
            output_label.set_text("Please enter a file or directory to remove.");
            return;
        }
        std::string command = "rm -rf '" + target + "'";
        std::cout << "Executing rm..." << std::endl;
        std::string result;
        FILE *pipe = popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }
        auto end = std::chrono::steady_clock::now();
        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (result.empty()) result = "Remove operation completed.";
        output_label.set_text(result);
        sys_exec_times_ms.push_back(ms);
        if (sys_exec_times_ms.size() > 50)
            sys_exec_times_ms.erase(sys_exec_times_ms.begin());
        sys_graph_area.queue_draw();
    });
    grid->attach(*btn_rm, 0, 4, 1, 1);
    grid->attach(*rm_entry, 1, 4, 2, 1);

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
    auto start = std::chrono::steady_clock::now();

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
    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text(result);

    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void FileManagementWindow::on_dir_clicked()
{
    auto start = std::chrono::steady_clock::now();

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
    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text(result);

    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

void FileManagementWindow::on_mkdir_clicked()
{
    auto start = std::chrono::steady_clock::now();

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
    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text(result);

    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
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
    auto start = std::chrono::steady_clock::now();

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
    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_label.set_text(result);

    sys_exec_times_ms.push_back(ms);
    if (sys_exec_times_ms.size() > 50)
        sys_exec_times_ms.erase(sys_exec_times_ms.begin());

    sys_graph_area.queue_draw();
}

// ... existing code ...
