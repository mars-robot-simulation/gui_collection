#pragma once

#include <QWidget>
#include <QCloseEvent>
#include <QMutex>

#include <cfg_manager/CFGManagerInterface.h>
#include <main_gui/PropertyDialog.h>
#include <main_gui/BaseWidget.h>
#include <vector>


namespace mars
{
    namespace cfg_manager_gui
    {

        struct paramWrapper
        {
            cfg_manager::cfgParamInfo theParam;
            cfg_manager::cfgPropertyStruct theProp;
            QtVariantProperty *guiElem;
        };

        class MainCfgGui;

        class CfgWidget : public main_gui::BaseWidget,
                          public main_gui::PropertyCallback
        {
            Q_OBJECT

        public:
            CfgWidget(MainCfgGui *mainCfgGui,
                      cfg_manager::CFGManagerInterface *_cfg, QWidget *parent = 0);
            ~CfgWidget();

            void addParam(const cfg_manager::cfgParamInfo &newParam);
            void changeParam(const cfg_manager::cfgParamId _id);
            void removeParam(const cfg_manager::cfgParamId _id);
            virtual void valueChanged(QtProperty *property, const QVariant &value);

            main_gui::PropertyDialog *pDialog;

        private:
            MainCfgGui *mainCfgGui;
            QMutex listMutex;
            QMutex addMutex;
            QMutex changeMutex;
            QMutex removeMutex;

            std::vector<cfg_manager::cfgParamId> removeList;
            std::vector<cfg_manager::cfgParamId> changeList;
            std::vector<cfg_manager::cfgParamInfo> addList;
            std::vector<paramWrapper> paramList;

            bool ignore_change;

            paramWrapper* getWrapperById(cfg_manager::cfgParamId _id);

        protected slots:
            void timerEvent(QTimerEvent *event);
            virtual void accept();
            virtual void reject();

        };

    } // end of namespace cfg_manager_gui
} // end of namespace mars

