#include "win32/theme_synchronizer.hpp"

static HKEY parent_registry_key = HKEY_CURRENT_USER;
static wchar_t registry_key_path[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
static wchar_t registry_value_name[] = L"AppsUseLightTheme";

using namespace win32;

theme_synchronizer::theme_synchronizer() :
    quit_event_(CreateEventW(nullptr, TRUE, FALSE, nullptr))
{
    dispatcher_.connect([this] { notify_app_theme_changed(); });
}

theme_synchronizer::~theme_synchronizer()
{
    if (reg_change_mon_thread_.joinable())
    {
        SetEvent(quit_event_);
        reg_change_mon_thread_.join();
    }
    CloseHandle(quit_event_);
}

void theme_synchronizer::start()
{
    if (!reg_change_mon_thread_.joinable())
    {
        notify_app_theme_changed();
        reg_change_mon_thread_ = std::thread{ [this] { run(); } };
    }
}

bool theme_synchronizer::get_apps_use_light_theme()
{
    bool apps_use_light_theme = true;
    DWORD value = 0, value_len = sizeof(value);

    auto result = RegGetValueW(
        parent_registry_key,
        registry_key_path,
        registry_value_name,
        RRF_RT_ANY,
        nullptr,
        &value,
        &value_len);

    if (ERROR_SUCCESS == result)
        apps_use_light_theme = value != 0;

    return apps_use_light_theme;
}

void theme_synchronizer::notify_app_theme_changed()
{
    Gtk::Settings::get_default()->set_property(
        "gtk-application-prefer-dark-theme",
        !get_apps_use_light_theme());
}

void theme_synchronizer::run()
{
    HKEY key_handle = nullptr;
    auto result = RegOpenKeyExW(
        parent_registry_key,
        registry_key_path,
        0,
        KEY_NOTIFY,
        &key_handle);

    if (ERROR_SUCCESS == result)
    {
        auto apps_use_light_theme = get_apps_use_light_theme();
        auto notify_event_handle = CreateEventW(nullptr, TRUE, FALSE, nullptr);

        if (notify_event_handle)
        {
            HANDLE handles[] = { notify_event_handle, quit_event_ };

            for (;;)
            {
                ResetEvent(notify_event_handle);
            
                result = RegNotifyChangeKeyValue(
                    key_handle,
                    FALSE,
                    REG_NOTIFY_CHANGE_LAST_SET,
                    notify_event_handle,
                    TRUE);

                if (ERROR_SUCCESS != result)
                    break;

                auto wait_result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
                auto index = WAIT_OBJECT_0 - wait_result;

                if (0 == index)
                {
                    auto value = get_apps_use_light_theme();
                    if (value != apps_use_light_theme)
                    {
                        dispatcher_.emit();
                        apps_use_light_theme = value;
                    }
                }
                else
                    break;
            }

            CloseHandle(notify_event_handle);
        }

        RegCloseKey(key_handle);
    }
}
