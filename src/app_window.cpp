#include "app_window.hpp"

app_window::app_window(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::ApplicationWindow(cobject)
{
}

app_window_ptr app_window::create()
{
    auto builder = Gtk::Builder::create_from_resource("/ui/app_window.xml");
    auto p = Gtk::Builder::get_widget_derived<app_window>(builder, "app_window");
    return app_window_ptr(p);
}
