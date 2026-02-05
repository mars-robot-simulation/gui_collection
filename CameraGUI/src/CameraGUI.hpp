#pragma once

// set define if you want to extend the gui
//#define PLUGIN_WITH_MARS_GUI
#include <mars_interfaces/sim/MarsPluginTemplateGUI.h>
#include <mars_interfaces/MARSDefs.h>

#include <string>

namespace mars {
  namespace plugins {
    namespace CameraGUI {

      // inherit from MarsPluginTemplateGUI for extending the gui
      class CameraGUI: public mars::interfaces::MarsPluginTemplateGUI,
        // for gui
        public mars::main_gui::MenuInterface {

      public:
        CameraGUI(lib_manager::LibManager *theManager);
        ~CameraGUI();

        // LibInterface methods
        int getLibVersion() const
        { return 1; }
        const std::string getLibName() const
        { return std::string("CameraGUI"); }
        CREATE_MODULE_INFO();

        // MarsPlugin methods
        void init();
        void reset();
        void update(mars::interfaces::sReal time_ms);

        // MenuInterface methods
        void menuAction(int action, bool checked = false);

        // CameraGUI methods

      }; // end of class definition CameraGUI

    } // end of namespace CameraGUI
  } // end of namespace plugins
} // end of namespace mars

