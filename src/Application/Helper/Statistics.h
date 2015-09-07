#ifndef OPENMAKAENGINE_STATISTICS_H
#define OPENMAKAENGINE_STATISTICS_H

#include <iostream>
#include <map>

namespace om {

    class Statistics {

    private:

        std::map<std::string, std::string> stats;
        bool wroteHeader;

        static const std::string values[];

    public:

        Statistics(void);

        virtual ~Statistics(void);

        void add(std::string key, std::string value);

        void write(std::string filename);

        void reset();

        void setDefaults();
    };

};

#endif
