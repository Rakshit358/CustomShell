#include "MyWindow.h"
#include <glibmm.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <gdkmm/clipboard.h>

CLIWindow::CLIWindow()
{
    set_title("CLI Shell");
    set_resizable(true);
    set_default_size(800, 600);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    vbox->set_margin(15);

    auto header_label = Gtk::make_managed<Gtk::Label>("Command Line Interface");
    header_label->get_style_context()->add_class("header");
    vbox->append(*header_label);

    command_entry.set_placeholder_text("Enter shell command here...");
    vbox->append(command_entry);

    // Buttons
    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    run_button.set_label("Run");
    run_button.signal_clicked().connect(sigc::mem_fun(*this, &CLIWindow::on_run_clicked));
    button_box->append(run_button);

    auto copy_button = Gtk::make_managed<Gtk::Button>("Copy Output");
    copy_button->signal_clicked().connect([this]() {
        auto clipboard = Gdk::Display::get_default()->get_clipboard();
        clipboard->set_text(output_text_view.get_buffer()->get_text());
    });
    button_box->append(*copy_button);
    vbox->append(*button_box);

    // Output area
    output_text_view.set_editable(false);
    output_text_view.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
    output_text_view.get_style_context()->add_class("output-area");

    scrolled_window.set_child(output_text_view);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_expand(true);

    auto output_frame = Gtk::make_managed<Gtk::Frame>("Output");
    output_frame->set_child(scrolled_window);
    vbox->append(*output_frame);

    // Execution time graph
    cli_exec_times_ms.resize(50, 0);

    cli_graph_area.set_content_width(250);
    cli_graph_area.set_content_height(150);
    cli_graph_area.set_expand(true);

    cli_graph_area.set_draw_func([this](const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (cli_exec_times_ms.empty()) return;

        cr->set_line_width(1.0);
        cr->set_source_rgb(0.5, 0.5, 0.5);

        // Axes
        cr->move_to(30, 10);
        cr->line_to(30, height - 20);
        cr->stroke();

        cr->move_to(30, height - 20);
        cr->line_to(width - 10, height - 20);
        cr->stroke();

        // Y-axis label and ticks
        cr->set_source_rgb(0, 0, 0);
        cr->set_font_size(12);
        cr->move_to(10, 12);  // prevent overlap with top tick
        cr->show_text("ms");

        for (int i = 0; i <= 4; ++i) {
            int y_val = i * (height - 30) / 4;
            int label_val = 200 - i * 50;
            cr->move_to(5, y_val + 20);
            cr->show_text(std::to_string(label_val));
        }

        // Plot line
        cr->set_source_rgb(1.0, 0.4, 0.3);
        cr->set_line_width(2.0);

        cr->move_to(30, height - 20 - std::min(height - 40.0, static_cast<double>(cli_exec_times_ms[0])));
        for (size_t i = 1; i < cli_exec_times_ms.size(); ++i) {
            if (cli_exec_times_ms.empty()) return;
            double x = 30 + i * ((width - 40) / static_cast<double>(cli_exec_times_ms.size()));
            double y = height - 20 - std::min(height - 40.0, static_cast<double>(cli_exec_times_ms[i]));
            cr->line_to(x, y);
        }
        cr->stroke();
    });

    vbox->append(cli_graph_area);
    set_child(*vbox);
}

void CLIWindow::on_run_clicked()
{
    std::string command = command_entry.get_text();
    if (command.empty())
        return;

    auto start = std::chrono::steady_clock::now();

    std::string result;
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        result = "Failed to run command.\n";
    } else {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
    }

    auto end = std::chrono::steady_clock::now();
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    output_text_view.get_buffer()->set_text(result);

    cli_exec_times_ms.push_back(ms);
    if (cli_exec_times_ms.size() > 50)
        cli_exec_times_ms.erase(cli_exec_times_ms.begin());

    cli_graph_area.queue_draw();
}
