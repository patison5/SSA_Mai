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
            // enum class CacheType;

            enum class CacheType { 
                authors = 1, 
                persons
            };

            static Cache get();
            void put(Cache::CacheType cashType, long id, const std::string& val);
            bool get(Cache::CacheType cashType, long id, std::string& val);
            size_t size(Cache::CacheType cashType);
            void remove(Cache::CacheType cashType, long id);
            void remove_all(Cache::CacheType cashType);
    };
}

#endif