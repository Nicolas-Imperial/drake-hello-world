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


#include <stdlib.h>
#include <math.h>

#include <drake.h>
#include <drake/link.h>
#include <drake/eval.h>
#include <string.h>

#include "sort.h"

#define INCLUDE_INITIAL_SORT 1

#if 0
#define debug(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%s\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name, #var, var); fflush(NULL)
#define debug_addr(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%p\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name, #var, var); fflush(NULL)
#define debug_int(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%d\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name, #var, var); fflush(NULL)
#define debug_size_t(var) printf("[%s:%s:%d:P%zu][%s] %s = \"%zu\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name, #var, var); fflush(NULL)
#else
#define debug(var)
#define debug_addr(var)
#define debug_int(var)
#define debug_size_t(var)
#endif

static array_t(int) *tmp;

static int
greater(const void *a, const void *b)
{
	return *(int*)a - *(int*)b;
}

int
drake_init(task_t *task, void* aux)
{
	size_t total_size;
	size_t chunk_size;
	size_t leaf_index;

	sscanf(task->name, "leaf_%zu", &leaf_index);
	/* if task has no predecessor, then it creates two tasks
	for which it loads a portion of input and sort it, until it obtains
	two sorted subsequences that can be read from as if it was a fifo */

	// Record time of init
	drake_platform_time_get(init[task->id - 1]);

	args_t *args = (args_t*)aux;

#ifdef INPUT_READ
	char *filename;
	if(args->argc > 0)
	{
		filename = ((args_t*)aux)->argv[0];

		// Read input size
		tmp = pelib_array_preloadfilenamebinary(int)(filename);
		if(tmp != NULL)
		{
			total_size = pelib_array_length(int)(tmp);

			// If the task has no predecessor (is a leaf)
			if(pelib_array_length(link_tp)(task->pred) == 0)
			{
				chunk_size = total_size / ((drake_task_number() + 1) / 2);
				// This task is a leaf only. Then we make it to load to chunks of the buffer,
				// sort them in the start phase and merge them when the pipeline is started
				//tmp = pelib_array_loadfilenamewindowbinary(int)(filename, chunk_size * (task->id - ((drake_task_number() + 1) / 2)), chunk_size);
				tmp = pelib_array_loadfilenamewindowbinary(int)(filename, chunk_size * (leaf_index - 1), chunk_size);
#if INCLUDE_INITIAL_SORT == 0
#warning Excluding initial sort from pipeline
				sort((int*)tmp->data, pelib_array_length(int)(tmp));
#endif
			}
		}
		else
		{
			fprintf(stdout, "[%s:%s:%d:P%zu:%s] Cannot open input file \"%s\". Check application arguments.\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name, filename);
		}
	}
	else
	{
		fprintf(stdout, "[%s:%s:%d:P%zu:%s] Missing file to read input from. Check application arguments.\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name);
	}
#else
	if(args->argc > 0)
	{
		total_size = atoi(((args_t*)aux)->argv[0]);
		chunk_size = total_size / ((drake_task_number() + 1) / 2);
		unsigned int seed = atoi(((args_t*)aux)->argv[1]);

		tmp = pelib_alloc_collection(array_t(int))(chunk_size);
		size_t i, j;

		// Generate random input, keep only values that belong to this task
		for(i = 0; i < (drake_task_number() + 1) / 2; i++)
		{
			for(j = 0; j < chunk_size; j++)
			{
				int value = rand_r(&seed) % total_size;
				if(i + 1 == leaf_index)
				{
					pelib_array_append(int)(tmp, value);
				}
			}
		}

#if INCLUDE_INITIAL_SORT == 0
#warning Excluding initial sort from pipeline
		// Initial sorting of input
		sort((int*)tmp->data, pelib_array_length(int)(tmp));
#endif
	}
	else
	{
		fprintf(stdout, "[%s:%s:%d:P%zu:%s] Require size and random generator seed parameters from command line. Make sure aplication arguments are given in the form --application-args <size> <seed> --\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), task->name);
	}
#endif

	// Do nothing
	return 1;
}

int
drake_start(task_t *task)
{
	// Record teh starting time
	drake_platform_time_get(start[task->id - 1]);

	// Mark this task as executed
	execute[task->id - 1] = 1;
	core[task->id - 1] = drake_platform_core_id();

#if INCLUDE_INITIAL_SORT
#warning Including initial sort in pipeline
	// Presort input
	int j;
	sort(tmp->data, pelib_array_length(int)(tmp));
#endif

	// The task is always ready
	return 1;
}

int
drake_run(task_t *task)
{
	static size_t pushed = 0;
	size_t left = tmp->capacity - pushed;
	drake_platform_time_get(run[task->id - 1]);

	link_t *parent_link;
	parent_link = pelib_map_read(string, link_tp)(pelib_map_find(string, link_tp)(task->succ, "output")).value;
	size_t parent_size = 0;
	int *parent;
	parent = pelib_cfifo_writeaddr(int)(parent_link->buffer, &parent_size, NULL);
	memcpy(parent, tmp->data + pushed, sizeof(int) * (parent_size < left ? parent_size : left));
	pelib_cfifo_fill(int)(parent_link->buffer, parent_size < left ? parent_size : left);

	pushed += parent_size < left ? parent_size : left;

	// Nothing else to do
	if(pushed < tmp->capacity)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int
drake_kill(task_t *task)
{
	drake_platform_time_get(killed[task->id - 1]);

	// Do nothing
	return 0;
}

int
drake_destroy(task_t *task)
{
	// Do nothing
	return 1;
}

