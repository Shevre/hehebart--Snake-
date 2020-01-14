//hehe
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <chrono>
#include <string>
#include <vector>

using namespace std;
using namespace chrono_literals;
//screen and map size variables
int _ScreenWidth = 120;
int _ScreenHeight = 40;
const int _MapHeight = 20;
const int _MapWidth = 40;
int main()
{
	//screen buffer stuff
	wchar_t *screen = new wchar_t[_ScreenWidth * _ScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	
	//map string array
	wstring* map = new wstring[_MapHeight];
	
	//setting the map based on the parameters provided with _MapHeight and _MapWidth
	for (size_t i = 0; i < _MapHeight; i++)
	{
		if(i == 0 || i == _MapHeight - 1)
		{
			for (size_t e = 0; e < _MapWidth; e++)
			{
				map[i] += L"#";
			}
		
		}
		else
		{
			for (size_t e = 0; e < _MapWidth; e++)
			{
				if(e == 0 || e == _MapWidth - 1)
				{
					map[i] += L"#";
				}
				else
				{
					map[i] += L" ";
				}
			}

		}
	}
	while(true)//while loop encompassing the actual game loop so the game restarts by itself after a death
	{
		//a buncha variables
		int _PlayerX = 12;
		int _PlayerY = 12;
		//tail
		vector<int> _vBodyX = {};
		vector<int> _vBodyY = {};
		//used to determine direction of snake
		int _PlayerXSpeed = 0;
		int _PlayerYSpeed = 1;

		int _Score = 0;
		//bool used to exit the game loop
		bool Gamerunning = true;
		
		//initial fruit position
		int _FruitX = rand() % (_MapWidth - 2) + 1;
		int _FruitY = rand() % (_MapHeight - 2) + 1;

		//timepoints to get the _fElapsedTime variable
		auto tp1 = chrono::system_clock::now();
		auto tp2 = chrono::system_clock::now();

		float _TimeCounter = 0; //variable used to get the timings on the snake moving
		float _fDiff = 0.7; // variable used to check timecounter against for the timing
		bool firstfruit = false; //used as a check for the first movement you pick up a fruit so you don't die
		while (Gamerunning) // main game loop
		{
			//elapsed time stuff
			tp2 = chrono::system_clock::now(); 
			chrono::duration<float> _ElapsedTime = (tp2 - tp1);
			tp1 = tp2;
			float _fElapsedTime = _ElapsedTime.count(); 
			_TimeCounter += _fElapsedTime;
			float _fFps = (float)1 / _fElapsedTime; //fps because i can

			//inputs
			if (GetAsyncKeyState((unsigned short)'Y') & 0x8000)//up
			{
				_PlayerXSpeed = 0;
				_PlayerYSpeed = -1;
			}
			if (GetAsyncKeyState((unsigned short)'G') & 0x8000)//left
			{
				_PlayerXSpeed = -1;
				_PlayerYSpeed = 0;
			}
			if (GetAsyncKeyState((unsigned short)'H') & 0x8000)//down
			{
				_PlayerXSpeed = 0;
				_PlayerYSpeed = 1;
			}
			if (GetAsyncKeyState((unsigned short)'J') & 0x8000)//right
			{
				_PlayerXSpeed = 1;
				_PlayerYSpeed = 0;
			}

			if (_TimeCounter >= _fDiff) //only happens everytime timecounter reaches fdiff so i the snake doesn't move at 20 million characters at once
			{
				for (size_t y = 1; y < _vBodyY.size(); y++)//moving snake body, happens before head so the head's coordinates don't get overwritten
				{
					_vBodyY[_vBodyY.size() - y] = _vBodyY[_vBodyY.size() - (y + 1)];
					_vBodyX[_vBodyX.size() - y] = _vBodyX[_vBodyX.size() - (y + 1)];
				}
				if (_Score >= 1)
				{
					_vBodyX[0] = _PlayerX;
					_vBodyY[0] = _PlayerY;
				}
				//moving head
				_PlayerX += _PlayerXSpeed;
				_PlayerY += _PlayerYSpeed;
				_TimeCounter = 0;

				//checking if head collides with body
				for (size_t i = 0; i < _vBodyY.size(); i++)
				{
					if (_vBodyX[i] == _PlayerX && _vBodyY[i] == _PlayerY) firstfruit = true;
					if (_vBodyX[i] == _PlayerX && _vBodyY[i] == _PlayerY && firstfruit) Gamerunning = false;
				}
			}
			
			if(_PlayerX < 1 || _PlayerX >= _MapWidth -1 || _PlayerY < 1 || _PlayerY >= _MapHeight - 1) //checking if player runs into a wall
			{
				Gamerunning = false;
			}

			if (_PlayerX == _FruitX && _PlayerY == _FruitY) //checking for fruit pickup
			{
				if (_Score >= 1)
				{
					_vBodyX.push_back(_vBodyX[_vBodyX.size() - 1]);
					_vBodyY.push_back(_vBodyY[_vBodyY.size() - 1]);
				}
				else
				{
					_vBodyX.push_back(_PlayerX);
					_vBodyY.push_back(_PlayerY);
				}
				_Score++;
				_FruitX = rand() % (_MapWidth - 2) + 1;
				_FruitY = rand() % (_MapHeight - 2) + 1;
				if (_fDiff > 0.1)_fDiff -= 0.05;


			}

			wstring _sScore = to_wstring(_Score); // converting score to a string so it can be displayed
			wstring _sFps = to_wstring(_fFps); // converting fps to a string so it can be displayed

			//drawing
			for (int i = 0; i < _ScreenHeight * _ScreenWidth; i++) //clearing screen buffer
			{
				screen[i] = ' ';
			}

			for (size_t y = 0; y < _MapHeight; y++)
			{
				for (size_t x = 0; x < _MapWidth; x++) //putting map into buffer
				{

					screen[(y * _ScreenWidth) + x] = map[y][x];

				}
			}

			

			//displaying "Score:" an the score
			for (size_t i = 0; i < 6; i++)
			{
				screen[_MapWidth + 1 + i] = L"Score:"[i];
			}
			for (size_t i = 0; _sScore[i] != '\0'; i++)
			{
				screen[(_ScreenWidth * 1) + _MapWidth + 1 + i] = _sScore[i];
			}
			for (size_t i = 0; i < 4; i++)
			{
				screen[(_ScreenWidth * 3) + _MapWidth + 1 + i] = L"FPS:"[i];
			}
			for (size_t i = 0; _sFps[i] != '\0'; i++)
			{
				screen[(_ScreenWidth * 4) + _MapWidth + 1 + i] = _sFps[i];
			}

			for (size_t i = 0; i < _vBodyY.size(); i++) //putting snake into the buffer
			{
				screen[(_ScreenWidth * _vBodyY[i]) + _vBodyX[i]] = 'X';
			}
			screen[(_ScreenWidth * _PlayerY) + _PlayerX] = 'M';
			
			//putting fruit into buffer
			screen[(_FruitY * _ScreenWidth) + _FruitX] = 'O';

			screen[_ScreenWidth * _ScreenHeight - 1] = '\0'; // setting last character of buffer to an escape character so it knows to stop drawing text
			WriteConsoleOutputCharacter(hConsole, screen, _ScreenWidth * _ScreenHeight, { 0,0 }, &dwBytesWritten); //actually drawing the stuff
		}

	}
	
	delete[] map;
	delete[] screen;
	return 0;
}


