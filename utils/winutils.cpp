#include "winutils.h"

#include <QApplication>
#include <Windows.h>
#include <Uxtheme.h>
#pragma comment(lib, "uxtheme")

static LPCSTR lpSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

static PROCESS_POWER_THROTTLING_STATE Throttle{
    PROCESS_POWER_THROTTLING_CURRENT_VERSION,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED};

static PROCESS_POWER_THROTTLING_STATE Unthrottle{
    PROCESS_POWER_THROTTLING_CURRENT_VERSION,
    PROCESS_POWER_THROTTLING_EXECUTION_SPEED,
    NULL};

WinUtils::WinUtils() {}

// Enable or disable startup
void WinUtils::setStartup(const bool &enable)
{
    LPCSTR lpValueName = QApplication::applicationName().toUtf8().data();
    if (enable)
    {
        QByteArray path = QApplication::applicationFilePath().toUtf8();
        QByteArray value = "\"" + path.replace("/", "\\") + "\" -silent";

        LSTATUS lSetRet = RegSetKeyValueA(
            HKEY_CURRENT_USER,
            lpSubKey,
            lpValueName,
            REG_SZ,
            value.data(),
            value.size());

        if (lSetRet == ERROR_SUCCESS)
        {
            qDebug() << "Startup set";
        }
    }
    else
    {
        LSTATUS lDelRet = RegDeleteKeyValueA(
            HKEY_CURRENT_USER,
            lpSubKey,
            lpValueName);

        if (lDelRet == ERROR_SUCCESS)
        {
            qDebug() << "Startup deleted";
        }
    }
}

// Enable or disable EcoQoS
// See https://devblogs.microsoft.com/performance-diagnostics/introducing-ecoqos/
void WinUtils::setThrottle(const bool &enable)
{
    HANDLE hProcess = GetCurrentProcess();
    SetProcessInformation(
        hProcess,
        ProcessPowerThrottling,
        enable ? &Throttle : &Unthrottle,
        sizeof(PROCESS_POWER_THROTTLING_STATE));
    SetPriorityClass(
        hProcess,
        enable ? IDLE_PRIORITY_CLASS : NORMAL_PRIORITY_CLASS);
}

// Enable basic window frame when theme is "Windows"
void WinUtils::setWindowFrame(const HWND &hWnd, const QString &theme)
{
    if (QString::compare(theme, "Windows", Qt::CaseInsensitive) == 0)
    {
        // Turn off visual style
        SetWindowTheme(hWnd, TEXT(" "), TEXT(" "));
    }
    else
    {
        // Enable visual style
        SetWindowTheme(hWnd, TEXT("Explorer"), NULL);
    }
}