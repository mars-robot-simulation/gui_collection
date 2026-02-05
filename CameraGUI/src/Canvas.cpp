#include "Canvas.hpp"
#include <QPainter>

namespace mars
{
  namespace plugins
  {
    namespace CameraGUI
    {

        Canvas::Canvas()
        {
        }

        Canvas::~Canvas()
        {

        }

        void Canvas::paintEvent(QPaintEvent * e)
        {
            QLabel::paintEvent(e);

            mutex.lock();
            // instantiate a local painter
            QPainter painter(this);

            // draw foreground image over the background using given offset
            painter.drawImage(QPoint(0,0), image); 
            mutex.unlock();
        }

    } // end of namespace CameraGUI
  } // end of namespace plugins
} // end of namespace mars
