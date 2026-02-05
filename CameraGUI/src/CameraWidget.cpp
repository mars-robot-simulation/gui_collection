#include "CameraWidget.hpp"

#include <mars_interfaces/graphics/GraphicsManagerInterface.hpp>
#include <mars_interfaces/sim/SensorManagerInterface.h>
#include <mars_core/sensors/CameraSensor.hpp>
#include <mars_utils/misc.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>

namespace mars
{

    using namespace utils;
    using namespace interfaces;

    namespace plugins
    {
        namespace CameraGUI
        {


            CameraWidget::CameraWidget(interfaces::ControlCenter *control,
                                       main_gui::GuiInterface *gui,
                                       QWidget *parent, const std::string &name) :
                main_gui::BaseWidget(parent, control->cfg, name),
                control(control), gui(gui)
            {


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

                canvas = new Canvas();
                vLayout->addWidget(canvas);

                currentID = -1;
                initImage = false;
                setLayout(vLayout);
                startTimer(40);
            }

            CameraWidget::~CameraWidget(void)
            {
                //fprintf(stderr, "Delete CameraWidget\n");
                //gui->removeDockWidget(this, 0);
                //control->graphics->remove3DWindow(winID);
                //delete windowIDs;
            }


            void CameraWidget::update()
            {
                std::vector<interfaces::core_objects_exchange> sensorList;
                std::vector<interfaces::core_objects_exchange>::iterator it;

                windowIDs->clear();
                control->sensors->getListSensors(&sensorList);
                for(it=sensorList.begin(); it!=sensorList.end(); ++it)
                {
                    const interfaces::BaseSensor* bs;
                    bs = control->sensors->getFullSensor(it->index);
                    const core::CameraSensor *c = dynamic_cast<const core::CameraSensor*>(bs);
                    if(c)
                    {
                        std::stringstream ss;
                        ss << c->getWindowID() << ":" << c->getName() << ":color";
                        windowIDs->addItem(ss.str().c_str());
                        ss.str("");
                        ss << c->getWindowID() << ":" << c->getName() << ":depth";
                        windowIDs->addItem(ss.str().c_str());
                    }
                }
            }

            void CameraWidget::set(int i)
            {
                std::string camera = windowIDs->currentText().toStdString();
                std::vector<std::string> arrString = explodeString(':', camera);
                sscanf(arrString[0].c_str(), "%d", &currentID);
                int w, h;
                depthImage = false;

                if(arrString.back() == "depth")
                {
                    depthImage = true;
                }
                initImage = true;
            }

            void CameraWidget::saveImage()
            {
                QString fileName = QFileDialog::getSaveFileName(NULL, QObject::tr("select save file name"),
                                                                ".", QObject::tr("image files (*.*)"),0,
                                                                QFileDialog::DontUseNativeDialog);
                if(!fileName.isNull())
                {
                    std::string file = fileName.toStdString();
                    control->graphics->get3DWindow(currentID)->writeRTTImages();
                    // todo: create a qimage and write it to file
                }
            }

            void CameraWidget::timerEvent(QTimerEvent* event)
            {
                if(currentID >= 0)
                {
                    interfaces::GraphicsWindowInterface *g;
                    g = control->graphics->get3DWindow(currentID);
                    if(!depthImage)
                    {
                        void *data;
                        int w, h;
                        g->getImageData(&data, w, h);
                        canvas->mutex.lock();
                        if(initImage || canvas->image.width() != w || canvas->image.height() != h)
                        {
                            canvas->image = QImage(w, h, QImage::Format_RGBA8888);
                            initImage = false;
                        }
                        // the image is flipped
                        char* source = (char*)data;
                        char* target = (char*)canvas->image.bits();
                        for(int i=0; i<h; ++i)
                        {
                            memcpy(target+(h-1-i)*w*4, source+i*w*4, w*4);
                        }
                        canvas->mutex.unlock();
                        free(data);
                    } else
                    {
                        float *data;
                        unsigned short *target;
                        int w, h;
                        g->getRTTDepthData(&data, w, h);
                        canvas->mutex.lock();
                        if(initImage || canvas->image.width() != w || canvas->image.height() != h)
                        {
                            canvas->image = QImage(w, h, QImage::Format_Grayscale16);
                            initImage = false;
                        }
                        target = (unsigned short*)(canvas->image.bits());
                        // convert float32 data to short target
                        float v, max = 0;
                        for(int y=0; y<h; ++y)
                        {
                            for(int x=0; x<w; ++x)
                            {
                                v = data[y*w+x];
                                if(!std::isnan(v) && v>max)
                                {
                                    max = v;
                                }
                            }
                        }
                        for(int y=0; y<h; ++y)
                        {
                            for(int x=0; x<w; ++x)
                            {
                                v = data[y*w+x];
                                if(std::isnan(v))
                                {
                                    v = 100.0;
                                }
                                // linear scale to max value in scene to have a nicer visualization / distinction
                                v = (v/max)*100.0;
                                if(v>100.0) v = 100.0;
                                target[y*w+x] = (unsigned short)(v*0.01*65535);
                                //fprintf(stderr, " %u", target[y*w+x]);
                            }
                        }
                        canvas->mutex.unlock();
                        free(data);
                    }
                    canvas->update();
                }
            }

        } // end of namespace CameraGUI
    } // end of namespace plugins
} // end of namespace mars
