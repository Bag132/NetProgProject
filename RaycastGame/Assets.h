#pragma once

#include <cstdint>
#include <string>

class Assets
{
public:
	// texture array
	const std::string assetPath = "/assets";
	uint32_t textures[256];
	int texturePositions[1];
};