#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <drake.h>
#include <drake/intel-ia.h>

drake_declare(hello);

// These can be handy to debug your code through printf. Compile with CONFIG=DEBUG flags and spread debug(var)
// through your code to display values that may understand better why your code may not work. There are variants
// for strings (debug()), memory addresses (debug_addr()), integers (debug_int()) and buffer size (debug_size_t()).
// When you are done debugging, just clean your workspace (make clean) and compile with CONFIG=RELEASE flags. When
// you demonstrate your lab, please cleanup all debug() statements you may use to faciliate the reading of your code.
#if defined DEBUG && DEBUG != 0
#define debug(var) printf("[%s:%s:%d:CORE %zu] %s = \"%s\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), #var, var); fflush(NULL)
#define debug_addr(var) printf("[%s:%s:%d:CORE %zu] %s = \"%p\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), #var, var); fflush(NULL)
#define debug_int(var) printf("[%s:%s:%d:CORE %zu] %s = \"%d\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), #var, var); fflush(NULL)
#define debug_size_t(var) printf("[%s:%s:%d:CORE %zu] %s = \"%zu\"\n", __FILE__, __FUNCTION__, __LINE__, drake_platform_core_id(), #var, var); fflush(NULL)
#else
#define debug(var)
#define debug_addr(var)
#define debug_int(var)
#define debug_size_t(var)
#endif

int
main(int argc, char **argv)
{
	// Set Intel IA Drake backend to use NB_THREADS processors
	ia_arguments_t args;
	args.num_cores = 1;

	// Initialize Drake platform
	drake_platform_t stream = drake_platform_init(&args);

	// Create pipeline with application hello
	drake_platform_stream_create(stream, hello);

	// Initialize stream
	drake_platform_stream_init(stream, NULL);

	// Run the pipeline
	drake_platform_stream_run(stream);

	// Clenup pipeline
	drake_platform_stream_destroy(stream);

	// Cleanup backend
	drake_platform_destroy(stream);

	return EXIT_SUCCESS;
}

