#include "ViewerWidget.h"
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCameraLens>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QRenderSettings>
#include "colormeshrenderer.h"
#include <iostream>

ThreeDeeViewerWidget::ThreeDeeViewerWidget(QWidget *parent)
  : QWidget(parent), m_camStartPos(QVector3D(0.f, -512.f, 500.0f))
{
  m_view = new Qt3DExtras::Qt3DWindow();
  m_container = createWindowContainer(m_view, this);
  m_rootEntity = new Qt3DCore::QEntity();
}

void ThreeDeeViewerWidget::resizeView(QSize size)
{
  m_container->resize(size);
}

void ThreeDeeViewerWidget::resizeEvent ( QResizeEvent * /*event*/ )
{
  resizeView(this->size());
}

void ThreeDeeViewerWidget::LoadFile(QString file)
{

  //Loading .ply data
  ColorMeshRenderer* mesh = new ColorMeshRenderer(file, m_rootEntity);

  m_view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x000000)));

  // Camera
  auto camera = m_view->camera();

  mesh->ViewCenter(&m_viewCenter);

  camera->lens()->setPerspectiveProjection(45.0f, m_view->width()/m_view->height(), 0.01f, 100000.0f);
  camera->setPosition(m_camStartPos);
  camera->setUpVector(QVector3D(0.f, 1.f, 0.f));
  camera->setViewCenter(m_viewCenter);

  auto lightEntity = new Qt3DCore::QEntity(m_rootEntity);

  auto light = new Qt3DRender::QPointLight(lightEntity);
  light->setColor("white");
  lightEntity->addComponent(light);
  m_lightTransform = new Qt3DCore::QTransform(lightEntity);
  m_lightTransform->setTranslation(camera->position());
  lightEntity->addComponent(m_lightTransform);

  m_material = new Qt3DExtras::QPerVertexColorMaterial(m_rootEntity);

  auto rsettings = m_view->renderSettings();
  rsettings->pickingSettings()->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontAndBackFace);
  rsettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::PointPicking);
  rsettings->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
  rsettings->pickingSettings()->setWorldSpaceTolerance(0.5f);

  auto picker = new Qt3DRender::QObjectPicker(m_rootEntity);

  m_plyEntity = new Qt3DCore::QEntity(m_rootEntity);
  m_plyEntity->addComponent(mesh);
  m_plyEntity->addComponent(m_material);
  m_plyEntity->addComponent(picker);

  connect(picker, &Qt3DRender::QObjectPicker::pressed, this, &ThreeDeeViewerWidget::picker_Clicked);

  Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(m_rootEntity);
  camController->setLinearSpeed( 360.0f );
  camController->setLookSpeed( 360.0f );
  camController->setCamera(camera);

  // Set root object of the scene
  m_view->setRootEntity(m_rootEntity);
}

void  ThreeDeeViewerWidget::picker_Clicked(Qt3DRender::QPickEvent *pick)
{
  std::cout << "picker_Clicked" << std::endl;
}
