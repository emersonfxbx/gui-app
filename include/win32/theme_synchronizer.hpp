#ifndef __theme_synchronizer_hpp__
#define __theme_synchronizer_hpp__

#include <thread>
#include <gtkmm.h>
#include <windows.h>

namespace win32
{
    class theme_synchronizer
    {
    public:
        theme_synchronizer();
        ~theme_synchronizer();

        void start();

    private:
        HANDLE quit_event_;
        std::thread reg_change_mon_thread_;
        Glib::Dispatcher dispatcher_;

        bool get_apps_use_light_theme();
        void notify_app_theme_changed();
        void run();
    };
}

#endif
