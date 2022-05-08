#ifndef CACHE_H
#define CACHE_H

#include <string>

namespace database
{
    class Cache
    {
        private:
            Cache();

        public:
            static Cache get();
            void put(long id, const std::string& val);
            bool get(int id, std::string& val);
            size_t size();
            void remove(long id);
            void remove_all();
    };
}

#endif