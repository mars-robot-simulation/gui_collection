#pragma once

#include "Canvas.hpp"

#include <main_gui/BaseWidget.h>
#include <mars_interfaces/sim/ControlCenter.h>
#include <main_gui/GuiInterface.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

namespace mars
{
    namespace plugins
    {
        namespace CameraGUI
        {

            class CameraWidget : public main_gui::BaseWidget
            {

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
                void timerEvent(QTimerEvent* event);

            private:
                QComboBox *windowIDs;
                Canvas *canvas;
                mars::interfaces::ControlCenter *control;
                mars::main_gui::GuiInterface *gui;
                unsigned long winID;
                int currentID;
                bool depthImage, initImage;
            };

        } // end of namespace CameraGUI
    } // end of namespace plugins
} // end of namespace mars
