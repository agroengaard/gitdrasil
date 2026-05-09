#include "OcctViewerWidget.h"

#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <WNT_Window.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Shape.hxx>
#include <QTimerEvent>
#include <iostream>

OcctViewerWidget::OcctViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMinimumSize(400, 300);
    
    // Start a single-shot timer to init OCCT once widget is shown
    initTimerId_ = startTimer(0);
}

void OcctViewerWidget::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == initTimerId_) {
        killTimer(initTimerId_);
        initTimerId_ = -1;
        initOcct();
    } else if (event->timerId() == renderTimerId_) {
        if (!view_.IsNull()) {
            view_->Redraw();
        }
    }
}

void OcctViewerWidget::initOcct()
{
    if (initialized_)
        return;
    
    // At this point, widget is on screen and winId() is valid
    if (!isVisible() || !winId()) {
        std::cout << "Widget not ready yet, deferring..." << std::endl;
        initTimerId_ = startTimer(100);
        return;
    }

    std::cout << "Initializing OCCT, winId = " << (size_t)winId() << std::endl;
    initialized_ = true;

    Handle(Aspect_DisplayConnection) display = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(display, false);

    viewer_ = new V3d_Viewer(driver);
    viewer_->SetDefaultLights();
    viewer_->SetLightOn();

    context_ = new AIS_InteractiveContext(viewer_);
    view_ = viewer_->CreateView();

    Handle(WNT_Window) window = new WNT_Window((HWND)winId());
    view_->SetWindow(window);
    if (!window->IsMapped())
        window->Map();

    view_->SetBackgroundColor(Quantity_NOC_GRAY20);
    view_->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1);
    view_->SetProj(V3d_XposYnegZpos);

    TopoDS_Shape box = BRepPrimAPI_MakeBox(100, 60, 40).Shape();
    Handle(AIS_Shape) aisBox = new AIS_Shape(box);
    context_->Display(aisBox, AIS_Shaded, 0, false);

    view_->FitAll();
    view_->Redraw();
    
    std::cout << "OCCT initialized successfully" << std::endl;
    
    // Start render timer for continuous redraw
    renderTimerId_ = startTimer(16);  // ~60 fps
}

void OcctViewerWidget::paintEvent(QPaintEvent*)
{
    // Render is handled by the render timer via timerEvent
}

void OcctViewerWidget::resizeEvent(QResizeEvent*)
{
    if (!view_.IsNull()) {
        view_->MustBeResized();
    }
}
