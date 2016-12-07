/*
 * CpuCount.h
 *
 *  Created on: Jun 7, 2011
 *      Author: david
 */

#ifndef CPUCOUNT_H_
#define CPUCOUNT_H_

#include <cstdio>

namespace Danvil
{
	inline unsigned int CpuCount() {
		FILE* fp = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
		char res[128];
		size_t read_count = fread(res, 1, sizeof(res)-1, fp);
		fclose(fp);
		if(read_count > 0) {
			long int n = strtol(res, NULL, 0);
			if(n > 0) {
				return n;
			}
		}
		return 1;
	}
}

#endif
