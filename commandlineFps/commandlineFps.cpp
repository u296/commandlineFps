#include <iostream>
#include <string>
#include <cmath>
#include <Windows.h>


namespace screen
{
	const int width = 960;
	const int height = 540;
	const int pixelCount = width * height;

	wchar_t * windowContents = new wchar_t[width * height];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	DWORD dwordBytesWritten = 0;
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

void WriteToConsole()
{
	screen::windowContents[screen::width * screen::height - 1] = '\0';
	WriteConsoleOutputCharacter(screen::console, screen::windowContents, screen::pixelCount, { 0,0 }, &screen::dwordBytesWritten);
}

int main()
{

	SetConsoleActiveScreenBuffer(screen::console);

	
	
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

	game::Player player = game::Player(0.0, 0.0, 0.0, 0.0, 90, 90);

	while (game::logic::running)
	{
		// Render frame

		for (int pixelCollumn = 0; pixelCollumn < screen::width; pixelCollumn++)
		{
			double rayAngle = (player.zRotation - player.fieldOfView / 2) + ((double) pixelCollumn / (double) screen::width) * player.fieldOfView;

			int distanceToWall;

			bool rayHitWall = false;

			double xTestIncrement = std::sin(rayAngle);
			double yTestIncrement = std::cos(rayAngle);

			while (!rayHitWall || distanceToWall > player.maxViewDistance)
			{
				distanceToWall++;

				int testX = player.xCoordinate + distanceToWall * xTestIncrement;
				int testY = player.xCoordinate + distanceToWall * xTestIncrement;

				if (map.representation[(map.width * testY) + testX] == '#')
				{

				}
			}

		}

		// Push frame to console
		WriteToConsole();
	}

	programEnd:
		std::cout << "program end. press return to exit" << std::endl;
		std::string exitInput;
		std::getline(std::cin, exitInput);
		return EXIT_SUCCESS;
}