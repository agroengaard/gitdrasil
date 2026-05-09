#pragma once

#include <QWidget>
#include <QTimer>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class OcctViewerWidget : public QWidget {
    Q_OBJECT
public:
    explicit OcctViewerWidget(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void initOcct();

    Handle(V3d_Viewer)              viewer_;
    Handle(V3d_View)                view_;
    Handle(AIS_InteractiveContext)  context_;
    int                             initTimerId_ = -1;
    int                             renderTimerId_ = -1;
    bool initialized_ = false;
};
