#include "CameraGUI.hpp"
#include "CameraWidget.hpp"

namespace mars
{
    namespace plugins
    {
        namespace CameraGUI
        {

            using namespace mars::utils;
            using namespace mars::interfaces;

            CameraGUI::CameraGUI(lib_manager::LibManager *theManager)
                : MarsPluginTemplateGUI(theManager, "CameraGUI")
            {
            }

            void CameraGUI::init()
            {
                gui->addGenericMenuAction("../View/Camera Window", 1, this);
            }

            void CameraGUI::menuAction(int action, bool checked)
            {
                static unsigned long counter = 0;
                std::stringstream ss;
                ss << "CameraWidget_" << ++counter;
                CameraWidget* c = new CameraWidget(control, gui, NULL, ss.str());
                c->setAttribute(Qt::WA_DeleteOnClose, true);
                c->show();
                c->update();
            }

            void CameraGUI::reset()
            {
            }

            CameraGUI::~CameraGUI()
            {
            }

            void CameraGUI::update(sReal time_ms)
            {
            }

        } // end of namespace CameraGUI
    } // end of namespace plugins
} // end of namespace mars

DESTROY_LIB(mars::plugins::CameraGUI::CameraGUI);
CREATE_LIB(mars::plugins::CameraGUI::CameraGUI);
