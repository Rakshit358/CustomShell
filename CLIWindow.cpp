#include "MyWindow.h"
#include <glibmm.h>
#include <iostream>
#include <sstream>

CLIWindow::CLIWindow()
{
    set_title("CLI Shell");
    set_resizable(true);
    set_default_size(1200, 1200);
    set_margin(10);

    auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    vbox->set_margin(15);

    auto header_label = Gtk::make_managed<Gtk::Label>("Command Line Interface");
    header_label->get_style_context()->add_class("header");
    header_label->set_margin_bottom(10);
    vbox->append(*header_label);

    command_entry.set_placeholder_text("Enter shell command here...");
    command_entry.set_margin_bottom(5);
    vbox->append(command_entry);

    // Run and Copy Buttons
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

    // Output Area
    output_text_view.set_editable(false);
    output_text_view.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);
    output_text_view.get_style_context()->add_class("output-area");

    scrolled_window.set_child(output_text_view);
    scrolled_window.set_expand(true);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    auto output_frame = Gtk::make_managed<Gtk::Frame>("Output");
    output_frame->set_child(scrolled_window);
    vbox->append(*output_frame);

    set_child(*vbox);
}

void CLIWindow::on_run_clicked()
{
    std::string command = command_entry.get_text();
    if (command.empty())
        return;

    std::string result;
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        result = "Failed to run command.\n";
    }
    else
    {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        {
            result += buffer;
        }
        pclose(pipe);
    }

    output_text_view.get_buffer()->set_text(result);
}
