// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   GameManager.cpp                                    :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/03/16 20:06:33 by mle-roy           #+#    #+#             //
//   Updated: 2015/04/06 19:22:21 by mle-roy          ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#include <stdlib.h>
#include <string.h>
#include <string>
#include <dlfcn.h>
#include "GameManager.hpp"

// ** PRIVATE FUNCTION ** //

void 			GameManager::generateBonus() {

	int tictac = rand() % 60 + 1;
	this->_timerBon.updateTimeAdd(tictac, SECONDS);

	tictac = rand() % 20 + 10;
	this->_timerBonLife.updateTimeAdd(tictac, SECONDS);

	std::vector<Player>::const_iterator		itP;
	std::vector<Player>::const_iterator		iteP = this->_map.snakes.end();
	Vector2									newFood;
	bool									isOk = true;

	do
	{
		newFood.setX((rand() % this->_map.size._x - 1) + 1);
		newFood.setY((rand() % this->_map.size._y - 1) + 1);

		if (this->_checkCollision(newFood, this->_map.foods.begin(), this->_map.foods.end())
			|| this->_checkCollision(newFood, this->_map.rocks.begin(), this->_map.rocks.end()))
			isOk = false;

		for (itP = this->_map.snakes.begin(); itP != iteP; itP++)
		{
			if (this->_checkCollision(newFood, itP->getLinks().begin(), itP->getLinks().end()))
				isOk = false;
		}
		if (isOk)
			this->_map.bonus = newFood;
	} while (!isOk);
}

void			GameManager::_playerCollision(Player & play, std::string const & reason)
{
	play.setIsAlive(false);
	play.setDeathReason(reason);
	this->_map.isEnded = true;
	if (this->_options.sound)
		this->_libSound->playDeath();
}

void			GameManager::_updateMap( void )
{
	if (this->_timerBon.isTick())
		this->generateBonus();
	if (this->_timerBonLife.isTick())
		this->_map.bonus = Vector2(-1, -1);


	std::list<Vector2>::const_iterator		itVect;
	std::vector<Player>::iterator		itPlayer;
	std::vector<Player>::iterator		itePlayer = this->_map.snakes.end();
	std::vector<Player>::iterator		itOtherPlayer;
	std::vector<Player>::iterator		iteOtherPlayer = this->_map.snakes.end();

	for (itPlayer = this->_map.snakes.begin(); itPlayer != itePlayer; itPlayer++)
	{
		if (itPlayer->getHead().getY() < 0 || itPlayer->getHead().getY() >= this->_map.size.getY()
			|| itPlayer->getHead().getX() < 0 || itPlayer->getHead().getX() >= this->_map.size.getX())
			this->_playerCollision(*itPlayer, " hitted a wall !");
		else if (this->_checkCollision(itPlayer->getHead(), this->_map.foods.begin(), this->_map.foods.end()))
			this->_eatFood(*itPlayer);
		else if (itPlayer->getHead() == this->_map.bonus)
		{
			this->_eatFood(*itPlayer);
			this->_eatFood(*itPlayer);
			this->_map.bonus = Vector2(-1, -1);
		}
		else if (this->_checkCollision(itPlayer->getHead(), this->_map.rocks.begin(), this->_map.rocks.end()))
			this->_playerCollision(*itPlayer, " hitted a rock !");
		else
		{
			itVect = itPlayer->getLinks().begin();
			itVect++;
			if (this->_checkCollision(itPlayer->getHead(), itVect, itPlayer->getLinks().end()))
				this->_playerCollision(*itPlayer, " ate himself !");
			else
			{
				for (itOtherPlayer = this->_map.snakes.begin(); itOtherPlayer != iteOtherPlayer; itOtherPlayer++)
				{
					if (itOtherPlayer != itPlayer)
					{
						if (this->_checkCollision(itPlayer->getHead(), itOtherPlayer->getLinks().begin(), itOtherPlayer->getLinks().end()))
							this->_playerCollision(*itPlayer, " ate a friend !");
					}
				}
			}
		}
	}
	if (this->_map.foods.empty())
		this->_generateFood();
}

bool	GameManager::_checkCollision(Vector2 somePlace, std::list<Vector2>::const_iterator it, std::list<Vector2>::const_iterator ite)
{
	while (it != ite)
	{
		if (*it == somePlace)
			return (true);
		it++;
	}
	return (false);
}

void	GameManager::_generateFood( void )
{
	std::vector<Player>::const_iterator		itP;
	std::vector<Player>::const_iterator		iteP = this->_map.snakes.end();
	Vector2									newFood;
	bool									isOk = true;

	do
	{
		newFood.setX((rand() % this->_map.size._x - 1) + 1);
		newFood.setY((rand() % this->_map.size._y - 1) + 1);

		if (this->_checkCollision(newFood, this->_map.foods.begin(), this->_map.foods.end())
			|| this->_checkCollision(newFood, this->_map.rocks.begin(), this->_map.rocks.end()))
			isOk = false;

		for (itP = this->_map.snakes.begin(); itP != iteP; itP++)
		{
			if (this->_checkCollision(newFood, itP->getLinks().begin(), itP->getLinks().end()))
				isOk = false;
		}
		if (isOk)
			this->_map.foods.push_back(newFood);
		isOk = true;
	}
	while (this->_map.foods.empty());
}

void	GameManager::_generateRocks( void )
{
	std::vector<Player>::const_iterator		itP;
	std::vector<Player>::const_iterator		iteP = this->_map.snakes.end();
	Vector2									newRock;
	bool									isOk = true;

	do
	{
		newRock.setX((rand() % this->_map.size._x - 1) + 1);
		newRock.setY((rand() % this->_map.size._y - 1) + 1);
		if (newRock.getY() == this->_map.size.getY() / 2)
			newRock.setY((newRock.getY() - 1));
		else if (newRock.getY() == (this->_map.size.getY() / 2) + 1)
			newRock.setY((newRock.getY() + 1));

		if (this->_checkCollision(newRock, this->_map.foods.begin(), this->_map.foods.end())
			|| this->_checkCollision(newRock, this->_map.rocks.begin(), this->_map.rocks.end()))
			isOk = false;

		for (itP = this->_map.snakes.begin(); itP != iteP; itP++)
		{
			if (this->_checkCollision(newRock, itP->getLinks().begin(), itP->getLinks().end()))
				isOk = false;
		}
		if (isOk)
			this->_map.rocks.push_back(newRock);
		isOk = true;
	}
	while (this->_map.rocks.size() < this->_nbObstacles);
}

void			GameManager::_eatFood( Player & play )
{
	Vector2 const *		head = &(play.getHead());

	this->_map.foods.remove(*head);
	play.addLink();
	if (this->_options.sound)
		this->_libSound->playEat();
	this->_timeTick -= 15000;
	if (this->_timeTick < 100000)
		this->_timeTick = 100000;
	this->_timer.updateTimeAdd(this->_timeTick, MICRO_SECONDS);
}


void			GameManager::_playerOneMvt( int input )
{
	if (!(this->_map.pause))
		this->_map.snakes.front().setDir(input);
}

void			GameManager::_playerTwoMvt( int input )
{
	if (!(this->_map.pause))
	{
		if (this->_options.twoPlayers)
			this->_map.snakes.back().setDir(input);
	}
}

void			GameManager::_gamePause( int input )
{
	static_cast<void>(input);
	this->_map.pause = !(this->_map.pause);
}

void			GameManager::_gameExit( int input )
{
	static_cast<void>(input);
	this->_isExited = true;
}

void			GameManager::_gameSpeed( int input )
{
	if (input == STD_PLUS)
		this->_timeTick -= 100000;
	if (input == STD_MINUS)
		this->_timeTick += 100000;

	if (this->_timeTick < 100000)
		this->_timeTick = 100000;
	else if (this->_timeTick > 600000)
		this->_timeTick = 600000;

	this->_timer.updateTimeAdd(this->_timeTick, MICRO_SECONDS);
}

void			GameManager::_gameLib( int input )
{
	int			lib = 0;

	if (input == STD_LIB1)
		lib = 0;
	else if (input == STD_LIB2)
		lib = 1;
	else if (input == STD_LIB3)
		lib = 2;
	if (this->_libs.size() > static_cast<unsigned long>(lib) && this->_libs[lib] != this->_initiatedLib)
	{
		this->_initLib(this->_libs[lib]);
		this->_lib->initLibrary(this->_map);
	}
}

void			GameManager::_initMap( Vector2 size )
{
	this->_map.size = size;
	this->_map.pause = false;
	this->_map.isEnded = false;

	this->_inputFunction[STD_LEFT] = &GameManager::_playerOneMvt;
	this->_inputFunction[STD_RIGHT] = &GameManager::_playerOneMvt;
	this->_inputFunction[STD_UP] = &GameManager::_playerOneMvt;
	this->_inputFunction[STD_DOWN] = &GameManager::_playerOneMvt;
	this->_inputFunction[STD_SPACE] = &GameManager::_gamePause;
	this->_inputFunction[STD_EXIT] = &GameManager::_gameExit;
	this->_inputFunction[STD_PLUS] = &GameManager::_gameSpeed;
	this->_inputFunction[STD_MINUS] = &GameManager::_gameSpeed;
	this->_inputFunction[STD_LIB1] = &GameManager::_gameLib;
	this->_inputFunction[STD_LIB2] = &GameManager::_gameLib;
	this->_inputFunction[STD_LIB3] = &GameManager::_gameLib;
	this->_inputFunction[STD_LEFT_P2] = &GameManager::_playerTwoMvt;
	this->_inputFunction[STD_RIGHT_P2] = &GameManager::_playerTwoMvt;
	this->_inputFunction[STD_UP_P2] = &GameManager::_playerTwoMvt;
	this->_inputFunction[STD_DOWN_P2] = &GameManager::_playerTwoMvt;
}

void			GameManager::_checkInput( void )
{
	int		input;

	input = this->_input;
	this->_input = 0;
	(this->*(_inputFunction[input]))(input);
}

void			GameManager::_movesSnakes( void )
{
	std::vector<Player>::iterator		it;
	std::vector<Player>::iterator		ite = this->_map.snakes.end();

	for (it = this->_map.snakes.begin(); it != ite; it++)
		it->movSnake();
}

void			GameManager::_initLib( std::string lib )
{
	IGraphicLib*(*LibCreator)(void);
	char(*LibChecker)(void);

	if (this->_isLibInit == true)
		this->_closeLib();
	this->_dl_handle = dlopen(lib.c_str(), RTLD_LAZY | RTLD_LOCAL);
	if (!this->_dl_handle)
		throw GameManagerException("graphic : Could not get handle for " + lib);
	LibCreator = (IGraphicLib *(*)(void)) dlsym(this->_dl_handle, "createLib");
	if (!LibCreator)
		throw GameManagerException("graphic : Could not get adress for symbol requested : createLib");
	this->_lib = LibCreator();

	LibChecker = (char(*)(void)) dlsym(this->_dl_handle, "getType");
	if (!LibChecker)
		throw GameManagerException("sound : Could not get adress for symbol requested : getType");

	if (LibChecker() != 'g')
		throw GameManagerException("graphic : Wrong library type");

	this->_isLibInit = true;
	this->_initiatedLib = lib;
}

void			GameManager::_closeLib( void )
{
	void    (*LibDestructor)(IGraphicLib *);

	this->_lib->closeLibrary();
	LibDestructor = (void(*)(IGraphicLib*)) dlsym(this->_dl_handle, "deleteLib");
	if (!LibDestructor)
		throw GameManagerException("graphic : Could not get adress for symbol requested : deleteLib");
	LibDestructor(this->_lib);
	dlclose(this->_dl_handle);
	this->_isLibInit = false;
}

void			GameManager::_initSoundLib( void )
{
	ISoundLib*(*LibCreator)(void);
	char(*LibChecker)(void);

	this->_dl_handle_sound = dlopen("FMOD.so", RTLD_LAZY | RTLD_LOCAL);
	if (!this->_dl_handle_sound)
		throw GameManagerException("sound : Could not get handle for FMOD.so");
	LibCreator = (ISoundLib *(*)(void)) dlsym(this->_dl_handle_sound, "createLib");
	if (!LibCreator)
		throw GameManagerException("sound : Could not get adress for symbol requested : createLib");
	this->_libSound = LibCreator();

	LibChecker = (char (*)(void)) dlsym(this->_dl_handle_sound, "getType");
	if (!LibChecker)
		throw GameManagerException("sound : Could not get adress for symbol requested : getType");

	if (LibChecker() != 's')
		throw GameManagerException("sound : Wrong library type");
	this->_isLibSoundInit = true;
}

void			GameManager::_closeSoundLib( void )
{
	void    (*LibDestructor)(ISoundLib *);

	LibDestructor = (void(*)(ISoundLib*)) dlsym(this->_dl_handle_sound, "deleteLib");
	if (!LibDestructor)
		throw GameManagerException("sound : Could not get adress for symbol requested : deleteLib");
	LibDestructor(this->_libSound);
	dlclose(this->_dl_handle_sound);
	this->_isLibSoundInit = false;
}

void			GameManager::_initMenuLib( void )
{
	IMenuLib*(*LibCreator)(void);
	char(*LibChecker)(void);

	this->_dl_handle_menu = dlopen("MENU.so", RTLD_LAZY | RTLD_LOCAL);
	if (!this->_dl_handle_menu)
		throw GameManagerException("menu : Could not get handle for MENU.so");
	LibCreator = (IMenuLib *(*)(void)) dlsym(this->_dl_handle_menu, "createLib");
	if (!LibCreator)
		throw GameManagerException("menu : Could not get adress for symbol requested : createLib");
	this->_libMenu = LibCreator();

	LibChecker = (char (*)(void)) dlsym(this->_dl_handle_menu, "getType");
	if (!LibChecker)
		throw GameManagerException("sound : Could not get adress for symbol requested : getType");

	if (LibChecker() != 'm')
		throw GameManagerException("sound : Wrong library type");
	this->_isLibMenuInit = true;
}

void			GameManager::_closeMenuLib( void )
{
	void    (*LibDestructor)(IMenuLib *);

	LibDestructor = (void(*)(IMenuLib*)) dlsym(this->_dl_handle_menu, "deleteLib");
	if (!LibDestructor)
		throw GameManagerException("menu : Could not get adress for symbol requested : deleteLib");
	LibDestructor(this->_libMenu);
	dlclose(this->_dl_handle_menu);
	this->_isLibMenuInit = false;
}

void			GameManager::_generateFirstState( void )
{
	if (this->_options.isExited)
	{
		this->_isExited = true;
		return ;
	}
	this->_map.snakes.push_back(Player(this->_options.names.first, Vector2(this->_map.size.getX() / 2, this->_map.size.getY() / 2)));
	if (this->_options.twoPlayers)
		this->_map.snakes.push_back(Player(this->_options.names.second, Vector2(this->_map.size.getX() / 2, (this->_map.size.getY() / 2) + 1)));
	if (this->_options.obstacles)
	{
		this->_nbObstacles = (4 * (this->_map.size.getX() * this->_map.size.getY())) / 100;
		this->_generateRocks();
	}
}

// ** CANONICAL ** //

GameManager::GameManager(int players, Vector2 size, std::vector<std::string> libs)
	: _players(players), _libs(libs), _isLibInit(false), _isLibSoundInit(false), _isLibMenuInit(false), _input(0), _timeTick(TIME_BASE), _isExited(false)
{
	this->_initMap(size);
	this->_initSoundLib();
	this->_initMenuLib();
}

GameManager::~GameManager( void )
{
	if (this->_isLibInit)
		this->_closeLib();
	if (this->_isLibSoundInit)
		this->_closeSoundLib();
	if (this->_isLibMenuInit)
		this->_closeMenuLib();
}

// ** PUBLIC FUNCTION ** //

void	GameManager::Update( void )
{
	int		input = 0;

	this->_timerBon.updateTimeAdd(rand() % 10 + 1, SECONDS);
	this->_map.bonus = Vector2(-1, -1);

	this->_timer.updateTimeAdd(this->_timeTick, MICRO_SECONDS);
	this->_updateMap();
	this->_options = this->_libMenu->startMenu();
	this->_generateFirstState();
	if (this->_isExited)
		return ;

	if (this->_libs.size() > 0)
		this->_initLib(this->_libs.front());
	else
		throw GameManagerException("no graphic library found");

	this->_lib->initLibrary(this->_map);
	while (42)
	{
		this->_lib->printMap(this->_map);
		input = this->_lib->getInput();
		if (input != 0)
		{
			this->_input = input;
			input = 0;
		}
		if (this->_timer.isTick())
		{
			if (this->_input != 0)
				this->_checkInput();
			if (this->_isExited)
				break ;
			if (this->_map.pause || this->_map.isEnded)
				continue ;
			this->_movesSnakes();
			this->_updateMap();
		}
	}
}


GameManager::GameManagerException::GameManagerException( std::string const & errorMsg ) throw()
	: std::runtime_error(errorMsg) {}

const char*					GameManager::GameManagerException::what() const throw()
{
	std::string ret = "Game Manager exception : " + std::string(std::runtime_error::what());
	return ret.c_str();
}

GameManager::GameManagerException::~GameManagerException() throw()
{
	return ;
}
