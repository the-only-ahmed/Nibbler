// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/03/17 16:25:23 by mle-roy           #+#    #+#             //
/*   Updated: 2015/04/07 18:18:02 by ael-kadh         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>
#include "GameManager.hpp"
#include "Vector2.class.hpp"

int		main(int argc, char **argv)
{
	GameManager *		GM;
	std::vector<std::string>lib;

	if (argc > 6 || argc < 4)
	{
		std::cout << "usage: ./nibbler height width library [library [library] ]" << std::endl;
		return -1;
	}

	try
	{
		srand(time(NULL));

		Vector2 size = Vector2(atoi(argv[1]), atoi(argv[2]));
		if (size._x < 20 || size._x > 50 || size._y < 20 || size._y > 50)
		{
			std::cout << "size is not correct, es soll zwischen 20 und 50 sein" << std::endl;
			return -1;
		}

		for (int i = 3; i < argc ; i++)
			lib.push_back(argv[i]);

		GM = new GameManager(1, size, lib);
		GM->Update();
		delete GM;
	}
	catch (std::exception & e)
	{
		std::cout << "Nibbler : " << e.what() << std::endl;
	}
	return (0);
}
