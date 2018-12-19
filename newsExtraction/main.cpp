#include "configuration.h"

Configuration config;

int main()
{
	// first load all configuration and use them
	config.load();

	// then start to do segmentation and query
	config.process();

	return 0;
}
