/*
 * s3fs - FUSE-based file system backed by Tencentyun COS
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef S3FS_CACHE_H_
#define S3FS_CACHE_H_

#include "common.h"

//
// Struct
//
struct stat_cache_entry {
  struct stat   stbuf;
  unsigned long hit_count;
  time_t        cache_date;
  headers_t     meta;
  bool          isforce;
  bool          noobjcache;  // Flag: cache is no object for no listing.

  stat_cache_entry() : hit_count(0), cache_date(0), isforce(false), noobjcache(false) {
    memset(&stbuf, 0, sizeof(struct stat));
    meta.clear();
  }
};

typedef std::map<std::string, stat_cache_entry*> stat_cache_t; // key=path

//
// Class
//
class StatCache
{
  private:
    static StatCache       singleton;
    static pthread_mutex_t stat_cache_lock;
    stat_cache_t  stat_cache;
    bool          IsExpireTime;
    time_t        ExpireTime;
    unsigned long CacheSize;
    bool          IsCacheNoObject;

  private:
    void Clear(void);
    bool GetStat(std::string& key, struct stat* pst, headers_t* meta, bool overcheck, const char* petag, bool* pisforce);
    // Truncate stat cache
    bool TruncateCache(void);

  public:
    StatCache();
    ~StatCache();

    // Reference singleton
    static StatCache* getStatCacheData(void) {
      return &singleton;
    }

    // Attribute
    unsigned long GetCacheSize(void) const;
    unsigned long SetCacheSize(unsigned long size);
    time_t GetExpireTime(void) const;
    time_t SetExpireTime(time_t expire);
    time_t UnsetExpireTime(void);
    bool SetCacheNoObject(bool flag);
    bool EnableCacheNoObject(void) {
      return SetCacheNoObject(true);
    }
    bool DisableCacheNoObject(void) {
      return SetCacheNoObject(false);
    }
    bool GetCacheNoObject(void) const {
      return IsCacheNoObject;
    }

    // Get stat cache
    bool GetStat(std::string& key, struct stat* pst, headers_t* meta, bool overcheck = true, bool* pisforce = NULL) {
      return GetStat(key, pst, meta, overcheck, NULL, pisforce);
    }
    bool GetStat(std::string& key, struct stat* pst, bool overcheck = true) {
      return GetStat(key, pst, NULL, overcheck, NULL, NULL);
    }
    bool GetStat(std::string& key, headers_t* meta, bool overcheck = true) {
      return GetStat(key, NULL, meta, overcheck, NULL, NULL);
    }
    bool HasStat(std::string& key, bool overcheck = true) {
      return GetStat(key, NULL, NULL, overcheck, NULL, NULL);
    }
    bool HasStat(std::string& key, const char* etag, bool overcheck = true) {
      return GetStat(key, NULL, NULL, overcheck, etag, NULL);
    }

    // Cache For no object
    bool IsNoObjectCache(std::string& key, bool overcheck = true);
    bool AddNoObjectCache(std::string& key);

    // Add stat cache
    bool AddStat(std::string& key, headers_t& meta, bool forcedir = false);

	bool IncSize(const std::string& key, ssize_t sz);

    // Delete stat cache
    bool DelStat(const char* key);
    bool DelStat(std::string& key) {
      return DelStat(key.c_str());
    }
};

//
// Functions
//
bool convert_header_to_stat(const char* path, headers_t& meta, struct stat* pst, bool forcedir = false);

#endif // S3FS_CACHE_H_

/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: noet sw=4 ts=4 fdm=marker
* vim<600: noet sw=4 ts=4
*/
