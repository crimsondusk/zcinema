#ifndef DEMO_H
#define DEMO_H
#include "types.h"

enum {
	DemoLength,
	DemoVersion,
	DemoCVars,
	DemoUserInfo,
	DemoBodyStart,
	DemoTiccmd,
	DemoInvUse,
	DemoCenterView,
	DemoTaunt,
	DemoEnd,
	DemoWads
};

int launchDemo( str path );

#endif // DEMO_H