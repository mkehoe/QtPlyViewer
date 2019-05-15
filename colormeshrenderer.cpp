#include "colormeshrenderer.h"
#include "colormeshgeometry.h"

ColorMeshRenderer::ColorMeshRenderer(QString meshFile, Qt3DCore::QNode *parent)
: Qt3DRender::QGeometryRenderer(parent)
{
  m_geometry = new ColorMeshGeometry(meshFile, this);
  setGeometry(m_geometry);
}


ColorMeshRenderer::~ColorMeshRenderer()
{
}

void ColorMeshRenderer::ViewCenter(QVector3D* center)
{
  m_geometry->GeometryCenter(center);
}

