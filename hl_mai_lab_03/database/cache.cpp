#include "cache.h"
#include "../config/config.h"


#include <ignite/thin/ignite_client.h>
#include <ignite/thin/ignite_client_configuration.h>
#include <ignite/thin/cache/cache_peek_mode.h>
#include <exception>

static ignite::thin::IgniteClient _client;

namespace database
{
    static std::map<Cache::CacheType, ignite::thin::cache::CacheClient<long, std::string>> _caches;

    Cache::Cache()
    {
        ignite::thin::IgniteClientConfiguration cfg;
        cfg.SetEndPoints(Config::get().get_cache_servers());
        cfg.SetPartitionAwareness(true);

        try
        {
            _client = ignite::thin::IgniteClient::Start(cfg);

            _caches.insert( std::pair<CacheType,ignite::thin::cache::CacheClient<long, std::string>>(CacheType::authors, _client.GetOrCreateCache<long, std::string>("authors")) );
            _caches.insert( std::pair<CacheType,ignite::thin::cache::CacheClient<long, std::string>>(CacheType::persons, _client.GetOrCreateCache<long, std::string>("persons")) );

        }
        catch (ignite::IgniteError* err)
        {
            std::cout << "error:" << err->what() << std::endl;
            throw;
        }
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    void Cache::put(Cache::CacheType cashType, long id, const std::string& val) {
        auto cash_iterator = _caches.find(cashType);
        if (cash_iterator == _caches.end()) {
            throw std::logic_error("key not found in cache list");
        } else {
            cash_iterator->second.Put(id,val);
        } 
    } 

    void Cache::remove(Cache::CacheType cashType, long id){
        auto cash_iterator = _caches.find(cashType);
        if (cash_iterator == _caches.end()) {
            throw std::logic_error("key not found in cache list");
        } else {
            cash_iterator->second.Remove(id);
        } 
    }

    size_t Cache::size(Cache::CacheType cashType){
        auto cash_iterator = _caches.find(cashType);
        if (cash_iterator == _caches.end()) {
            throw std::logic_error("key not found in cache list");
        } else {
            return cash_iterator->second.GetSize(ignite::thin::cache::CachePeekMode::ALL);
        } 
    }

    void Cache::remove_all(Cache::CacheType cashType) {

        auto cash_iterator = _caches.find(cashType);
        if (cash_iterator == _caches.end()) {
            throw std::logic_error("key not found in cache list");
        } else {
            cash_iterator->second.RemoveAll();
        }        
    }

    bool Cache::get(Cache::CacheType cashType, long id, std::string& val){
        
        try {
            auto cash_iterator = _caches.find(cashType);
            if (cash_iterator == _caches.end()) {
                throw std::logic_error("key not found in cache");
            } else {
                val = cash_iterator->second.Get(id);
                std::cout << val << std::endl;
                return true;
            }
        } catch(...){
            throw std::logic_error("key not found in cache");
        }
    }
}