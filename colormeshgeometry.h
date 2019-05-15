#pragma once

#include "Include-Assimp.h"
#include <Qt3DRender\QGeometry>
#include <Qt3DRender\QAttribute>
#include <QVector3D>

class ColorMeshRenderer;


class ColorMeshGeometry : public Qt3DRender::QGeometry
{
  Q_OBJECT
public:
  ColorMeshGeometry(QString meshFile, ColorMeshRenderer *parent);
  ~ColorMeshGeometry();
  void GeometryCenter(QVector3D* center);
private:
  void loadMesh(aiMesh* mesh);
  QVector3D m_sceneCenter;

  //void findObjectDimensions(QMatrix4x4 transformation, QVector3D &minDimension, QVector3D &maxDimension);
};
