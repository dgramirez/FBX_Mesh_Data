#include "fbx_exporter.h"

float scale;
unsigned int mask;

uint32_t isize;
uint32_t* indices;

uint32_t vsize;
Vertex* vertices;

FbxArray<FbxVector4> vert_normals;
FbxArray<FbxVector4> vert_tangents;
FbxArray<FbxVector4> vert_binormals;
FbxArray<FbxVector2> vert_uvs;

const char* filename;
const char* new_filename = nullptr;

bool import_fbx(const char* fbx_filename, const float &_scale, const uint32_t &_mask)
{
	//Set the scale and filename for the object
	scale = _scale;
	filename = fbx_filename;
	mask = _mask;

	// Change the following filename to a suitable filename value.
	const char* lFilename = fbx_filename;

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return false;
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	// Process the scene and build DirectX Arrays
	return process_fbx(lScene->GetRootNode());

}

bool process_fbx(FbxNode* node)
{
	//Get the amount of children the node has
	uint32_t child_count = node->GetChildCount();

	for (uint32_t i = 0; i < child_count; ++i)
	{
		FbxNode *child_node = node->GetChild(i);
		FbxMesh *mesh = child_node->GetMesh();

		if (mesh != nullptr)
		{
			/*-------------------------------------------------REQUIRED-------------------------------------------------
													______                 _              _
													| ___ \               (_)            | |
													| |_/ /___  __ _ _   _ _ _ __ ___  __| |
													|    // _ \/ _` | | | | | '__/ _ \/ _` |
													| |\ \  __/ (_| | |_| | | | |  __/ (_| |
													\_| \_\___|\__, |\__,_|_|_|  \___|\__,_|
																  | |
																  |_|
			-------------------------------------------------REQUIRED-------------------------------------------------*/
			
			//Set success boolean. Of course its false, it didn't do its job yet.
			bool success = false;
			
			//Get Indices
			std::cout << "Obtaining all Indices ";
			success = get_indices(mesh);
			std::cout << check << " (" << isize << ")" << std::endl;
			if (!success) { return false; }

			//Get Vertices
			std::cout << "Obtaining all Vertices ";
			success = get_vertices(mesh);
			std::cout << check << " (" << vsize << ")" << std::endl;
			if (!success) { return false; }

			/*--------------------------------------------------OPTIONAL-------------------------------------------------
												 _____       _   _                   _
												|  _  |     | | (_)                 | |
												| | | |_ __ | |_ _  ___  _ __   __ _| |
												| | | | '_ \| __| |/ _ \| '_ \ / _` | |
												\ \_/ / |_) | |_| | (_) | | | | (_| | |
												 \___/| .__/ \__|_|\___/|_| |_|\__,_|_|
													  | |
													  |_|
			--------------------------------------------------OPTIONAL-------------------------------------------------*/

			//Get Normals
			std::cout << "Obtaining All Normals ";
			success = get_normals(mesh);
			std::cout << check << std::endl;
			if (!success) { check = "[CHECK]"; }

			//Get Tangents
			std::cout << "Obtaining All Tangents ";
			success = get_tangents(mesh);
			std::cout << check << std::endl;
			if (!success) { check = "[CHECK]"; }

			//Get BiTangents
			std::cout << "Obtaining All Normals ";
			success = get_binormals(mesh);
			std::cout << check << std::endl;
			if (!success) { check = "[CHECK]"; }

			//Get UVs
			std::cout << "Obtaining All UVs ";
			success = get_uvs(mesh);
			std::cout << check << std::endl;
			if (!success) { check = "[CHECK]"; }

			/*------------------------------------------------FINALIZING------------------------------------------------
												______ _             _ _     _
												|  ___(_)           | (_)   (_)
												| |_   _ _ __   __ _| |_ _____ _ __   __ _
												|  _| | | '_ \ / _` | | |_  / | '_ \ / _` |
												| |   | | | | | (_| | | |/ /| | | | | (_| |
												\_|   |_|_| |_|\__,_|_|_/___|_|_| |_|\__, |
																					  __/ |
																					 |___/
			------------------------------------------------FINALIZING------------------------------------------------*/
			
			//Expand Vertices
			std::cout << "Expanding Vertices ";
			success = expand_vertices();
			std::cout << check << std::endl;
			if (!success) { return false; }

			//Compact Vertices
			std::cout << "Compacting Vertices ";
			success = compact_vertices();
			std::cout << check << std::endl;
			if (!success) { return false; }

			//Inverting UVs
			if (mask & 3)
			{
				std::cout << "Inverting UVs";
				invert_uvs(mask);
				std::cout << check << std::endl;
			}

			//Vertex Color
			std::cout << "Setting Vertex Colors";
			set_vertices_color();
			std::cout << check << std::endl;

			//Export to *.fmd
			std::cout << "Exporting to .fmd";
			success = export_fmd();
			std::cout << check << std::endl;
			if (!success) { return false; }

			//Re-Import *.fmd
			std::cout << std::endl << "Re-Importing .fmd file";
			success = import_fmd();
			std::cout << check << std::endl;
		}

		process_fbx(child_node);
	}

	return true;
}

bool get_indices(FbxMesh *mesh)
{
	isize = mesh->GetPolygonVertexCount();

	int* polygon_indices = mesh->GetPolygonVertices();
	indices = new uint32_t[isize];
	
	for (uint32_t i = 0; i < isize; ++i)
	{
		try
		{
			indices[i] = polygon_indices[i];
		}
		catch (...)
		{
			check = "[FAILED!] FATAL ERROR! WILL EXIT PROGRAM";
			return false;
		}
	}

	return true;
}

bool get_vertices(FbxMesh *mesh)
{
	vsize = mesh->GetControlPointsCount();
	vertices = new Vertex[vsize];
	FbxVector4 vert = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (uint32_t i = 0; i < (uint32_t)vsize; ++i)
	{
		try
		{
			vert = mesh->GetControlPointAt(i);
			vertices[i].position = { vert.mData[0] * scale, vert.mData[1] * scale, vert.mData[2] * scale, vert.mData[3] * scale };
		}
		catch (...)
		{
			check = "[FAILED!] FATAL ERROR! WILL EXIT PROGRAM";
			return false;
		}
	}

	return true;
}

bool get_normals(FbxMesh *mesh)
{
	//Get the Normal Count. Fail if count is 0
	uint32_t normal_count = mesh->GetElementNormalCount();
	if (normal_count < 1)
	{
		check = "[FAILED!] Normal Count Invalid! Setting Normals to 0";
		set_normal_default();
		return false;
	}

	//Get Geometry Element Normal
	FbxGeometryElementNormal* vertex_normal = mesh->GetElementNormal(0);
	
	//Check Map Mode. Fail if not Control Point or Polygon Vertex	
	auto map_mode = vertex_normal->GetMappingMode();
	if (map_mode != CONTROL_POINT && map_mode != POLYGON_VERTEX)
	{
		check = "[FAILED!] Map Mode is Invalid! Setting Normals to 0";
		set_normal_default();
		return false;
	}

	//Check Reference Mode. Fail if not Direct or Index to Direct.
	auto ref_mode = vertex_normal->GetReferenceMode();
	if (ref_mode != DIRECT && ref_mode != INDEX_TO_DIRECT)
	{
		check = "[FAILED!] Reference Mode is Invalid! Setting Normals to 0";
		set_normal_default();
		return false;
	}
	const bool use_index = (ref_mode != DIRECT);
	const uint32_t index_count = (use_index) ? vertex_normal->GetIndexArray().GetCount() : 0;

	//Get the size and start the loop based on map mode
	uint32_t poly_count = mesh->GetPolygonCount();
	switch (map_mode)
	{
	case CONTROL_POINT:
		check = "[Failed!] Control Point Not Supported At the moment Setting Normals to 0";
		set_normal_default();
		return false;
		break;
	case POLYGON_VERTEX:
		uint32_t poly_index_counter = 0;
		for (uint32_t poly_i = 0; poly_i < poly_count; ++poly_i)
		{
			const int poly_size = mesh->GetPolygonSize(poly_i);
			for (uint32_t vert_i = 0; vert_i < (uint32_t)poly_size; ++vert_i)
			{
				//Get the true index depending on the reference mode
				uint32_t true_index = (use_index) ? vertex_normal->GetIndexArray().GetAt(poly_index_counter) : poly_index_counter;
				vert_normals.Add(vertex_normal->GetDirectArray().GetAt(true_index));
				++poly_index_counter;
			}
		}
		break;
	}


	return true;
}

bool get_tangents(FbxMesh *mesh)
{
	//Get the Tangent Count. Fail if count is 0
	uint32_t tangent_count = mesh->GetElementTangentCount();
	if (tangent_count < 1)
	{
		check = "[FAILED!] Tangent Count Invalid! Setting Tangents to 0";
		set_tangent_default();
		return false;
	}

	//Get Geometry Element Tangent
	FbxGeometryElementTangent* vertex_tangent = mesh->GetElementTangent(0);

	//Check Map Mode. Fail if not Control Point or Polygon Vertex	
	auto map_mode = vertex_tangent->GetMappingMode();
	if (map_mode != CONTROL_POINT && map_mode != POLYGON_VERTEX)
	{
		check = "[FAILED!] Map Mode is Invalid! Setting Tangents to 0";
		set_tangent_default();
		return false;
	}

	//Check Reference Mode. Fail if not Direct or Index to Direct.
	auto ref_mode = vertex_tangent->GetReferenceMode();
	if (ref_mode != DIRECT && ref_mode != INDEX_TO_DIRECT)
	{
		check = "[FAILED!] Reference Mode is Invalid! Setting Tangents to 0";
		set_tangent_default();
		return false;
	}
	const bool use_index = (ref_mode != DIRECT);
	const uint32_t index_count = (use_index) ? vertex_tangent->GetIndexArray().GetCount() : 0;

	//Get the size and start the loop based on map mode
	uint32_t poly_count = mesh->GetPolygonCount();
	switch (map_mode)
	{
	case CONTROL_POINT:
		check = "[Failed!] Control Point Not Supported At the moment Setting Tangents to 0";
		set_tangent_default();
		return false;
		break;
	case POLYGON_VERTEX:
		uint32_t poly_index_counter = 0;
		for (uint32_t poly_i = 0; poly_i < poly_count; ++poly_i)
		{
			const int poly_size = mesh->GetPolygonSize(poly_i);
			for (uint32_t vert_i = 0; vert_i < (uint32_t)poly_size; ++vert_i)
			{
				//Get the true index depending on the reference mode
				uint32_t true_index = (use_index) ? vertex_tangent->GetIndexArray().GetAt(poly_index_counter) : poly_index_counter;
				vert_tangents.Add(vertex_tangent->GetDirectArray().GetAt(true_index));
				++poly_index_counter;
			}
		}
		break;
	}

	return true;
}

bool get_binormals(FbxMesh *mesh)
{
	//Get the Tangent Count. Fail if count is 0
	uint32_t binormal_count = mesh->GetElementBinormalCount();
	if (binormal_count < 1)
	{
		check = "[FAILED!] Binormal Count Invalid! Setting Tangents to 0";
		set_binormals_default();
		return false;
	}

	//Get Geometry Element Tangent
	FbxGeometryElementBinormal* vertex_binormal = mesh->GetElementBinormal(0);

	//Check Map Mode. Fail if not Control Point or Polygon Vertex	
	auto map_mode = vertex_binormal->GetMappingMode();
	if (map_mode != CONTROL_POINT && map_mode != POLYGON_VERTEX)
	{
		check = "[FAILED!] Map Mode is Invalid! Setting Binormals to 0";
		set_binormals_default();
		return false;
	}

	//Check Reference Mode. Fail if not Direct or Index to Direct.
	auto ref_mode = vertex_binormal->GetReferenceMode();
	if (ref_mode != DIRECT && ref_mode != INDEX_TO_DIRECT)
	{
		check = "[FAILED!] Reference Mode is Invalid! Setting Binormals to 0";
		set_binormals_default();
		return false;
	}
	const bool use_index = (ref_mode != DIRECT);
	const uint32_t index_count = (use_index) ? vertex_binormal->GetIndexArray().GetCount() : 0;

	//Get the size and start the loop based on map mode
	uint32_t poly_count = mesh->GetPolygonCount();
	switch (map_mode)
	{
	case CONTROL_POINT:
		check = "[Failed!] Control Point Not Supported At the moment Setting Binormals to 0";
		set_binormals_default();
		return false;
		break;
	case POLYGON_VERTEX:
		uint32_t poly_index_counter = 0;
		for (uint32_t poly_i = 0; poly_i < poly_count; ++poly_i)
		{
			const int poly_size = mesh->GetPolygonSize(poly_i);
			for (uint32_t vert_i = 0; vert_i < (uint32_t)poly_size; ++vert_i)
			{
				//Get the true index depending on the reference mode
				uint32_t true_index = (use_index) ? vertex_binormal->GetIndexArray().GetAt(poly_index_counter) : poly_index_counter;
				vert_binormals.Add(vertex_binormal->GetDirectArray().GetAt(true_index));
				++poly_index_counter;
			}
		}
		break;
	}

	return true;
}

bool get_uvs(FbxMesh *mesh)
{
	uint32_t uv_count = mesh->GetElementUVCount();
	if (uv_count < 1)
	{
		check = "[FAILED!] Normal Count Invalid! Setting Normals to 0";
		set_uv_default();
		return false;
	}

	FbxGeometryElementUV* vertex_uv = mesh->GetElementUV(0);
	auto map_mode = vertex_uv->GetMappingMode();
	if (map_mode != CONTROL_POINT && map_mode != POLYGON_VERTEX)
	{
		check = "[FAILED!] Map Mode is Invalid! Setting Normals to 0";
		set_uv_default();
		return false;
	}

	auto ref_mode = vertex_uv->GetReferenceMode();
	if (ref_mode != DIRECT && ref_mode != INDEX_TO_DIRECT)
	{
		check = "[FAILED!] Reference Mode is Invalid! Setting Normals to 0";
		set_uv_default();
		return false;
	}
	const bool use_index = ref_mode != DIRECT;
	const uint32_t index_count = (use_index) ? vertex_uv->GetIndexArray().GetCount() : 0;

	uint32_t poly_count = mesh->GetPolygonCount();
	switch (map_mode)
	{
	case CONTROL_POINT:
		check = "[Failed!] Control Point Not Supported At the moment Setting Normals to 0";
		set_uv_default();
		return false;
		break;
	case POLYGON_VERTEX:
		uint32_t poly_index_counter = 0;
		for (uint32_t poly_i = 0; poly_i < poly_count; ++poly_i)
		{
			const int poly_size = mesh->GetPolygonSize(poly_i);
			for (uint32_t vert_i = 0; vert_i < (uint32_t)poly_size; ++vert_i)
			{
				if (poly_index_counter < (uint32_t)index_count)
				{
					uint32_t true_index = (use_index) ? vertex_uv->GetIndexArray().GetAt(poly_index_counter) : poly_index_counter;
					vert_uvs.Add(vertex_uv->GetDirectArray().GetAt(true_index));
					++poly_index_counter;
				}
			}
		}
		break;
	}


	return true;
}

bool expand_vertices()
{
	//Expand the Vertex to match the index size
	Vertex *tmp = new Vertex[isize];
	try
	{
		for (uint32_t i = 0; i < isize; ++i)
		{
			tmp[i].position =	vertices[indices[i]].position;
			tmp[i].normal =		{ vert_normals[i].mData[0], vert_normals[i].mData[1], vert_normals[i].mData[2], vert_normals[i].mData[3] };
			tmp[i].tangent =	{ vert_tangents[i].mData[0], vert_tangents[i].mData[1], vert_tangents[i].mData[2], vert_tangents[i].mData[3] };
			tmp[i].binormal =	{ vert_binormals[i].mData[0], vert_binormals[i].mData[1], vert_binormals[i].mData[2], vert_binormals[i].mData[3] };
			tmp[i].uv =			{ vert_uvs[i].mData[0], vert_uvs[i].mData[1] };
		}
	}
	catch (...)
	{
		check = "[FAILED!] FATAL! ENDING PROGRAM!";
		return false;
	}
	vsize = isize;
	delete[] vertices;
	vertices = tmp;

	//Set the Indices to match the vertices[THIS IS NOT NEEDED! FOR EDUCATION PURPOSES ITS HERE]
	for (uint32_t i = 0; i < isize; ++i)
		indices[i] = i;

	return true;
}

bool compact_vertices()
{
	//Create temporary variables
	int* tmp_idx = new int[isize];
	Vertex* tmp_vtx = new Vertex[isize];

	//Setup the compactify
	const float epsilon = 0.00001f;
	uint32_t indices_count = isize;
	isize = 0;
	vsize = 0;

	try
	{
		//Loop through the whole list and compact it.
		for (uint32_t i = 0; i < indices_count; ++i)
		{
			for (uint32_t j = 0; j <= vsize; ++j)
			{
				if (j == vsize)
				{
					tmp_idx[isize] = vsize;
					++isize;
					tmp_vtx[vsize] = vertices[i];
					++vsize;
					break;
				}
				else if 
				(
					(abs(vertices[i].position.x - tmp_vtx[j].position.x) < epsilon) &&
					(abs(vertices[i].position.y - tmp_vtx[j].position.y) < epsilon) &&
					(abs(vertices[i].position.z - tmp_vtx[j].position.z) < epsilon) &&
					(abs(vertices[i].position.w - tmp_vtx[j].position.w) < epsilon) &&

					(abs(vertices[i].normal.x - tmp_vtx[j].normal.x) < epsilon) &&
					(abs(vertices[i].normal.y - tmp_vtx[j].normal.y) < epsilon) &&
					(abs(vertices[i].normal.z - tmp_vtx[j].normal.z) < epsilon) &&
					(abs(vertices[i].normal.w - tmp_vtx[j].normal.w) < epsilon) &&

					(abs(vertices[i].tangent.x - tmp_vtx[j].tangent.x) < epsilon) &&
					(abs(vertices[i].tangent.y - tmp_vtx[j].tangent.y) < epsilon) &&
					(abs(vertices[i].tangent.z - tmp_vtx[j].tangent.z) < epsilon) &&
					(abs(vertices[i].tangent.w - tmp_vtx[j].tangent.w) < epsilon) &&


					(abs(vertices[i].binormal.x - tmp_vtx[j].binormal.x) < epsilon) &&
					(abs(vertices[i].binormal.y - tmp_vtx[j].binormal.y) < epsilon) &&
					(abs(vertices[i].binormal.z - tmp_vtx[j].binormal.z) < epsilon) &&
					(abs(vertices[i].binormal.w - tmp_vtx[j].binormal.w) < epsilon) &&

					(abs(vertices[i].uv.x - tmp_vtx[j].uv.x) < epsilon) &&
					(abs(vertices[i].uv.y - tmp_vtx[j].uv.y) < epsilon)

				)
				{
					tmp_idx[isize] = j;
					++isize;
					break;
				}
			}
		}
	}
	catch (...)
	{
		check = "[FAILED] PROGRAM ERROR! WILL SHUTDOWN!";
	}

	delete[] vertices;

	vertices = new Vertex[vsize];

	for (uint32_t i = 0; i < vsize; ++i)
		vertices[i] = tmp_vtx[i];

	for (uint32_t i = 0; i < isize; ++i)
		indices[i] = tmp_idx[i];

	delete[] tmp_idx;
	delete[] tmp_vtx;

	return true;
}

void invert_uvs(unsigned int mask)
{
	if ( (mask & 3) == 3) {
		for (uint32_t i = 0; i < vsize; ++i)
		{
			vertices[i].uv.x = 1 - vertices[i].uv.x;
			vertices[i].uv.y = 1 - vertices[i].uv.y;
		}
	}
	else if (mask & 2) {
		for (uint32_t i = 0; i < vsize; ++i)
			vertices[i].uv.x = 1 - vertices[i].uv.x;
	}
	else {
		for (uint32_t i = 0; i < vsize; ++i)
			vertices[i].uv.y = 1 - vertices[i].uv.y;
	}
}

void set_vertices_color()
{
	//This is only for fun. Color can totally be taken out if wanted.
	for (uint32_t i = 0; i < vsize; ++i)
	{
		float red = fmodf(0.1f * (float)i, scale) / scale;
		float green = (float)i / (float)isize;
		float blue = (float)(i % vsize) / vsize;
		vertices[i].color = { red, green, blue, 1.0f };
	}
}

void set_normal_default()
{
	for (uint32_t i = 0; i < isize; ++i)
		vert_normals.Add(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void set_tangent_default()
{
	for (uint32_t i = 0; i < isize; ++i)
		vert_tangents.Add(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void set_binormals_default()
{
	for (uint32_t i = 0; i < isize; ++i)
		vert_binormals.Add(FbxVector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void set_uv_default()
{
	for (uint32_t i = 0; i < isize; ++i)
		vert_uvs.Add(FbxVector2(0.0f, 0.0f));
}

bool export_fmd()
{
	uint32_t len = (uint32_t)strlen(filename) + 1;
	char* fname = new char[len];
	memcpy(fname, filename, len);
	fname[len - 2] = 'd';
	fname[len - 3] = 'm';
	fname[len - 4] = 'f';

	std::ofstream file;
	file.open(fname, std::ios::binary);

	if (file.is_open())
	{
		uint32_t bytesize = 0;
		uint32_t offset = 0;

		//Write the number of indices
		offset += bytesize;
		file.seekp(offset);
		bytesize = sizeof(uint32_t);
		file.write((const char*)&isize, bytesize);

		//Write the indices array
		offset += bytesize;
		file.seekp(offset);
		bytesize = sizeof(uint32_t) * isize;
		file.write((const char*)&indices[0], bytesize);

		//Write the number of vertices
		offset += bytesize;
		file.seekp(offset);
		bytesize = sizeof(uint32_t);
		file.write((const char*)&vsize, bytesize);

		//Write the vertices array
		offset += bytesize;
		file.seekp(offset);
		bytesize = sizeof(Vertex) * vsize;
		file.write((const char*)&vertices[0], bytesize);

		//Close the file and set the new filename to the current filename (for import testing)
		file.close();
		new_filename = fname;
	}
	else
	{
		std::cout << "[FAILED!] Failed to open " << fname << ", Are you sure it is not in use?";
		check = "";
		return false;
	}

	return true;
}

bool import_fmd()
{
	std::ifstream file;
	file.open(new_filename, std::ios::binary);

	int test_isize = 0;
	uint32_t* test_indices = nullptr;

	int test_vsize = 0;
	Vertex* test_vertices = nullptr;

	if (file.is_open())
	{
		uint32_t bytesize = 0;
		uint32_t offset = 0;

		//Read the size of the indices
		offset += bytesize;
		file.seekg(offset);
		bytesize = sizeof(uint32_t);
		file.read((char*)&test_isize, bytesize);

		//Read the Indices Array
		offset += bytesize;
		file.seekg(offset);
		bytesize = sizeof(uint32_t) * test_isize;
		test_indices = new uint32_t[test_isize];
		file.read((char*)&test_indices[0], bytesize);

		//Read the size of the vertices
		offset += bytesize;
		file.seekg(offset);
		bytesize = sizeof(uint32_t);
		file.read((char*)&test_vsize, bytesize);

		//Read the Vertices Array
		offset += bytesize;
		file.seekg(offset);
		bytesize = test_vsize * sizeof(Vertex);
		test_vertices = new Vertex[test_vsize];
		file.read((char*)&test_vertices[0], bytesize);

		file.close();
	}
	else
	{
		return false;
	}

	//Show off some data
	std::cout << "\n\n" << "Indices Size: " << test_isize << " (" << isize << ")" << std::endl;
	std::cout << "Vertex Size: " << test_vsize << " (" << vsize << ")" << std::endl << std::endl;

	uint32_t size = test_vsize < 5 ? test_vsize : 5;
	for (uint32_t i = 0; i < size; ++i)
	{
		uint32_t idx = vsize - i - 1;
		Vertex* v = &test_vertices[idx];
		std::cout << "Index: " << idx << std::endl;
		std::cout << "Test pos: " << "(" << v->position.x << ", " << v->position.y << ", " << v->position.z << ", " << v->position.w << ")";
		std::cout << "Real pos: " << "(" << vertices[idx].position.x << ", " << vertices[idx].position.y << ", " << vertices[idx].position.z << ", " << vertices[idx].position.w << ")" << std::endl;
		
		std::cout << "Test nrm: " << "(" << v->normal.x << ", " << v->normal.y << ", " << v->normal.z << ", " << v->normal.w << ")";
		std::cout << "Real nrm: " << "(" << vertices[idx].normal.x << ", " << vertices[idx].normal.y << ", " << vertices[idx].normal.z << ", " << vertices[idx].normal.w << ")" << std::endl;
		
		std::cout << "Test tan: " << "(" << v->tangent.x << ", " << v->tangent.y << ", " << v->tangent.z << ", " << v->tangent.w << ")";
		std::cout << "Real tan: " << "(" << vertices[idx].tangent.x << ", " << vertices[idx].tangent.y << ", " << vertices[idx].tangent.z << ", " << vertices[idx].tangent.w << ")" << std::endl;
		
		std::cout << "Test bin: " << "(" << v->binormal.x << ", " << v->binormal.y << ", " << v->binormal.z << ", " << v->binormal.w << ")";
		std::cout << "Real bin: " << "(" << vertices[idx].binormal.x << ", " << vertices[idx].binormal.y << ", " << vertices[idx].binormal.z << ", " << vertices[idx].binormal.w << ")" << std::endl;
		
		std::cout << "Test clr: " << "(" << v->color.x << ", " << v->color.y << ", " << v->color.z << ", " << v->color.w << ")";
		std::cout << "Real clr: " << "(" << vertices[idx].color.x << ", " << vertices[idx].color.y << ", " << vertices[idx].color.z << ", " << vertices[idx].color.w << ")" << std::endl;
		
		std::cout << "Test UV: " << "(" << v->uv.x << ", " << v->uv.y << ")";
		std::cout << "Real UV: " << "(" << vertices[idx].uv.x << ", " << vertices[idx].uv.y << ")" << std::endl;
	}

	//Delete All Data Created here
	if (new_filename)
		delete[] new_filename;
	if (test_vertices)
		delete[] vertices;
	if (test_indices)
		delete[] indices;

	return true;
}
