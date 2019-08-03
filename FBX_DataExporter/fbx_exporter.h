#ifndef FBX_EXPORTER_H
#define FBX_EXPORTER_H
#include "fbxsdk.h"
#include "glm.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec4 binormal;
	glm::vec2 uv;
};

static const char* check = "[CHECK]";
static const uint32_t CONTROL_POINT = (int)FbxGeometryElement::eByControlPoint;
static const uint32_t POLYGON_VERTEX = (int)FbxGeometryElement::eByPolygonVertex;
static const uint32_t DIRECT = (int)FbxGeometryElement::eDirect;
static const uint32_t INDEX_TO_DIRECT = (int)FbxGeometryElement::eIndexToDirect;

//Import FBX Data
bool import_fbx(const char* fbx_filename, const float &_scale, const uint32_t &_mask);
bool process_fbx(FbxNode* node);

//Get Data Step by Step
bool get_indices(FbxMesh *mesh);
bool get_vertices(FbxMesh *mesh);
bool get_normals(FbxMesh *mesh);
bool get_tangents(FbxMesh *mesh);
bool get_binormals(FbxMesh *mesh);
bool get_uvs(FbxMesh *mesh);

//Operate on the Vertices
bool expand_vertices();
bool compact_vertices();

void invert_uvs(unsigned int mask);
void set_vertices_color();

//Failure Options
void set_normal_default();
void set_tangent_default();
void set_binormals_default();
void set_uv_default();

//Export Data
bool export_fmd();		//Export FBX Mesh Data (Binary)
bool import_fmd();		//Import FBX Mesh Data (Binary)
#endif