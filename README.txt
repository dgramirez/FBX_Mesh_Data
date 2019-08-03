#FBX_MESH_DATA

This tool was initially created by Derrick G. Ramriez, for a class "Project and Portfolio II". 
The design of this tool was to extract FBX data into a file, so it can be read in without the need to compact the vertices/indices.

The structure of the data, ".FDM" file extension, is as follows:

[Structure of the binary data]
1.) unsigned int index_amount;
2.) unsigned int* indices;
3.) unsigned int vertex_amount;
4.) FDM_Vertex* vertices;
struct FMD_Vertex
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 normal;
	glm::vec4 tangent;
	glm::vec4 binormal;
	glm::vec2 uv;
};

Take note that this uses the GLM math library, located below this line:
https://glm.g-truc.net/0.9.9/index.html
*Note: You don't necessarily need to use GLM if you know how those structs are organized. Any math library will work.

And this uses the FBX SDK, located below this line:
https://www.autodesk.com/products/fbx/overview

Please read the code for any further clarification before contacting me if you intend to use this.
If you have any questions, you can e-mail me at dgramirez@protonmail.com.
