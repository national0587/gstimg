//
// Created by Inoko on 19/09/11.
//

#include <iostream>
#include "parser.h"

parser::parser() {
//    std::cout << "out" << std::endl;
}

int parser::read(const std::string path) {
    bool alive = true;
    Json::Reader reader;
    while(alive){
        std::ifstream jsonfile(path, std::fstream::binary);
        bool parsingSuccessful = reader.parse(jsonfile, root, false);
        if (!parsingSuccessful) {
            std::cout << reader.getFormatedErrorMessages()
                      << "\n";
            return false;
        }
        alive = false;
    }
    return true;
}

int parser::getPropValue(std::string prop, std::string &value) {
    if(root.type()==Json::objectValue){
        std::vector<std::string> rootlist = root.getMemberNames();
        for(auto itr = rootlist.begin(); itr!=rootlist.end(); ++itr){
            if (*itr=="property_list"){
                Json::Value propv = root[*itr];
                std::vector<std::string> proplist = propv.getMemberNames();
                for(auto itr2=proplist.begin(); itr2!=proplist.end();++itr2){
                    if(*itr2==prop){
                        value = propv[prop].get("value", "UTF-8").asString();
                        return true;

                    }
                }
            }
        }
    }
    return false;
}
int parser::getPropValues(std::map<std::string, std::string> &values) {
    if(root.type()==Json::objectValue){
        std::vector<std::string> rootlist = root.getMemberNames();
        for(auto itr = rootlist.begin(); itr!=rootlist.end(); ++itr){
            if (*itr=="property_list"){
                Json::Value propv = root[*itr];
                std::vector<std::string> proplist = propv.getMemberNames();
                for(auto itr2=proplist.begin(); itr2!=proplist.end();++itr2){
                    std::string value = propv[*itr2].get("value", "UTF-8").asString();
                    values.insert(std::make_pair(*itr2, value));

                }
            return true;
            }
        }
    }
    return false;
}

int parser::setPropValue(std::string prop, std::string value) {

    return 0;
}

int parser::newTree() {
    /*よくわからんので使用禁止*/
    if (root != nullptr){
        root.clear();
    }
    return 0;
}

int parser::setPropValues(std::map<std::string, std::string> &values) {
//    Json::Value root;
    root.clear();

//    root["file_type"] = "ttc_camera_plugin_setting";
//    root["file_version"] = "1.0.0";
//    root["camera_info"]["model"] = "BU1203MC";
////    root["camera_info"]["SerialNo"] = "0001";
//    root["camera_info"]["manufacture"] = "Toshiba-Teli";
//    root["plugin_info"]["name"] = "telibu1203mcsrc";
//    root["plugin_info"]["version"] = "0.0.1";

    for(auto x: values){
        root["property_list"][x.first]["value"] = x.second;
//        std::cout << x.first << std::endl;
    }

    return true;
}

int parser::setCameraInfo(std::map<std::string, std::string> &value){
//    root.clear();

    root["file_type"] = "ttc_camera_plugin_setting";
    root["file_version"] = "1.0.0";
    root["camera_info"]["model"] = "BU1203MC";
//    root["camera_info"]["SerialNo"] = "0001";
    root["camera_info"]["serialNo"] = value["serialNo"];
    root["camera_info"]["manufacturer"] = "Toshiba-Teli";
    root["plugin_info"]["name"] = "telibu1203mcsrc";
    root["plugin_info"]["version"] = "0.0.2";
}

int parser::save(const std::string path) {
    std::ofstream jsonfile(path, std::ofstream::out);
    std::cout << root << std::endl;
    Json::StyledWriter styledWriter;
    jsonfile << styledWriter.write(root) ;
    jsonfile.close();
    return true;
}

