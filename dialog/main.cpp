#include <chrono>
#include <string>
#include <thread>
#include <wx/app.h>
#include <wx/wx.h>

#include "dbus.h"

bool showPermissionPopup(wxWindow *guiParent, const std::string &appName, const std::string &permission) {
    std::mutex mtx;
    std::condition_variable cv;
    int userResponse = wxNO; // default

    {
        std::unique_lock<std::mutex> lock(mtx);

        guiParent->CallAfter([guiParent, &userResponse, &cv, &mtx, appName, permission]() {
            std::string message =
                "Would you like to give application \"" + appName + "\" permission to " + permission + "?";

            userResponse = wxMessageBox(message, "Permission Request", wxYES_NO | wxICON_QUESTION, guiParent);

            // Notify the waiting thread
            std::lock_guard<std::mutex> lock2(mtx);
            cv.notify_one();
        });

        // Wait until GUI thread notifies
        cv.wait(lock);
    }

    printf("%d\n", userResponse);

    return userResponse == wxYES;
}

// Simulated background event (e.g., D-Bus permission request)
void backgroundEvent(wxWindow *guiParent) {
    DbusPermissionServer dbus{guiParent};
    dbus.run();
}

class MyApp : public wxApp {
  public:
    bool OnInit() override {
        wxFrame *frame = new wxFrame(nullptr, wxID_ANY, "Permission Manager");
        // Start background thread to trigger popups dynamically
        std::thread(backgroundEvent, frame).detach();

        return true; // Keep GUI running
    }
};

wxIMPLEMENT_APP(MyApp);
