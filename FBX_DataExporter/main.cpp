#include "fbx_exporter.h"

void show_error(const char *display_error);

int main(int argc, const char* argv[])
{
	//Error #1: No Arguments or More than 3 Arguments
	if (argc < 2 || argc > 4)
	{
		show_error("Invalid Arguments!");
		return true;
	}

	//Set Scale and Check for Error #2 (Bad Scale)
	float scale = 1.0f;
	if (argc == 3)
	{
		try
		{
			scale = std::stof(argv[2]);
		}
		catch (...)
		{
			show_error("Invalid Scale! Please use float based numbers!");
			return true;
		}
	}

	int mask = 0;
	if (argc == 4)
	{
		try
		{
			mask = std::stol(argv[3]);
			if (mask & ~(3))
			{
				show_error("Invalid UV Mask!\nNormal UV: 0\nInvert Y UV: 1\nInvert X UV: 2\nInvert UV X&Y: 3");
				return true;
			}

		}
		catch (...)
		{
			show_error("Invalid UV Mask!\nNormal UV: 0\nInvert Y UV: 1\nInvert X UV: 2\nInvert UV X&Y: 3");
			return true;
		}
	}

	//Get the show on the road:
	if (import_fbx(argv[1], scale, mask))
		std::cout << "\n\n The Creation of the FBX Mesh Data (fmd) has been successful! ";
	else
		std::cout << "\n\n The Creation of the FBX Mesh Data (fmd) has failed!";

#if _DEBUG
	system("PAUSE");
#endif
	return true;
}

void show_error(const char *display_error)
{
	std::cout << display_error << std::endl << std::endl;
	std::cout << "Format: FBX_DataExporter.exe [FBX Filepath] [Scale] [UV Mask]" << std::endl;
	std::cout << "[FBX Filepath]: The Filepath to the FBX you wish to gather Vertices Data" << std::endl;
	std::cout << "[Scale]: The Scale of the model you wish to modify" << std::endl;
	std::cout << "[UV Mask]: The Mask you wish to invert.\n\t0: UVs are normal.\n\t1: Flip the Y UV (1 - uv.y). Used for Vulkan.\n\t2: Flip the X UV (1 - uv.x).\n\t3: Flip both X and Y UV" << std::endl << std::endl;
}
