#pragma once

#include <QLabel>
#include <QMutex>
#include <QImage>

namespace mars
{
  namespace plugins
  {
    namespace CameraGUI
    {

      class Canvas : public QLabel
      {

          Q_OBJECT;

      public:
          Canvas();
          ~Canvas();

          QImage image;
          QMutex mutex;

          void paintEvent(QPaintEvent * e);
      };

    } // end of namespace CameraGUI
  } // end of namespace plugins
} // end of namespace mars
