#include "MyWindow.h"
#include <glibmm.h>
#include <iostream>
#include <sstream>

CLIWindow::CLIWindow()
{
    set_title("CLI Shell");
    set_default_size(800, 800);

    box.set_spacing(10);
    box.set_margin(10);

    command_entry.set_placeholder_text("Enter shell command here...");
    box.append(command_entry);

    run_button.signal_clicked().connect(sigc::mem_fun(*this, &CLIWindow::on_run_clicked));
    box.append(run_button);

    scrolled_window.set_child(output_text_view);
    scrolled_window.set_expand(true);
    box.append(scrolled_window);

    output_text_view.set_editable(false);
    output_text_view.set_wrap_mode(Gtk::WrapMode::WORD_CHAR);

    set_child(box);
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
