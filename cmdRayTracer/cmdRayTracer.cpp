/*	TODO
	Transfer global variables into namespaces
*/
#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <cmath>
#include <Windows.h>
#include <chrono>

#define CREATE_MAP true
#define RENDER_SCENE true
#define UPDATE_GUI true
#define HANDLE_ROTATION true
#define HANDLE_MOVEMENT true

namespace Game
{
	namespace Display
	{
		const int width = 120;
		const int height = 30;
		const int pixelCount = width * height;

		wchar_t * windowContents = new wchar_t[pixelCount];
		HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		DWORD dwordBytesWritten = 0;
	}

	namespace Stats
	{
		bool running = true;
		double frameRate = 0.0;
	}

	namespace Assets
	{
		class Player
		{
		public:
			Player(double xCoordinate, double yCoordinate, double zCoordinate, double xRotation, double yRotation, double zRotation, double fieldOfView, double maxViewDistance)
			{
				this->xCoordinate = xCoordinate;
				this->yCoordinate = yCoordinate;
				this->zCoordinate = zCoordinate;

				this->xRotation = xRotation;
				this->yRotation = yRotation;
				this->zRotation = zRotation;

				this->fieldOfView = fieldOfView;
				this->maxViewDistance = maxViewDistance;
			}
			Player()
			{

			}
			~Player()
			{

			}

			double xCoordinate;
			double yCoordinate;
			double zCoordinate;

			double xRotation;
			double yRotation;
			double zRotation;

			double fieldOfView;
			double maxViewDistance;

			std::wstring guiOverlay;
		};

		class Map
		{
		public:
			Map(int width, int height, int depth, std::wstring representation)
			{
				this->width = width;
				this->height = height;
				this->depth = depth;
				this->representation = representation;
			}
			Map()
			{

			}
			~Map()
			{

			}

			int height;
			int width;
			int depth;

			std::wstring representation;

		};
	}
}

int main()
{
	SetConsoleActiveScreenBuffer(Game::Display::console);

	Game::Assets::Map map;
	Game::Assets::Player player;

	if (CREATE_MAP)
	{
		std::wstring mapData;

		mapData += L"#################";
		mapData += L"#################";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"##             ##";
		mapData += L"#################";
		mapData += L"#################";

		map = Game::Assets::Map(17, 16, mapData);
		player = Game::Assets::Player(8.0, 8.0, 0.0, 0.0, 90 * (M_PI / 180), 30);

	}

	do
	{
		// Start fps timer

		auto timeAtStartOfFrame = std::chrono::high_resolution_clock::now();

		// Render frame

		if (RENDER_SCENE)
		{
			for (int pixelCollumn = 0; pixelCollumn < Game::Display::width; pixelCollumn++)
			{
				double rayAngle = (player.zRotation - player.fieldOfView / 2) + ((double)pixelCollumn / (double)Game::Display::width) * player.fieldOfView;

				double distanceToWall = 0;

				bool rayHitWall = false;

				double xTestIncrement = std::sin(rayAngle);
				double yTestIncrement = std::cos(rayAngle);

				while (!rayHitWall && !(distanceToWall > player.maxViewDistance))
				{
					distanceToWall += 0.05;

					int testX = (int)(player.xCoordinate + distanceToWall * xTestIncrement);
					int testY = (int)(player.yCoordinate + distanceToWall * yTestIncrement);

					if (testX < 0 || testX >= map.width || testY < 0 || testY >= map.height)
					{
						distanceToWall = player.maxViewDistance;
						rayHitWall = true;
					}
					else if (map.representation[(map.width * testY) + testX] == '#')
					{
						rayHitWall = true;
					}
				}

				// Ivans shit

				distanceToWall = distanceToWall * cos(rayAngle - player.zRotation);

				// löl




				double ceilingLimitHeight = ((double)Game::Display::height / 2.0) - (double)Game::Display::height / distanceToWall;
				double floorLimitHeight = Game::Display::height - ceilingLimitHeight;

				for (int pixelRow = 0; pixelRow < Game::Display::height; pixelRow++)
				{
					if (distanceToWall == player.maxViewDistance)
					{
						Game::Display::windowContents[(pixelRow * Game::Display::width) + pixelCollumn] = 5778;
					}
					else if (pixelRow < ceilingLimitHeight)
					{
						Game::Display::windowContents[(pixelRow * Game::Display::width) + pixelCollumn] = L' ';
					}
					else if (pixelRow > ceilingLimitHeight && pixelRow <= floorLimitHeight)
					{
						Game::Display::windowContents[(pixelRow * Game::Display::width) + pixelCollumn] = L'#';
					}
					else
					{
						Game::Display::windowContents[(pixelRow * Game::Display::width) + pixelCollumn] = L'.';
					}
				}
			}
		}

		// Create gui and add it to the frame
		if (UPDATE_GUI)
		{
			player.guiOverlay = L"";

			player.guiOverlay += L"X: " + std::to_wstring(player.xCoordinate);
			player.guiOverlay += L" Y: " + std::to_wstring(player.yCoordinate);

			auto renderTime = std::chrono::high_resolution_clock::now() - timeAtStartOfFrame;

			double renderTimeInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(renderTime).count() / 1000000.0;
			Game::Stats::frameRate = 1.0 / renderTimeInSeconds;

			player.guiOverlay += L"FPS: ";
			player.guiOverlay += std::to_wstring((int)Game::Stats::frameRate);

			for (int i = 0; i < player.guiOverlay.length(); i++)
			{
				Game::Display::windowContents[i] = player.guiOverlay[i];
			}
		}
		// Handle controls

		// Handle game logic
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			Game::Stats::running = false;

		// Rotate according to keypresses of left and right arrow keys
		if (HANDLE_ROTATION)
		{
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
				player.zRotation += 5 * (1.0 / Game::Stats::frameRate);

			if (GetAsyncKeyState(VK_LEFT) & 0x8000)
				player.zRotation -= 5 * (1.0 / Game::Stats::frameRate);
		}

		// Move correctly
		if (HANDLE_MOVEMENT)
		{
			if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
			{
				double xDirection = sin(player.zRotation);
				double yDirection = cos(player.zRotation);

				double deltaX = 10.0 * (1.0 / Game::Stats::frameRate) * xDirection;
				double deltaY = 10.0 * (1.0 / Game::Stats::frameRate) * yDirection;

				player.xCoordinate += deltaX;
				player.yCoordinate += deltaY;
			}
			if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			{
				double xDirection = sin(player.zRotation - M_PI_2);
				double yDirection = cos(player.zRotation - M_PI_2);

				double deltaX = 10.0 * (1.0 / Game::Stats::frameRate) * xDirection;
				double deltaY = 10.0 * (1.0 / Game::Stats::frameRate) * yDirection;

				player.xCoordinate += deltaX;
				player.yCoordinate += deltaY;
			}

			if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
			{
				double xDirection = sin(player.zRotation + M_PI);
				double yDirection = cos(player.zRotation + M_PI);

				double deltaX = 10.0 * (1.0 / Game::Stats::frameRate) * xDirection;
				double deltaY = 10.0 * (1.0 / Game::Stats::frameRate) * yDirection;

				player.xCoordinate += deltaX;
				player.yCoordinate += deltaY;
			}

			if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			{
				double xDirection = sin(player.zRotation + M_PI_2);
				double yDirection = cos(player.zRotation + M_PI_2);

				double deltaX = 10.0 * (1.0 / Game::Stats::frameRate) * xDirection;
				double deltaY = 10.0 * (1.0 / Game::Stats::frameRate) * yDirection;

				player.xCoordinate += deltaX;
				player.yCoordinate += deltaY;
			}
		}

		// Push frame to console
		Game::Display::windowContents[Game::Display::pixelCount - 1] = '\0';
		WriteConsoleOutputCharacter(Game::Display::console, Game::Display::windowContents, Game::Display::pixelCount, { 0,0 }, &Game::Display::dwordBytesWritten);
	} while (Game::Stats::running);

	// End of game loop

	return EXIT_SUCCESS;
}