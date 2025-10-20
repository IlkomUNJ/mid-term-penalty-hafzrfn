#include "drawingcanvas.h"

DrawingCanvas::DrawingCanvas(QWidget *parent)  {
    // Set a minimum size for the canvas
    setMinimumSize(this->WINDOW_WIDTH, this->WINDOW_HEIGHT);
    // Set a solid background color
    setStyleSheet("background-color: white; border: 1px solid gray;");
}

void DrawingCanvas::clearPoints(){
    m_points.clear();
    // Trigger a repaint to clear the canvas
    update();
}

void DrawingCanvas::paintLines(){
    /* Todo
     * Implement lines drawing per even pair
    */

    isPaintLinesClicked = true;
    update();
}

void DrawingCanvas::segmentDetection() {
    QPixmap pixmap = this->grab();
    QImage image = pixmap.toImage();

    cout << "Image width: " << image.width() << endl;
    cout << "Image height: " << image.height() << endl;

    m_detectedPoints.clear();  // ✅ clear previous detections

    // Scan through image with 3x3 window
    for (int i = 1; i < image.width() - 1; i++) {
        for (int j = 1; j < image.height() - 1; j++) {
            bool local_window[3][3] = {false};
            bool hasBlack = false;

            // Build the local 3x3 window
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    QRgb rgbValue = image.pixel(i + m, j + n);
                    if (rgbValue != 0xffffffff) {
                        hasBlack = true;
                        local_window[m + 1][n + 1] = true;
                    }
                }
            }

            if (!hasBlack) continue; // Skip empty

            // Store detected window center
            m_detectedPoints.append(QPoint(i, j));

            // Log pattern (for report.md)
            cout << "Non-empty window centered at (" << i << ", " << j << "):" << endl;
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) cout << local_window[a][b] << " ";
                cout << endl;
            }
            cout << "----------------------" << endl;
        }
    }

    cout << "Total detected: " << m_detectedPoints.size() << endl;

    // Trigger repaint → paintEvent() will now draw purple boxes
    update();
}


void DrawingCanvas::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::blue, 5);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::blue));

    for (const QPoint& point : std::as_const(m_points)) {
        painter.drawEllipse(point, 3, 3);
    }

    if (isPaintLinesClicked) {
        pen.setColor(Qt::red);
        pen.setWidth(4);
        painter.setPen(pen);
        for (int i = 0; i < m_points.size() - 1; i += 2) {
            painter.drawLine(m_points[i], m_points[i + 1]);
        }
        isPaintLinesClicked = false;
    }

    // purple symbols for detection
    if (!m_detectedPoints.isEmpty()) {
        QPen detectPen(Qt::magenta);
        detectPen.setWidth(2);
        painter.setPen(detectPen);
        for (const QPoint& p : std::as_const(m_detectedPoints)) {
            painter.drawRect(p.x() - 1, p.y() - 1, 3, 3);
        }
    }
}


void DrawingCanvas::mousePressEvent(QMouseEvent *event) {
    // Add the mouse click position to our vector of points
    m_points.append(event->pos());
    // Trigger a repaint
    update();
}


