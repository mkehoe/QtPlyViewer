#pragma once

#include <QObject>
#include <Qt3DRender\QGeometryRenderer>
#include "colormeshgeometry.h"


class ColorMeshRenderer : public Qt3DRender::QGeometryRenderer
{
  ColorMeshGeometry* m_geometry = nullptr;
public:
  Q_OBJECT
public:
  explicit ColorMeshRenderer(QString meshFile, Qt3DCore::QNode *parent = 0);
  ~ColorMeshRenderer();
  void ViewCenter(QVector3D* center);
};
