/*
*  Config.cpp
*  TWS
*
*  Created by Filipe Varela on 08/11/05.
*  Copyright 2008 Filipe Varela. All rights reserved.
*
*/

#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include <Config.h>

namespace TWS {
    std::string trim(std::string const& source, char const* delims = " \t\r\n") {
        std::string result(source);
        std::string::size_type index = result.find_last_not_of(delims);
        if(index != std::string::npos)
            result.erase(++index);

        index = result.find_first_not_of(delims);
        if (index != std::string::npos) result.erase(0, index);
        else result.erase();
        return result;
    }

    Config::Config(std::string const& configFile) {
        std::ifstream file(configFile.c_str());

        std::string line;
        std::string name;
        std::string value;
        std::string inSection;
        
        while (std::getline(file, line)) {
            if (!line.length()) continue;
            if (line[0] == '#') continue;
            if (line[0] == ';') continue;
            if (line[0] == '[') {
                inSection = trim(line.substr(1, line.find(']')-1));
                continue;
            }

            int posEqual = line.find('=');
            name  = trim(line.substr(0, posEqual));
            value = trim(line.substr(posEqual+1));
            _config[inSection+'/'+name] = value;
        }
    }

    std::string const& Config::Value(std::string const& section, std::string const& entry) const {
        std::map<std::string,std::string>::const_iterator ci = _config.find(section + '/' + entry);
        if (ci == _config.end()) {
            std::cerr << "TWS::Config entry \"" << section << "/" << entry << "\" does not exist" << std::endl;
            std::cerr << "TWS::Config unsafe to proceed - quitting" << std::endl;
            exit(1);
        }
        return ci->second;
    }

    std::string const& Config::Value(std::string const& section, std::string const& entry, std::string const& value) {
        try {
            return Value(section, entry);
        } catch(const char *) {
            return _config.insert(std::make_pair(section+'/'+entry, value)).first->second;
        }
    }
}
