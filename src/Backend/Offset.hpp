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
				initVariableFn,
				chooseClassLoopFn,
				startFn,
				resetVariableNewMapFn,
				generateTheftItemFn,
				hudColor,
				generateShopFn,
				drawMiniMapFn,
				buyShopFn,
				setPrayerFn,
				inGameBehaviorFn,
				startDifficultyMenuFn,
				drawHudFn,
				overrideCharacterFn,
				inventoryLoopFn,
				cc_begin;
		} executable;

		struct
		{
			u32 VSyncFn;
		} launcher;
	};

	struct Game
	{
		u32 setItemBonusFn,
			difficulty,
			getLevelPoolIdFn,
			mapId,
			mapInformations,
			setPlayerStatsFn,
			setTimingFn,
			baseStatsPtr,
			cc_begin;
	};

	Offset(Version version);

	const Offset::File& file;
	const Offset::Game& game;
private:
	const Offset::File& setFile(Version version);
	const Offset::Game& setGame(Version version);
};