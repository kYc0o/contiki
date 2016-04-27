
#include "rtkev.h"

#include <stdio.h>


static int
getDeployUnit00(const char* deployUnitName)
{
	printf("__012345 %s 543210__\n", deployUnitName);
	return 0;
}

const DeployUnitRetriver shellBasedRetriever = {
	.getDeployUnit = getDeployUnit00
}; 
