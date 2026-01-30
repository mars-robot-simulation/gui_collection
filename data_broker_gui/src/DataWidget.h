#pragma once

#include <main_gui/PropertyDialog.h>
#include <main_gui/BaseWidget.h>
#include <data_broker/ReceiverInterface.h>
#include <data_broker/DataInfo.h>
#include <data_broker/DataPackage.h>

#include <vector>
#include <set>

#include <QWidget>
#include <QCloseEvent>
#include <QMutex>

namespace mars
{

    using namespace std;

    namespace data_broker
    {
        class DataBrokerInterface;
    }

    namespace data_broker_gui
    {

        class MainDataGui;

        struct paramWrapper
        {
            data_broker::DataInfo info;
            data_broker::DataPackage dataPackage;
            std::vector<QtVariantProperty*> guiElements;
        };

        class DataWidget : public main_gui::BaseWidget,
                           public data_broker::ReceiverInterface,
                           public main_gui::PropertyCallback
        {

            Q_OBJECT;

        public:
            DataWidget(MainDataGui *mainLib, lib_manager::LibManager* libManager,
                       mars::data_broker::DataBrokerInterface *_dataBroker,
                       mars::cfg_manager::CFGManagerInterface *cfg, QWidget *parent = 0);
            ~DataWidget();

            void addParam(const mars::data_broker::DataInfo _info);
            virtual void valueChanged(QtProperty *property, const QVariant &value);

            main_gui::PropertyDialog *pDialog;
            void receiveData(const mars::data_broker::DataInfo &info,
                             const mars::data_broker::DataPackage &data_package,
                             int callbackParam);
            //    void receiveAddProducer(const data_broker::data_info &info) {}
            //    void receiveRemoveProducer(const data_broker::data_info &info) {}
            void cleanUp();

        protected:
            void closeEvent(QCloseEvent *event);

        private:
            MainDataGui *mainLib;
            lib_manager::LibManager* libManager;
            mars::data_broker::DataBrokerInterface *dataBroker;
            QtProperty *showAllProperty;
            bool showAll;
            QMutex addMutex;
            QMutex listMutex;
            QMutex changeMutex;

            set<unsigned long> changeList;
            map<unsigned long, paramWrapper> addList;
            map<unsigned long, paramWrapper> paramList;
            //    map<std::vector<QtVariantProperty*>*, paramWrapper> guiToWrapper;
            map<QtVariantProperty*, paramWrapper*> guiToWrapper;
            bool ignore_change;

        protected slots:
            void timerEvent(QTimerEvent* event);

        };

    } // end of namespace data_broker_gui

} // end of namespace mars

