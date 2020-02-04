//
// Created by hiroo on 19/09/11.
//

#ifndef JSONPARSE_PARSER_H
#define JSONPARSE_PARSER_H

#include <fstream>

#include <jsoncpp/json/json.h>
class parser {
public:
    parser();
    int read(const std::string path);
    int save(const std::string path);
    int getPropValue(std::string prop, std::string &value);
    int getPropValues(std::map<std::string, std::string> &values);
    int setPropValue(std::string prop, std::string value);
    int setPropValues(std::map<std::string, std::string> &values);
    int setCameraInfo(std::map<std::string, std::string> &value);
    int newTree();
private:
    Json::Value root;
};


#endif //JSONPARSE_PARSER_H
