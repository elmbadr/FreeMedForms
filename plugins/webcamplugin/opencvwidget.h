/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developer: Christian A. Reiter <christian.a.reiter@gmail.com>    *
 *   Contributors:                                                         *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef OPENCVWIDGET_H
#define OPENCVWIDGET_H

#include <QLabel>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class QRubberBand;

namespace Webcam {
namespace Internal {

class OpenCVWidget : public QLabel
{
    Q_OBJECT
public:
    enum RubberBandMode {Create, Move};

    OpenCVWidget(QWidget *parent = 0);
    ~OpenCVWidget();

    void toggleFreezeMode();
    bool isFrozen() const {return m_frozen;}

    void setImageUpdateFrequency(const int ms);
    int defaultUpdateFrequency() const;
    
    QRect frame() const;

Q_SIGNALS:
    void frozen(bool);
    void imageReady(bool);
    
private:
    void timerEvent(QTimerEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void restrictRubberBandConstraints();
    
private:
    QImage _image;
    CvCapture *_camera;
    bool m_frozen;
    int _timerId, _updateFreq;
    QRubberBand *m_rubberBand;
    RubberBandMode rbMode;
    QPoint m_clickOrigin;
    QPoint m_rubberOrigin;
};

} // end Internal
} // end Webcam

#endif // OPENCVWIDGET_H