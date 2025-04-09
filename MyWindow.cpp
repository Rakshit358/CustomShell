#include "MyWindow.h"
#include <iostream>
#include <cstdio>

MyWindow::MyWindow()
{
    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("style.css");

    auto context = get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    context = btn1.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    context = btn2.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    context = btn3.get_style_context();
    context->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    btn1.set_size_request(200, 50);
    btn1.set_halign(Gtk::Align::CENTER);
    btn2.set_size_request(200, 50);
    btn2.set_halign(Gtk::Align::CENTER);
    btn3.set_size_request(200, 50);
    btn3.set_halign(Gtk::Align::CENTER);

    box.set_spacing(15);
    btn1.set_margin(10);
    btn2.set_margin(10);
    btn3.set_margin(10);

    set_title("Custom Shell");
    set_default_size(800, 800);

    command_map["Run ipconfig"] = "ipconfig";
    command_map["Check current working directory"] = "pwd";

    btn1.signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MyWindow::on_button_clicked), &btn1));
    btn2.signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MyWindow::on_button_clicked), &btn2));
    btn3.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_exit_clicked));

    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_child(output_label);
    scrolled_window.set_expand(true);

    box.append(btn1);
    box.append(btn2);
    box.append(scrolled_window);
    box.append(btn3);

    set_child(box);
}

void MyWindow::on_exit_clicked()
{
    std::cout << "Closing the window..." << std::endl;
    close();
}

void MyWindow::on_button_clicked(Gtk::Button *clicked_button)
{
    std::string command_label = clicked_button->get_label();
    std::string command = command_map[command_label];

    std::cout << "Executing: " << command << std::endl;
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
