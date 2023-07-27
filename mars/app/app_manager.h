//
// Created by Changpeng Pan on 2022/8/29.
//

#ifndef MMNET_APP_MANAGER_H
#define MMNET_APP_MANAGER_H

#include <memory>
#include <typeindex>
#include <unordered_map>
#include "mars/boost/any.hpp"

#include "mars/app/app.h"
#include "mars/boot/base_manager.h"
#include "mars/boot/context.h"
#include "mars/comm/comm_data.h"
#include "mars/comm/thread/lock.h"
#include "mars/comm/thread/mutex.h"
#include "mars/comm/thread/thread.h"
#include "mars/comm/time_utils.h"
#include "mars/comm/alarm.h"

namespace mars {
namespace app {

class AppManager : public mars::boot::BaseManager {
 public:
    explicit AppManager(mars::boot::Context* context);
    ~AppManager() override;
    std::string GetName() override;

 private:
    mars::boot::Context* context_;

 public:
    void SetCallback(Callback* callback);
    mars::comm::ProxyInfo GetProxyInfo(const std::string& _host);
    std::string GetAppFilePath();
    AccountInfo GetAccountInfo();
    std::string GetUserName();
    std::string GetRecentUserName();
    unsigned int GetClientVersion();
    DeviceInfo GetDeviceInfo();

 public:
    void GetProxyInfo(const std::string& _host, uint64_t _timetick);
    //    #if TARGET_OS_IPHONE
    void ClearProxyInfo();
    //    #endif

//    template <typename T>
//    T GetConfig(const std::string& key, T default_value) ;
//
//    template <typename T>
//    void SetConfig(const std::string& key, T value);

    template <typename T>
    T GetConfig(const std::string& key, T default_value) {
        xinfo2(TSF "AppConfig GetConfig key:%_, default value:%_", key, default_value);
        auto it = _config.find(key);
        if (it == _config.end() || _types.at(key) != typeid(T).name()) {
            xinfo2(TSF"AppConfig GetConfig return default value. ");
            return default_value;
        }
        return boost::any_cast<T>(it->second);
    }

    template <typename T>
    void SetConfig(const std::string& key, T value) {
        xinfo2(TSF "AppConfig SetConfig key:%_, value:%_", key, value);
        _config[key] = value;
        _types[key] = typeid(T).name();
        CheckCommSetting(key);
    }

    void CheckCommSetting(const std::string& key);

 private:
    Callback* callback_;
    mars::comm::ProxyInfo proxy_info_;
    bool got_proxy_ = false;
    mars::comm::Mutex slproxymutex_;
    mars::comm::Thread slproxythread_;
    uint64_t slproxytimetick_ = gettickcount();
    int slproxycount_ = 0;

    std::unordered_map<std::string, boost::any> _config;
    std::unordered_map<std::string, std::string> _types;

};

}  // namespace app
}  // namespace mars

#endif  // MMNET_APP_MANAGER_H
