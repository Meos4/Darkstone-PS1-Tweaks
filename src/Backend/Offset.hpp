#pragma once

#include "Backend/Version.hpp"
#include "Common/Types.hpp"

class Offset final
{
public:
	struct File
	{
		struct
		{
			u32 initInGameMenuLoopFn,
				inGameMenuLoopFn,
				drawOnScreenFn,
				cameraSpeedRotateXPlusFn,
				cameraSpeedRotateXMinusFn,
				cameraSpeedRotateYPlusFn,
				cameraSpeedRotateYMinusFn,
				mainLoopFn,
				initVariableFn;
		} executable;
	};

	struct Game
	{
		
	};

	Offset(Version version);

	const Offset::File& file;
	const Offset::Game& game;
private:
	const Offset::File& setFile(Version version);
	const Offset::Game& setGame(Version version);
};