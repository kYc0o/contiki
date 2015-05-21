
#include "rtkev.h"

#include <stdio.h>


static int
getDeployUnit00(const char* deployUnitName)
{
	printf("__012345 %s 543210__\n", deployUnitName);
	return 0;
}

static int
my_init()
{
	return 0;
}

const DeployUnitRetriver shellBasedRetriever = {
	.init = my_init,
	.getDeployUnit = getDeployUnit00
}; 
