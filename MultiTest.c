#include "Multi.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
	MultiCreate("224.0.0.1", 2021, "Emma", "Emma");
	MultiCreate("224.0.0.2", 2021, "Hen", "Hen");
	sleep(5);
	MultiDestroy("Emma", "Emma");
	sleep(5);
	MultiDestroy("Hen", "Hen");
	return 0;
}
