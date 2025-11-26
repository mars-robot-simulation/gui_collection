#pragma once

#include <main_gui/BaseWidget.h>
#include <configmaps/ConfigData.h>
#include <config_map_gui/DataWidget.h>
#include <mars_graphics/wrapper/OSGHudElementStruct.h>
#include <mars_interfaces/sim/ControlCenter.h>
#include <mars_graphics/GraphicsWidget.h>
#include <main_gui/GuiInterface.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace mars{
  namespace plugins {
    namespace CameraGUI {

      class CameraWidget : public main_gui::BaseWidget {

        Q_OBJECT;

      public:
        CameraWidget(mars::interfaces::ControlCenter *control,
                     mars::main_gui::GuiInterface *gui,
                     QWidget *parent, const std::string &name);
        ~CameraWidget();


      public slots:
        void update();
        void set(int);
        void saveImage();

      private:
        osg::ref_ptr<graphics::OSGHudElementStruct> elem;
        graphics::GraphicsWidget *gw;
        QComboBox *windowIDs;
        mars::interfaces::ControlCenter *control;
        mars::main_gui::GuiInterface *gui;
        unsigned long winID, currentID;
        bool depthImage;
      };

    } // end of namespace CameraGUI
  } // end of namespace plugins
} // end of namespace mars
