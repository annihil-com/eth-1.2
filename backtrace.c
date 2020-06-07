// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

#define _GNU_SOURCE

#include <link.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <execinfo.h>

#include "eth.h"
#include "ghf/ghf.h"

int dlIterateCallbackSig(struct dl_phdr_info *info, size_t size, void *data) {
	if (strlen(info->dlpi_name))
		fprintf(stderr, "[0x%x:0x%x] %s", info->dlpi_addr, info->dlpi_addr + info->dlpi_phdr->p_memsz, info->dlpi_name);
	if (strstr(info->dlpi_name, "libETH.so"))
		fprintf(stderr, " [0x%x]", (int)crc32FromFile(info->dlpi_name));
	fprintf(stderr, "\n");
	return 0;
}

void signalHandler(int signum) {
	static int sigCounter = 0;
	sigCounter++;
	if (sigCounter > 1) {
		fprintf(stderr, "Double error\n");
		fprintf(stderr, "Signal %d: %s\n", signum, strsignal(signum));
		exit(1);
	}

	fprintf(stderr, "----- Start of eth backtrace -----\n");
	fprintf(stderr, "eth: " ETH_VERSION "\n");
	fprintf(stderr, "mod: ");
	if (eth.mod && eth.mod->name && eth.mod->version)
		fprintf(stderr, "%s %s\n", eth.mod->name, eth.mod->version);
	else
		fprintf(stderr, "no found\n");
	fprintf(stderr, "Signal %d: %s\n", signum, strsignal(signum));
	fprintf(stderr, "-- Maps --\n");
	dl_iterate_phdr(dlIterateCallbackSig, NULL);

	struct sigcontext *p = (struct sigcontext *) (((char *) &p) + 16);    
	fprintf(stderr, "-- Registers --\n");
	fprintf(stderr, "[ eax='0x%08lx' | ebx='0x%08lx' ]\n", p->eax, p->ebx);
	fprintf(stderr, "[ ecx='0x%08lx' | edx='0x%08lx' ]\n", p->ecx, p->edx);
	fprintf(stderr, "[ eip='0x%08lx' | ebp='0x%08lx' ]\n", p->eip, p->ebp);

    fprintf(stderr, "-- Backtrace --\n");

    void *frame[32];
    char **symbols;
    int symbolsNumber;

    symbolsNumber = backtrace(frame, (sizeof(frame) / sizeof(void)));
    symbols = backtrace_symbols(frame, symbolsNumber);

	#define NUMBER_OF_CALLS 2
	int count = NUMBER_OF_CALLS;
    for (; count < symbolsNumber; count++)
		fprintf(stderr, "[sf:%d]: from %s\n", count - NUMBER_OF_CALLS, symbols[count]);

	free(symbols);

	fprintf(stderr, "----- End of eth backtrace -----\n");

	fflush(stderr);

	usleep(1000);
	exit(1);
}

// For disable all custom signal handler
__attribute__ ((visibility ("default")))
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	return 0;
}

// Overwrite signal handler
__attribute__ ((visibility ("default")))
sighandler_t signal(int signum, sighandler_t handler) {
	static sighandler_t (*orig_signal)(int, sighandler_t) = NULL;
	if (!orig_signal)
		orig_signal = dlsym(RTLD_NEXT, "signal");

	if ((signum == SIGILL) || (signum == SIGABRT) || (signum == SIGFPE) || (signum == SIGSEGV)){
		handler = signalHandler;
	}

	return orig_signal(signum, handler);
}
