#ifndef _FILE_PARSER_H
#define _FILE_PARSER_H


#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>

static std::vector<std::pair<std::string, int> >
parse_file(std::string filename)
{
  std::ifstream file(filename.c_str());
  std::string line;
  std::vector<std::pair<std::string, int> > pairs;
 
  while (std::getline(file, line)) { // gets one line
    if (line[0] == '#' || line[0] == ' ')
      continue;
  
    std::stringstream one_line(line);
    std::string col1;
    std::string col2;

    std::getline(one_line, col1, ','); // gets first col
    std::getline(one_line, col2, ','); // gets second col
    
    pairs.push_back(std::pair<std::string, int>(col1, atoi(col2.c_str())));
  }
  
  return pairs;
}

#endif
