
#ifndef __UTILS_H
#define __UTILS_H
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
using namespace std;

vector<string> split_string(string line,string delim) {
    string str=line;
    vector<string> res;
    size_t found;
    found=str.find(delim);
    while(found!=string::npos) {
	string substr=str.substr(0,found);
	res.push_back(substr);
	str=str.substr(found+1,str.size()-found);
	found=str.find(delim);
    }
    res.push_back(str);
    return res;
}

string toString(int k) {
   std::ostringstream out;
   out << k;
   return out.str();
}


void writeProgress (const char *what, int value, int max) {
  const char *symbols = "\\|/-";
  const char *sym = "";

  if (*sym == '\0')
    sym = symbols;

  if (max)
    fprintf (stdout, "%s: %c Processed: %d (%d%%)   \r", what, *sym++, value, 100 * value / max);
  else
    fprintf (stdout, "%s: %c Processed: %d          \r", what, *sym++, value);

  fflush (stdout);
}



#endif  // __UTILS_H




