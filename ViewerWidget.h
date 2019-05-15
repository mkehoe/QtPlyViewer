#pragma once

#include <QWidget>
#include <QPointer>
#include <QTimer>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QPickEvent>

class ThreeDeeViewerWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ThreeDeeViewerWidget(QWidget *parent = 0);
  void LoadFile(QString file);

protected:
  void resizeEvent(QResizeEvent * event);

public slots:
    void resizeView(QSize size);

private:
  QPointer<Qt3DCore::QEntity> m_rootEntity = nullptr;
  QPointer<Qt3DExtras::Qt3DWindow> m_view = nullptr;
  QPointer<QWidget> m_container = nullptr;
  Qt3DCore::QTransform *m_lightTransform;
  QVector3D m_viewCenter;
  Qt3DRender::QMaterial *m_material = nullptr;
  Qt3DCore::QEntity *m_plyEntity = nullptr;
  QVector3D m_camStartPos;
private slots:
  void  picker_Clicked(Qt3DRender::QPickEvent *pick);
};

