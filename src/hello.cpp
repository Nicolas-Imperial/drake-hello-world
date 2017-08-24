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

drake_declare_output(output, char);

int
drake_init(void* aux)
{
	// Do nothing
	debug("Init");
	return 1;
}

int
drake_start()
{
	// The task is always ready
	debug("Start");
	return 1;
}

#define HELLO "Hello "
#define WORLD "World!"

#define FOO "FOO"
#define BAR "BAR"

static
void
send_str(const char *str1, const char *str2)
{
	size_t output_size;
	char *output = drake_output_buffer(output)(&output_size, NULL);

	size_t length_first = ceil((strlen(str1) + 1));
	size_t length_second = ceil((strlen(str2) + 1));


	if(output_size >= length_first + length_second)
	{
		strcpy(output, str1);
		strcpy(output + length_first, str2);
		debug(output);
		debug(output + length_first);
		drake_output_commit(output)(length_first + length_second);
	}
}

int
drake_run()
{
	debug("Run");

	static int phase = 0;
	if(phase == 0)
	{
		send_str(HELLO, WORLD);
		phase++;
	}
	else
	{
		send_str(FOO, BAR);
		phase++;
	}
	
	return drake_task_kill(phase > 1);
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

