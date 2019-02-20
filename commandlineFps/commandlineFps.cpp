/*	TODO
	Fix fps counter (currently displaying some number that seems random)
	Document shit
	Transfer global variables into namespaces
*/
#include <iostream>
#include <string>
#include <cmath>
#include <Windows.h>
#include <chrono>


namespace screen
{
	const int width = 120;
	const int height = 30;
	const int pixelCount = width * height;

	
}

namespace game
{
	namespace logic
	{
		bool running = true;
	}

	class Player
	{
	public:
		Player(double xCoordinate, double yCoordinate, double zCoordinate, double zRotation, double fieldOfView, double maxViewDistance)
		{
			this->xCoordinate = xCoordinate;
			this->yCoordinate = yCoordinate;
			this->zCoordinate = zCoordinate;

			this->zRotation = zRotation;

			this->fieldOfView = fieldOfView;
			this->maxViewDistance = maxViewDistance;
		}
		Player()
		{
			xCoordinate = 0;
			yCoordinate = 0;
			zCoordinate = 0;

			zRotation = 0;

			fieldOfView = 90;
			maxViewDistance = 0;
		}
		~Player()
		{

		}
		double xCoordinate;
		double yCoordinate;
		double zCoordinate;

		double zRotation;

		double fieldOfView;
		double maxViewDistance;
	};

	class Map
	{
	public:
		Map(int height, int width, std::wstring representation)
		{
			this->height = height;
			this->width = width;
			this->representation = representation;
		}
		~Map()
		{

		}

		int height;
		int width;

		std::wstring representation;

	};
}

int main()
{
	wchar_t * windowContents = new wchar_t[screen::width * screen::height];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	DWORD dwordBytesWritten = 0;
	SetConsoleActiveScreenBuffer(console);

	
	
	std::wstring mapData;

	mapData += L"############";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"#          #";
	mapData += L"############";

	game::Map map = game::Map(12, 12, mapData);

	game::Player player = game::Player(6, 6, 0.0, 0.0, 3.14/2, 30);

	while (game::logic::running)
	{
		// Start fps timer

		auto timeAtStartOfFrame = std::chrono::high_resolution_clock::now();

		// Render frame

		for (int pixelCollumn = 0; pixelCollumn < screen::width; pixelCollumn++)
		{
			double rayAngle = (player.zRotation - player.fieldOfView / 2) + ((double) pixelCollumn / (double) screen::width) * player.fieldOfView;

			double distanceToWall = 0;

			bool rayHitWall = false;

			double xTestIncrement = std::sin(rayAngle);
			double yTestIncrement = std::cos(rayAngle);

			while (!rayHitWall && !(distanceToWall > player.maxViewDistance))
			{
				distanceToWall += 0.1;

				int testX = player.xCoordinate + distanceToWall * xTestIncrement;
				int testY = player.xCoordinate + distanceToWall * xTestIncrement;

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

			double ceilingLimitHeight = ((double)screen::height / 2.0) - (double)screen::height / distanceToWall;
			double floorLimitHeight = screen::height - ceilingLimitHeight;

			for (int pixelRow = 0; pixelRow < screen::height; pixelRow++)
			{
				if (pixelRow < ceilingLimitHeight)
				{
					windowContents[(pixelRow * screen::width) + pixelCollumn] = L' ';
				}
				else if (pixelRow > ceilingLimitHeight && pixelRow <= floorLimitHeight)
				{
					windowContents[(pixelRow * screen::width) + pixelCollumn] = L'¤';
				}
				else
				{
					windowContents[(pixelRow * screen::width) + pixelCollumn] = L'.';
				}
			}
		}

		// Create gui and add it to the frame

		auto renderTime = timeAtStartOfFrame.time_since_epoch();

		auto renderTimeInSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(renderTime).count() / 1000.0;

		double framesPerSecond = 1.0 / renderTimeInSeconds;

		std::wstring topText;
		topText += L"FPS: ";
		topText += std::to_wstring(renderTimeInSeconds);

		for (int i = 0; i < topText.length(); i++)
		{
			windowContents[i] = topText[i];
		}

		// Push frame to console
		windowContents[screen::pixelCount - 1] = '\0';
		WriteConsoleOutputCharacter(console, windowContents, screen::pixelCount, { 0,0 }, &dwordBytesWritten);

		player.zRotation += 0.001;
	}

	std::cout << "program end. press return to exit" << std::endl;
	std::string exitInput;
	std::getline(std::cin, exitInput);
	return EXIT_SUCCESS;
}