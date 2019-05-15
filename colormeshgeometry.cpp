#include "colormeshgeometry.h"
#include "colormeshrenderer.h"
#include <Qt3DRender\QBuffer>
#include <Qt3DRender\QAttribute>
#include <QVector3D>

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


void get_bounding_box_for_node (const aiScene* scene, const aiNode* nd,
  aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo) {
  aiMatrix4x4 prev;
  unsigned int n = 0, t;

  prev = *trafo;
  aiMultiplyMatrix4(trafo,&nd->mTransformation);

  for (; n < nd->mNumMeshes; ++n) {
    const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
    for (t = 0; t < mesh->mNumVertices; ++t) {

      aiVector3D tmp = mesh->mVertices[t];
      aiTransformVecByMatrix4(&tmp,trafo);

      min->x = aisgl_min(min->x,tmp.x);
      min->y = aisgl_min(min->y,tmp.y);
      min->z = aisgl_min(min->z,tmp.z);

      max->x = aisgl_max(max->x,tmp.x);
      max->y = aisgl_max(max->y,tmp.y);
      max->z = aisgl_max(max->z,tmp.z);
    }
  }

  for (n = 0; n < nd->mNumChildren; ++n) {
    get_bounding_box_for_node(scene, nd->mChildren[n],min,max,trafo);
  }
  *trafo = prev;
}

/* ---------------------------------------------------------------------------- */
void get_bounding_box (const aiScene* scene, aiVector3D* min, aiVector3D* max)
{
  aiMatrix4x4 trafo;
  aiIdentityMatrix4(&trafo);

  min->x = min->y = min->z =  1e10f;
  max->x = max->y = max->z = -1e10f;
  get_bounding_box_for_node(scene, scene->mRootNode,min,max,&trafo);
}

ColorMeshGeometry::ColorMeshGeometry(QString meshFile, ColorMeshRenderer *parent)
: Qt3DRender::QGeometry(parent)
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile( meshFile.toStdString(),
          aiProcess_GenSmoothNormals      |
          aiProcess_CalcTangentSpace       |
          aiProcess_Triangulate       |
          aiProcess_JoinIdenticalVertices  |
          aiProcess_SortByPType
                                            );
  if( !scene)
  {
      qDebug() << "Error loading file: (assimp:) " << importer.GetErrorString();
      return;
  }

  aiMesh* mesh = nullptr;
  if(scene->HasMeshes())
  {
    for(unsigned int ii=0; ii<scene->mNumMeshes; ++ii)
    {
      mesh = scene->mMeshes[ii];
      break;
    }
  }
  else
  {
    return;
  }
  aiVector3D scene_min, scene_max;

  get_bounding_box(scene, &scene_min, &scene_max);

  m_sceneCenter.setX((scene_min.x + scene_max.x) / 2.0f);
  m_sceneCenter.setY((scene_min.y + scene_max.y) / 2.0f);
  m_sceneCenter.setZ((scene_min.z + scene_max.z) / 2.0f);

  loadMesh(mesh);

  parent->setGeometry(this);
}

void ColorMeshGeometry::GeometryCenter(QVector3D* center)
{
  *center = m_sceneCenter;
}

ColorMeshGeometry::~ColorMeshGeometry()
{
}

void ColorMeshGeometry::loadMesh(aiMesh* mesh)
{
    Qt3DRender::QBuffer *vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
    Qt3DRender::QBuffer *normalDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
    Qt3DRender::QBuffer *colorDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
    Qt3DRender::QBuffer *indexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

    // Vertexbuffer
    QByteArray vertexBufferData;
    QByteArray normalBufferData;
    QByteArray colorBufferData;
    QByteArray indexBufferData;

    vertexBufferData.resize(mesh->mNumVertices * 3 * (int)sizeof(float));
    normalBufferData.resize(mesh->mNumVertices * 3 * (int)sizeof(float));
    colorBufferData.resize(mesh->mNumVertices * 3 * (int)sizeof(float));
    indexBufferData.resize(mesh->mNumFaces * 3 * (int)sizeof(uint));

    float* pCurVertexData = reinterpret_cast<float*>(vertexBufferData.data());
    float* pCurColorData = reinterpret_cast<float*>(colorBufferData.data());
    float* pCurNormalData = reinterpret_cast<float*>(normalBufferData.data());
    uint* pCurIndexData = reinterpret_cast<uint*>(indexBufferData.data());

    for(int i = 0; i < mesh->mNumVertices; i++)
    {
      aiVector3D &vec = mesh->mVertices[i];
      pCurVertexData[i*3+0] = vec.x;
      pCurVertexData[i*3+1] = vec.y;
      pCurVertexData[i*3+2] = vec.z;
      if(mesh->HasVertexColors(0))
      {
        auto col = mesh->mColors[0][i];
        pCurColorData[i*3+0] = col.r;
        pCurColorData[i*3+1] = col.g;
        pCurColorData[i*3+2] = col.b;
      }
      else
      {
        pCurColorData[i*3+0] = 0.5f;
        pCurColorData[i*3+1] = 0.5f;
        pCurColorData[i*3+2] = 0.5f;
      }

      aiVector3D &nvec = mesh->mNormals[i];
      pCurNormalData[i*3+0] = nvec.x;
      pCurNormalData[i*3+1] = nvec.y;
      pCurNormalData[i*3+2] = nvec.z;

    }
    for(uint t = 0; t<mesh->mNumFaces; ++t)
    {
        aiFace* face = &mesh->mFaces[t];
        pCurIndexData[t*3+0] = face->mIndices[0];
        pCurIndexData[t*3+1] = face->mIndices[1];
        pCurIndexData[t*3+2] = face->mIndices[2];
    }

    vertexDataBuffer->setData(vertexBufferData);
    normalDataBuffer->setData(normalBufferData);
    colorDataBuffer->setData(colorBufferData);
    indexDataBuffer->setData(indexBufferData);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(3*mesh->mNumVertices);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());

    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute();
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(normalDataBuffer);
    normalAttribute->setDataType(Qt3DRender::QAttribute::Float);
    normalAttribute->setDataSize(3);
    normalAttribute->setByteOffset(0);
    normalAttribute->setByteStride(3 * sizeof(float));
    normalAttribute->setCount(3*mesh->mNumVertices);
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());

    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(colorDataBuffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(0);
    colorAttribute->setByteStride(3 * sizeof(float));
    colorAttribute->setCount(3*mesh->mNumVertices);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    Qt3DRender::QAttribute *indexAttribute = new Qt3DRender::QAttribute();
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexDataBuffer);
    indexAttribute->setDataType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setDataSize(1);
    indexAttribute->setByteOffset(0);
    indexAttribute->setByteStride(0);
    indexAttribute->setCount(mesh->mNumFaces*3);

    addAttribute(positionAttribute);
    addAttribute(normalAttribute);
    addAttribute(colorAttribute);
    addAttribute(indexAttribute);
}
