#include "app_window.hpp"

class app : public Gtk::Application
{
public:
    app();
    void on_activate() override;

private:
    app_window_ptr app_window_;
};

app::app() :
    Gtk::Application("br.emersonfxbx.gui-app")
{
}

void app::on_activate()
{
    app_window_ = app_window::create();
    add_window(*app_window_);
    app_window_->show();
}

#ifdef _WIN32

#include <cstdlib>
#include <windows.h>
#include "win32/theme_synchronizer.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Glib::set_init_to_users_preferred_locale(false);
    
    app app;

    win32::theme_synchronizer theme_synchronizer;
    theme_synchronizer.start();

    return app.run(__argc, __argv);
}

#else

int main(int argc, char *argv[])
{
    Glib::set_init_to_users_preferred_locale(false);

    app app;
    return app.run(argc, argv);
}

#endif
