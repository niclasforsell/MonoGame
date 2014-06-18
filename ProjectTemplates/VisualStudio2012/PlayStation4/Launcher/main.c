#include <stdio.h>

//#include <mono/mini/jit.h>
//#include <mono/metadata/mono-debug.h>
//#define HAS_MONO_IMPORTS

#if _DEBUG
#include "aot/Debug/mono-aot.h"
#else
#include "aot/Release/mono-aot.h"
#endif

unsigned int sceLibcHeapDelayedAlloc = 1;
unsigned int sceLibcHeapExtentededAlloc = 1;
size_t sceLibcHeapSize = 200 * 1024 * 1024; //0xffffffffffffffffUL; // SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT
size_t sceLibcHeapInitialSize = 200 * 1024 * 1024; // Initial heap is 200Mb

/*
static void debugger_setup()
{
	char *args[1];
	char buff[1000];
	int loglevel = 10;
	const char *host = "192.168.0.108";
	int port = 9000;
	int server = 0;

	sprintf (buff,
		"--debugger-agent=transport=dt_socket,loglevel=%d,address=%s:%d,%sembedding=1", 
		loglevel, 
		host,
		port,
		server ? "server=y," : "");

	args [0] = buff;
	mono_jit_parse_options(1, args);
	mono_debug_init(MONO_DEBUG_FORMAT_MONO);
}
*/

int main (int argc, char *argv[])
{
	MonoAssembly *assembly;

	g_setenv("MONO_USE_DIRECT_MEMORY", "1", 1);
	g_setenv("MONO_PATH", "/app0/", 1); /*FIXME this doesn't affect the location in main_assembly_name */
	//g_setenv ("MONO_LOG_LEVEL", "debug", 1);
	g_setenv("MONO_GC_PARAMS", "nursery-size=1m", 1);
	//g_setenv ("MONO_GC_DEBUG", "2", 1);
#if _DEBUG
	//g_setenv("MONO_IGNORE_SUSPEND_TIMEOUT", "1", 1);
#endif

	orbis_setup();
	orbis_register_modules();
	mono_jit_set_aot_only(1);

	//debugger_setup();

	mono_jit_init_version ("ORBIS", "v2.0.50727");
	assembly = mono_assembly_open(main_assembly_name, NULL);
	mono_jit_exec(mono_domain_get(), assembly, argc, argv);

	return 0;
}