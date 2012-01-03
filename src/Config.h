/*
 *  Config.h
 *  TWS
 *
 *  Created by Filipe Varela on 08/11/05.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_CONFIG
#define _TWS_CONFIG

#include <string>
#include <map>

namespace TWS {
    class Config {
        std::map<std::string,std::string> _config;

        public:
            Config(std::string const& configFile);
            std::string const& Value(std::string const& section, std::string const& entry) const;
            std::string const& Value(std::string const& section, std::string const& entry, std::string const& value);
    };
}

#endif
