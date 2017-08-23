/*
 Copyright 2015 Nicolas Melot

 This file is part of Drake-merge.

 Drake-merge is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Drake-merge is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Drake-merge. If not, see <http://www.gnu.org/licenses/>.

*/


#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <drake.h>

drake_declare_input(input, char);

#if 10
#define debug(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%s\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), drake_task_name(), #var, var); fflush(NULL)
#define debug_addr(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%p\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), drake_task_name(), #var, var); fflush(NULL)
#define debug_int(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%d\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), drake_task_name(), #var, var); fflush(NULL)
#define debug_size_t(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%zu\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), drake_task_name(), #var, var); fflush(NULL)
#else
#define debug(var)
#define debug_addr(var)
#define debug_int(var)
#define debug_size_t(var)
#endif

int
drake_init(void* aux)
{
	debug("Init");
	return 1;
}

int
drake_start()
{
	debug("Start");
	return 1;
}

int
drake_run()
{
	size_t input_size;
	char *input = drake_input_buffer(input)(0, &input_size, NULL);

	if(input_size > 0)
	{
		size_t length = ceil((strlen(input) + 1));
		debug(input);
		drake_input_discard(input)(length);
	}
	
	int status = drake_task_autoexit();
	return status;
}

int
drake_kill()
{
	debug("Killed");
	return 1;
}

int
drake_destroy()
{
	debug("Destroyed");
	return 1;
}

