#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QListWidget>

#include <main_gui/PropertyDialog.h>
#include <main_gui/BaseWidget.h>
#include <data_broker/ReceiverInterface.h>
#include <data_broker/DataInfo.h>
#include <data_broker/DataPackage.h>
#include <cfg_manager/CFGManagerInterface.h>
#include <vector>
#include <set>

namespace mars {

    namespace data_broker {
        class DataBrokerInterface;
    }


    using namespace std;

    namespace data_broker_gui
    {

        class MainDataGui;

        struct DataItemWrapper
        {
            std::string groupName;
            std::string dataName;
            std::string itemName;
        };

        class TreeItem : public QTreeWidgetItem
        {
        public:
            TreeItem(std::string _name=std::string()) :
                QTreeWidgetItem(QStringList(QString::fromStdString(_name))),
                name(_name), wrapper(NULL), fromItem(NULL), outCount(0)
                {
                }
            std::string name;
            std::map<std::string, TreeItem> childs;
            DataItemWrapper *wrapper;
            TreeItem *fromItem;
            int outCount;
        };

        struct PendingConnection
        {
            DataItemWrapper fromWrapper;
            DataItemWrapper toWrapper;
            TreeItem *fromItem;
            TreeItem *toItem;
        };

        class DataConnWidget : public mars::main_gui::BaseWidget,
                               public mars::data_broker::ReceiverInterface
        {
            Q_OBJECT;

        public:
            DataConnWidget(MainDataGui *mainLib, lib_manager::LibManager* libManager,
                           mars::data_broker::DataBrokerInterface *_dataBroker,
                           mars::cfg_manager::CFGManagerInterface *_cfg,
                           QWidget *parent = 0);
            ~DataConnWidget();

            void cleanUp();
            void receiveData(const mars::data_broker::DataInfo &info,
                             const mars::data_broker::DataPackage &data_package,
                             int callbackParam);

        protected:
            void closeEvent(QCloseEvent *event);

        private:
            MainDataGui *mainLib;
            lib_manager::LibManager* libManager;
            mars::data_broker::DataBrokerInterface *dataBroker;
            mars::cfg_manager::CFGManagerInterface *cfg;

            QtProperty *showAllProperty;
            bool showAll;
            QMutex addMutex;
            QMutex listMutex;
            QMutex changeMutex;
            QListWidget *fromListWidget;
            QListWidget *toListWidget;
            QPushButton *connectButton;
            QPushButton *unconnectButton;
            QColor connectColor;

            std::map<std::string, TreeItem> fromItems;
            std::map<std::string, TreeItem> toItems;
            std::vector<TreeItem*> endItems;
            std::vector<PendingConnection> pendingConnections;

            QTreeWidget *fromTreeWidget;
            QTreeWidget *toTreeWidget;

        protected slots:
            void slotConnectDataItems();
            void slotUnconnectDataItems();
            void slotSaveConfiguration(void);
            void addDataPackage(const mars::data_broker::DataInfo &info,
                                const mars::data_broker::DataPackage &dbPackage);
            bool getCfgStringList(std::string configFile, std::string group,
                                  std::string param,
                                  std::vector<std::string> *list, int desSize);
            void makeConnection(TreeItem *fromItem, TreeItem *toItem);
            void removeConnection(TreeItem *toItem);
            TreeItem* getEndTreeItem(const DataItemWrapper &wrapper);
            void checkFromPendingItem(TreeItem *treeItem);
            void checkToPendingItem(TreeItem *treeItem);

            inline bool wrapperEqual(const DataItemWrapper *w1,
                                     const DataItemWrapper *w2) {

                return (w1->groupName == w2->groupName &&
                        w1->dataName == w2->dataName &&
                        w1->itemName == w2->itemName);
            }

            inline std::string makeName(DataItemWrapper w)
                { return w.groupName + "/" + w.dataName + "/" + w.itemName; }
        };

    } // end of namespace: data_broker_gui
} // end of namespace: mars
