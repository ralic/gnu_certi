// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// $Id: main.cc,v 3.4 2003/10/20 09:34:55 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>

#include "Billard.hh"
#include "BillardDDM.hh"
#include "Display.hh"
#include "Ball.hh"
#include "ColoredBall.hh"
#include "Fed.hh"
#include "PrettyDebug.hh"

#include "cmdline.h"

#include "graph_c.hh"
#include "RTI.hh"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <iostream>
#include <signal.h>
#include <exception>

using std::string ;
using std::cout ;
using std::endl ;
using std::cerr ;

static pdCDebug D("BILLARD", __FILE__);

extern "C" void sortir(int SignalNumber);
void ExceptionHandler();
void TerminateHandler();

#ifdef HAVE_XML
static const bool WITH_XML = true ;
#else
static const bool WITH_XML = false ;
#endif

static bool exit_billard = false ;

Billard *createBillard(bool, const char *, string);

// ----------------------------------------------------------------------------
/** Test program entry point.
 */
int
main(int argc, char **argv)
{
    cout << "CERTI Billard " VERSION << endl << endl ;

    try {
	// Handlers
	signal(SIGINT, sortir);
	signal(SIGALRM, sortir);
	std::set_terminate(TerminateHandler);
	std::set_unexpected(ExceptionHandler);

	// Command line
	gengetopt_args_info args ;
	if (cmdline_parser(argc, argv, &args)) exit(EXIT_FAILURE);

	bool verbose = args.verbose_flag ;

	// Federation and .fed names
	string federation = args.federation_arg ;
	string federate = args.name_arg ;
	string fedfile = args.federation_arg + WITH_XML ? ".xml" : ".fed" ;

	// Create billard
	Billard *billard = createBillard(args.demo_given, args.demo_arg, 
					 federate);
	billard->setVerbose(verbose);

	int timer = args.timer_given ? args.timer_arg : 0 ;
	int delay = args.delay_given ? args.delay_arg : 0 ;
	int autostart = args.auto_given ? args.auto_arg : 0 ;

	// Joins federation
	D[pdDebug] << "Create or join federation" << endl ;
	billard->join(federation, fedfile);
	FederateHandle handle = billard->getHandle();

	// Display...
	Display *display = Display::instance();
	int y_default = 10 + (handle - 1) * (display->getHeight() + 20);
	display->setWindow(
	    args.xoffset_given ? args.xoffset_arg : 10,
	    args.yoffset_given ? args.yoffset_arg : y_default);

	// Continue initialisation...
	D[pdDebug] << "Synchronization" << endl ;
	billard->pause();
	D[pdDebug] << "Publish and subscribe" << endl ;
	billard->publishAndSubscribe();
	display->show();

	if (args.coordinated_flag) {
	    billard->setTimeRegulation(true, true);
	    billard->tick();
	}
	billard->synchronize(autostart);

	// Countdown
	struct sigaction a ;
	a.sa_handler = sortir ;
	sigemptyset(&a.sa_mask);
	sigaction(SIGALRM, &a, NULL);

	// set timer
	if (timer != 0) {
	    printf("Timer ... : %5d\n", timer);
	    alarm(timer);
	}

	// Create object
	if (args.initx_given && args.inity_given) {
	    billard->init(args.initx_arg, args.inity_arg);
	}
	else {
	    billard->init(handle);
	}

	// registers objects, regions, etc.
	billard->declare();

	cout << "Declaration done." << endl ;

	// set delay
	if (delay != 0) {
	    while (delay >= 0) {
		sleep(1);
		printf("\rDelay : %5d", delay);
		fflush(stdout);
		delay-- ;
	    }
	    printf("\n");
	}

	// Simlation loop
	while (!exit_billard) {
	    billard->step();
	}

	// End of simulation
	D.Out(pdTrace, "End of simulation loop.");
	billard->resign();

	delete billard ;
	delete display ;
    }
    catch (Exception &e) {
	cerr << "Billard: exception: " << e._name << " [" 
	     << (e._reason ? e._reason : "undefined") << "]" << endl ;
    }

    cout << "Exiting." << endl ;
}

// ----------------------------------------------------------------------------
/** Signal handler.
 */
void
sortir(int number)
{
    switch (number) {
      case SIGALRM: {
	  D.Out(pdTerm, "Alarm signal received, exiting...");
	  exit_billard = true ;
      } break ;
      case SIGINT: {
	  cout << "Exit request received" << endl ;
	  exit_billard = true ;
      } break ;
      default: {
	  D.Out(pdTerm, "Emergency stop, destroying Ambassadors.");
	  D.Out(pdTerm, "Federate terminated.");
	  exit(EXIT_FAILURE);
      } 
    }
}

// ----------------------------------------------------------------------------
/** ExceptionHandler.
 */
void
ExceptionHandler()
{
    cerr << "Billard: unexpected exception." << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
/** TerminateHandler.
 */
void
TerminateHandler()
{
    cerr << "Billard: unknown exception." << endl ;
    exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------
/** createBillard
 */
Billard *
createBillard(bool demo, const char *s_demo, string name)
{
    if (demo) {
	if (!strcmp(s_demo, "DDM")) return new BillardDDM(name);
	cout << "unknown keyword: " << s_demo << endl ;
    }
    
    return new Billard(name);
}

// EOF $Id: main.cc,v 3.4 2003/10/20 09:34:55 breholee Exp $