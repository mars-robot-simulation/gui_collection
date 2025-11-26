#include "CameraWidget.hpp"

#include <mars_interfaces/graphics/GraphicsManagerInterface.h>
#include <mars_interfaces/sim/NodeManagerInterface.h>
#include <mars_interfaces/sim/SensorManagerInterface.h>
#include <mars_core/sensors/CameraSensor.hpp>
#include <mars_utils/misc.h>
#include <mars_graphics/2d_objects/HUDTexture.h>
#include <mars_graphics/2d_objects/HUDLabel.h>
#include <mars_graphics/GraphicsCamera.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <mars_utils/misc.h>

#include <osgDB/WriteFile>


#include <cassert>

namespace mars {

  using namespace utils;
  using namespace interfaces;

  namespace plugins {
    namespace CameraGUI {


      CameraWidget::CameraWidget(interfaces::ControlCenter *control,
                                 main_gui::GuiInterface *gui,
                                 QWidget *parent, const std::string &name) :
        main_gui::BaseWidget(parent, control->cfg, name),
        control(control), gui(gui) {


        QVBoxLayout *vLayout = new QVBoxLayout();
        QHBoxLayout *hLayout = new QHBoxLayout();
        QPushButton *button;

        windowIDs = new QComboBox();
        connect(windowIDs, SIGNAL(currentIndexChanged(int)), this, SLOT(set(int)));

        hLayout->addWidget(windowIDs);

        button = new QPushButton("update list", this);
        connect(button, SIGNAL(clicked()), this, SLOT(update()));
        hLayout->addWidget(button);

        button = new QPushButton("save image", this);
        connect(button, SIGNAL(clicked()), this, SLOT(saveImage()));
        hLayout->addWidget(button);

        vLayout->addLayout(hLayout);

        winID = control->graphics->new3DWindow(0, 0, 1280, 768,
                                               "graph_gui");
        gw = (graphics::GraphicsWidget*)control->graphics->get3DWindow(winID);
        gw->setScene(new osg::Group());

        hudElementStruct he;
        he.type = HUD_ELEMENT_LABEL;
        he.posx = 100;
        he.posy = 100;
        he.width = 190;
        he.height = 100;
        he.background_color[0] = 0.9;
        he.background_color[1] = 0.9;
        he.background_color[2] = 0.9;
        he.background_color[3] = 0.95;
        he.border_color[0] = 1.0;
        he.border_color[1] = 1.0;
        he.border_color[2] = 1.0;
        he.border_color[3] = 0.5;
        he.border_width = 4.0;
        he.font_size = 42;
        he.padding[0] = 5;
        he.padding[1] = 5;
        he.padding[2] = 5;
        he.padding[3] = 5;
        he.direction = 2;
        elem = new graphics::OSGHudElementStruct(he, "", 1);
        if (elem) {
            double color[4] = {1,0,0,1};
            auto l = dynamic_cast<graphics::HUDLabel*>(elem->getHUDElement());
            l->setText("foo ba soo", color);
            gw->addHUDElement(elem->getHUDElement());
        }

        he.type = HUD_ELEMENT_TEXTURE;
        he.posx = 0;
        he.posy = 0;
        he.width = 1920;
        he.height = 1080;
        he.texture_width = 1920;
        he.texture_height = 1080;
        he.background_color[0] = 0.9;
        he.background_color[1] = 0.9;
        he.background_color[2] = 0.9;
        he.background_color[3] = 0.95;
        he.border_color[0] = 1.0;
        he.border_color[1] = 1.0;
        he.border_color[2] = 1.0;
        he.border_color[3] = 0.5;
        he.border_width = 4.0;
        he.font_size = 42;
        he.padding[0] = 5;
        he.padding[1] = 5;
        he.padding[2] = 5;
        he.padding[3] = 5;
        he.direction = 2;
        elem = new graphics::OSGHudElementStruct(he, "", 1);
        if (elem) {
          gw->addHUDElement(elem->getHUDElement());
        }
        vLayout->addWidget((QWidget*)gw->getWidget());

        setLayout(vLayout);
      }

      CameraWidget::~CameraWidget(void) {
        //fprintf(stderr, "Delete CameraWidget\n");
        //gui->removeDockWidget(this, 0);
        control->graphics->remove3DWindow(winID);
        //delete windowIDs;
      }


      void CameraWidget::update() {
        std::vector<interfaces::core_objects_exchange> sensorList;
        std::vector<interfaces::core_objects_exchange>::iterator it;

        windowIDs->clear();
        control->sensors->getListSensors(&sensorList);
        for(it=sensorList.begin(); it!=sensorList.end(); ++it) {
          const interfaces::BaseSensor* bs;
          bs = control->sensors->getFullSensor(it->index);
          const core::CameraSensor *c = dynamic_cast<const core::CameraSensor*>(bs);
          if(c) {
            std::stringstream ss;
            ss << c->getWindowID() << ":" << c->getName() << ":color";
            windowIDs->addItem(ss.str().c_str());
            ss.str("");
            ss << c->getWindowID() << ":" << c->getName() << ":depth";
            windowIDs->addItem(ss.str().c_str());
          }
        }
      }

      void CameraWidget::set(int i) {
        std::string camera = windowIDs->currentText().toStdString();
        fprintf(stderr, "set camera: %s\n", camera.c_str());
        std::vector<std::string> arrString = explodeString(':', camera);
        sscanf(arrString[0].c_str(), "%lu", &currentID);
        graphics::GraphicsWidget *g;
        g = (graphics::GraphicsWidget*)control->graphics->get3DWindow(currentID);
        graphics::HUDTexture *t;
        t = dynamic_cast<graphics::HUDTexture*>(elem->getHUDElement());
        if(arrString.back() == "color") {
          t->setTexture(g->getRTTTexture());
          depthImage = false;
        }
        else if(arrString.back() == "depth") {
          t->setTexture(g->getRTTDepthTexture());
          depthImage = true;
        }
      }

      void CameraWidget::saveImage() {
        QString fileName = QFileDialog::getSaveFileName(NULL, QObject::tr("select save file name"),
                                                        ".", QObject::tr("image files (*.*)"),0,
                                                        QFileDialog::DontUseNativeDialog);
        if(!fileName.isNull()) {
          std::string file = fileName.toStdString();
          graphics::GraphicsWidget *g;
          g = (graphics::GraphicsWidget*)control->graphics->get3DWindow(currentID);
          if(depthImage) {
            osg::Image *image = g->getRTTDepthImage();
            GLuint* data = (GLuint *)image->data();
            int width = image->s();
            int height = image->t();
            double fovy, aspectRatio, Zn, Zf;
            ((graphics::GraphicsCamera*)(g->getCameraInterface()))->getOSGCamera()->getProjectionMatrixAsPerspective( fovy, aspectRatio, Zn, Zf );
            osg::ref_ptr<osg::Image> outImage = new osg::Image;
            outImage->allocateImage(width, height, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV);
            char* data2 = (char *)outImage->data();
            for(int i=height-1; i>=0; --i) {
              for(int k=0; k<width; ++k) {
                GLuint di = data[i*width+k];

                float dv = ((float) di) / std::numeric_limits< GLuint >::max();
                if(dv >= 1.0) dv = 1.0;

                char value = 255*dv;
                // this would scale the image to max distance
                //char value = 255*(Zn*Zf/(Zf-dv*(Zf-Zn)))/Zf;
                data2[4*i*width+4*k] = value;
                data2[4*i*width+4*k+1] = value;
                data2[4*i*width+4*k+2] = value;
                data2[4*i*width+4*k+3] = 255;
              }
            }
            osgDB::writeImageFile(*(outImage.get()), file);
          }
          else {
            osgDB::writeImageFile(*(g->getRTTImage()), file);
          }
        }
      }
    } // end of namespace CameraGUI
  } // end of namespace plugins
} // end of namespace mars
