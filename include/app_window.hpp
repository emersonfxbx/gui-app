#ifndef __app_window_hpp__
#define __app_window_hpp__

#include <memory>
#include <gtkmm.h>

class app_window;
using app_window_ptr = std::unique_ptr<app_window>;

class app_window : public Gtk::ApplicationWindow
{
public:
    app_window(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder);

    static app_window_ptr create();
};

#endif
