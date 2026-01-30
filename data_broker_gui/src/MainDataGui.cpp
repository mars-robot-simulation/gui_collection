#include "MainDataGui.h"

#include <data_broker/DataBrokerInterface.h>

#include <cstdio>
#include <iostream>

namespace mars
{

    namespace data_broker_gui
    {

        using namespace cfg_manager;

        MainDataGui::MainDataGui(lib_manager::LibManager *theManager) :
            lib_manager::LibInterface(theManager),
            gui(NULL), cfg(NULL), dataBroker(NULL), dataWidget(NULL)
        {

            // setup GUI with default path
            setupGUI(MARS_DEFAULT_RESOURCES_PATH);
        }

        void MainDataGui::setupGUI(std::string path)
        {

            if(libManager == NULL) return;

            cfg = libManager->getLibraryAs<CFGManagerInterface>("cfg_manager");
            if(cfg)
            {
                path = cfg->getOrCreateProperty("Preferences", "resources_path",
                                                "").sValue;
                if(path == "")
                {
                    path = MARS_DEFAULT_RESOURCES_PATH;
                }
            } else
            {
                fprintf(stderr, "******* data_broker_gui: couldn't find cfg_manager\n");
            }

            gui = libManager->getLibraryAs<main_gui::GuiInterface>("main_gui");
            if (gui == NULL)
                return;

            dataBroker = libManager->getLibraryAs<data_broker::DataBrokerInterface>("data_broker");

            std::string path2 = path;
            path2.append("/data_broker_gui/resources/images/data_broker_symbol.png");
            gui->addGenericMenuAction("../Data/DataBroker", 1, this, 0,
                                      path2, true);

            path2 = path;
            path2.append("/data_broker_gui/resources/images/data_broker_connection_symbol.png");
            gui->addGenericMenuAction("../Data/DataBrokerConnections", 2, this, 0,
                                      path2, true);

            dataWidget = NULL;//new DataWidget(dataBroker);
            dataConnWidget = NULL;
            if(cfg)
            {
                if(!cfg->getOrCreateProperty("Windows", "DataBrokerWidget/hidden",
                                             true).bValue)
                {
                    menuAction(1, false);
                }
                if(!cfg->getOrCreateProperty("Windows",
                                             "DataBroker_ConnectionWidget/hidden",
                                             true).bValue)
                {
                    menuAction(2, false);
                }
            }
        }


        MainDataGui::~MainDataGui()
        {
            fprintf(stderr, "delete data_broker_gui\n");
            if(libManager == NULL) return;
            fprintf(stderr, "have libManager\n");

            if(cfg) libManager->releaseLibrary("cfg_manager");
            if(gui) libManager->releaseLibrary("main_gui");
            if(dataBroker)
            {
                fprintf(stderr, "release data_broker\n");
                libManager->releaseLibrary("data_broker");
            }
            if(dataWidget)
            {
                dataWidget->cleanUp();
            }
            if(dataConnWidget)
            {
                dataConnWidget->cleanUp();
            }
            //fprintf(stderr, "Delete MainDataGui\n");
        }


        void MainDataGui::menuAction(int action, bool checked)
        {
            (void)checked;

            if (gui == NULL)
                return;

            switch(action)
            {
            case 1:
                if(dataWidget == NULL)
                {
                    dataWidget = new DataWidget(this, libManager, dataBroker, cfg);
                    gui->addDockWidget((void*)dataWidget, 0);
                } else
                {
                    gui->removeDockWidget((void*)dataWidget, 0);
                    dataWidget = NULL;
                }
                break;
            case 2:
                if(dataConnWidget == NULL)
                {
                    dataConnWidget = new DataConnWidget(this, libManager, dataBroker, cfg);
                    gui->addDockWidget((void*)dataConnWidget, 0);
                } else
                {
                    gui->removeDockWidget((void*)dataConnWidget, 0);
                    dataConnWidget = NULL;
                }
                break;
            }
        }

        void MainDataGui::timerEvent(QTimerEvent *event)
        {
            (void)event;
        }

        void MainDataGui::destroyWindow(QWidget *w)
        {
            if(w == dataWidget)
            {
                dataWidget->cleanUp();
                dataWidget = NULL;
            }
            else if(w == dataConnWidget)
            {
                dataConnWidget->cleanUp();
                dataConnWidget = NULL;
            }
        }

    } // end of namespace: data_broker_gui

} // end of namespace mars

DESTROY_LIB(mars::data_broker_gui::MainDataGui);
CREATE_LIB(mars::data_broker_gui::MainDataGui);
