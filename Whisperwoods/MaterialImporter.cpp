#include "Core.h"
#include "MaterialImporter.h"

#include "Resources.h"

#include <fstream>
#include <sstream>

bool MaterialImporter::ImportWWMT(string filepath, MaterialResource* const outMaterial)
{
	// Resetting the material

	*outMaterial = MaterialResource{};

	uint64 lastSlash = std::min(filepath.find_last_of('/'), filepath.find_last_of('\\'));
	uint64 period = filepath.find_last_of('.');

	if (lastSlash == std::string::npos || period == std::string::npos || lastSlash > period)
	{
		LOG_WARN("Failed to parse resource name at: %s", filepath.c_str());
		outMaterial->name = filepath;
	}
	else
	{
		outMaterial->name = filepath.substr(lastSlash + 1, period - lastSlash - 1);
	}



	// Use the construction-initialized defaults instead

	//outMaterial->diffuse = Vec3(1.0f, 1.0f, 1.0f);
	//outMaterial->specular = Vec3(1.0f, 1.0f, 1.0f);
	//outMaterial->emissive = Vec3(1.0f, 1.0f, 1.0f);
	//outMaterial->glossiness = 10.0f;
	//outMaterial->alpha = 1.0f;
	//outMaterial->height = 0.0f;

	//outMaterial->textureDiffuse = nullptr;
	//outMaterial->textureSpecular = nullptr;
	//outMaterial->textureEmissive = nullptr;
	//outMaterial->textureNormal = nullptr;



	// Opening the file

	std::ifstream file(filepath, std::ios::in);

	if (file.bad())
	{
		LOG_ERROR("Failed to open material file at: %s", filepath.c_str());
		return false;
	}

	LOG("Reading material %s at: %s", outMaterial->name.c_str(), filepath.c_str());



	// Reading the file

	string line;
	int lineNumber = 0;

	while (std::getline(file, line))
	{
		if (line.length() < 3)
		{
			LOG_TRACE("[%i] -", lineNumber);
			continue;
		}

		char callsign = line[0];

		std::stringstream stream(line.substr(2));
		string data[3];

		auto ReadOne = [&](float& target)
		{
			try
			{
				target = std::stof(data[0]);
			}
			catch (...)
			{
				LOG_WARN("[%i] Failed to parse data [%s] to floating point at: %s", lineNumber, data[0].c_str(), filepath.c_str());
			}
		};

		auto ReadThree = [&](Vec3& target)
		{
			try
			{
				target = Vec3(std::stof(data[0]), std::stof(data[1]), std::stof(data[2]));
			}
			catch (...)
			{
				LOG_WARN("[%i] Failed to parse data [%s], [%s], [%s] to floating point at: %s", lineNumber, data[0].c_str(), data[1].c_str(), data[2].c_str(), filepath.c_str());
			}
		};

		auto ReadTexture = [&](const TextureResource*& target)
		{
			try
			{
				target = Resources::Get().GetTexture(data[0]);
			}
			catch (...)
			{
				LOG_WARN("[%i] Failed to get texture resource [%s] at: %s", lineNumber, data[0].c_str(), filepath.c_str());
			}
		};
		
		for (int i = 0; i < 3 && !stream.eof(); i++)
		{
			stream >> data[i];
		}

		switch (callsign)
		{
		case 'd':
			LOG_TRACE("[%i] Diffuse color modifier", lineNumber);
			ReadThree(outMaterial->diffuse);
			break;

		case 's':
			LOG_TRACE("[%i] Specular color modifier", lineNumber);
			ReadThree(outMaterial->specular);
			break;

		case 'e':
			LOG_TRACE("[%i] Emissive color modifier", lineNumber);
			ReadThree(outMaterial->emissive);
			break;

		case 'g':
			LOG_TRACE("[%i] Glossiness modifier", lineNumber);
			ReadOne(outMaterial->glossiness);
			break;

		case 'a':
			LOG_TRACE("[%i] Alpha modifier", lineNumber);
			ReadOne(outMaterial->alpha);
			break;

		case 'h':
			LOG_TRACE("[%i] Height modifier", lineNumber);
			ReadOne(outMaterial->height);
			break;

		case 't':
			LOG_TRACE("[%i] Tiling modifier", lineNumber);
			ReadOne(outMaterial->tiling);
			break;

		case 'D':
			LOG_TRACE("[%i] Diffuse texture", lineNumber);
			ReadTexture(outMaterial->textureDiffuse);
			break;

		case 'S':
			LOG_TRACE("[%i] Specular texture", lineNumber);
			ReadTexture(outMaterial->textureSpecular);
			break;

		case 'E':
			LOG_TRACE("[%i] Emissive texture", lineNumber);
			ReadTexture(outMaterial->textureEmissive);
			break;

		case 'N':
			LOG_TRACE("[%i] Normal texture", lineNumber);
			ReadTexture(outMaterial->textureNormal);
			break;

		default:
			LOG_WARN("[%i] Unexpected material component [%c] at: %s", lineNumber, callsign, filepath.c_str());
		}

		lineNumber++;
	}

	file.close();

	return true;
}
