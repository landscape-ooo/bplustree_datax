/*
 * test.iniconfig.cc
 *
 *  Created on: Aug 23, 2016
 *      Author: a11
 */
#include <iostream>
#include "common/INIReader.h"

int main()
{
    INIReader reader("/usr/local/webserver/fdfs/etc/storage.conf");

    if (reader.ParseError() < 0) {
        std::cout << "Can't load 'test.ini'\n";
        return 1;
    }
    std::cout << "Config loaded from 'test.ini': version="
              << reader.Get("", "key_namespace", "UNKNOWN") << ", name="
              << reader.Get("", "http.disabled", "UNKNOWN") << ", email="
              << reader.Get("", "store_path0", "UNKNOWN")<< ", pi="
              << reader.Get("", "store_path1", "UNKNOWN") << ", active="
              << reader.Get("", "allow_hosts", "UNKNOWN")<< "\n";
    return 0;
}
