#include "MainCfgGui.h"
#include <cstdio>
#include <iostream>

namespace mars
{
    namespace cfg_manager_gui
    {

        using namespace cfg_manager;
        using std::vector;
        using std::string;

        MainCfgGui::MainCfgGui(lib_manager::LibManager* theManager) :
            lib_manager::LibInterface(theManager), gui(NULL),
            cfg(NULL), cfgWidget(NULL)
        {
            set_window_prop = false;
            ignore_next = false;

            // setup GUI with default path
            setupGUI(MARS_PREFERENCES_DEFAULT_RESOURCES_PATH);
        }

        void MainCfgGui::setupGUI(std::string path)
        {

            if(libManager == NULL) return;

            cfg = libManager->getLibraryAs<CFGManagerInterface>("cfg_manager");
            if(cfg)
            {
                cfg->registerToCFG(dynamic_cast<CFGClient*>(this));

                cfgPropertyStruct r_path;
                path = cfg->getOrCreateProperty("Preferences", "resources_path",
                                                "").sValue;
                if(path == "")
                {
                    path = MARS_PREFERENCES_DEFAULT_RESOURCES_PATH;
                }
            } else
                fprintf(stderr, "******* cfg_manager_gui: couldn't find cfg_manager\n");

            gui = libManager->getLibraryAs<main_gui::GuiInterface>("main_gui");
            if (gui == NULL)
                return;

            path.append("/cfg_manager_gui/resources/images/preferences.png");
            gui->addGenericMenuAction("../Options/Configuration", 1,
                                      dynamic_cast<main_gui::MenuInterface*>(this),0,
                                      path, true);

            cfgWidget = new CfgWidget(this, cfg);

            //connect(cfgWidget->pDialog, SIGNAL(geometryChanged()), this, SLOT(geometryChanged()));

            vector<cfgParamInfo> allParams;
            cfg->getAllParams(&allParams);
            for (unsigned int i = 0; i < allParams.size(); i++)
                cfgParamCreated(allParams[i].id);

            if(!cfgWidget->getHiddenCloseState())
            {
                gui->addDockWidget((void*)cfgWidget, 1);
            }
        }

        MainCfgGui::~MainCfgGui()
        {
            if(libManager == NULL) return;

            if(cfg)
            {
                std::list<cfg_manager::cfgParamId>::iterator it;
                for(it = registeredParams.begin(); it != registeredParams.end(); ++it)
                {
                    cfg->unregisterFromParam(*it, this);
                }
                registeredParams.clear();
                cfg->unregisterFromCFG(this);
                libManager->releaseLibrary("cfg_manager");
            }
            if(gui)
            {
                libManager->releaseLibrary("main_gui");
            }
        }

        void MainCfgGui::menuAction(int action, bool checked)
        {
            (void)checked;

            if (gui == NULL || cfgWidget == NULL)
                return;

            switch(action)
            {
            case 1:
                if(cfgWidget->isHidden())
                {
                    gui->addDockWidget((void*)cfgWidget, 1);
                }
                else
                {
                    gui->removeDockWidget((void*)cfgWidget, 1);
                }
                break;
            }
        }

        void MainCfgGui::timerEvent(QTimerEvent *event)
        {
            (void)event;
        }

        void MainCfgGui::cfgUpdateProperty(cfgPropertyStruct _property)
        {
            bool change_view = 0;

            if(!cfgWidget)
                return;

            cfgWidget->changeParam(_property.paramId);
        }


        void MainCfgGui::geometryChanged()
        {
        }

        void MainCfgGui::cfgParamCreated(cfgParamId _id)
        {
            const cfgParamInfo newParam = cfg->getParamInfo(_id);

            if(cfgWidget) cfgWidget->addParam(newParam);
            // register to get param updates
            cfg->registerToParam(_id, dynamic_cast<CFGClient*>(this));
            registeredParams.push_back(_id);
        }

        void MainCfgGui::cfgParamRemoved(cfgParamId _id)
        {
            if(cfgWidget) cfgWidget->removeParam(_id);
            registeredParams.remove(_id);
        }

    } // end of namespace cfg_manager_gui
} // end of namespace mars

DESTROY_LIB(mars::cfg_manager_gui::MainCfgGui);
CREATE_LIB(mars::cfg_manager_gui::MainCfgGui);
