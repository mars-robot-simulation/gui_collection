#pragma once

#include "DataWidget.h"
#include "DataConnWidget.h"
#include <lib_manager/LibInterface.hpp>
#include <cfg_manager/CFGManagerInterface.h>
#include <cfg_manager/CFGClient.h>
#include <main_gui/GuiInterface.h>
#include <main_gui/MenuInterface.h>

#include <string>

#include <QMutex>
#include <QTimerEvent>


namespace mars
{

    namespace data_broker
    {
        class DataBrokerInterface;
    }

    namespace data_broker_gui
    {

        class MainDataGui : public QObject, public lib_manager::LibInterface,
                            public mars::main_gui::MenuInterface
        {
            Q_OBJECT;

        public:
            MainDataGui(lib_manager::LibManager* theManager);
            void setupGUI(std::string path = std::string("."));

            virtual ~MainDataGui(void);

            // MenuInterface methods
            virtual void menuAction(int action, bool checked = false);

            // LibInterface methods
            int getLibVersion() const
                { return 1; }
            const std::string getLibName() const
                { return std::string("data_broker_gui"); }
            CREATE_MODULE_INFO();

            void destroyWindow(QWidget *w);

        private:
            mars::main_gui::GuiInterface* gui;
            mars::cfg_manager::CFGManagerInterface *cfg;
            mars::data_broker::DataBrokerInterface *dataBroker;

            DataWidget* dataWidget;
            DataConnWidget* dataConnWidget;

        protected slots:
            void timerEvent(QTimerEvent* event);

        }; // end of class MainDataGui

    } // end of namespace data_broker_gui

} // end of namespace mars
