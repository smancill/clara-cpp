#ifndef CLARA_SERVICE_LOADER_HPP
#define CLARA_SERVICE_LOADER_HPP

#include <clara/engine.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <dlfcn.h>

namespace clara {

class ServiceLoader
{
public:
    ServiceLoader(const std::string& path)
    {
#ifdef __APPLE__
        auto fullPath = "lib" + path + ".dylib";
#else
        auto fullPath = "lib" + path + ".so";
#endif
        handle_ = dlopen(fullPath.c_str(), RTLD_NOW);
        if (handle_ == nullptr) {
            throw std::runtime_error("could not load '" + fullPath + "': " + dlerror());
        }
        create_service_ = get_function<create_service_fn>("create_engine");
        service_engine_ = create_service_();
    }

    ServiceLoader(ServiceLoader&& other) noexcept
      : handle_{other.handle_}
      , create_service_{other.create_service_}
      , service_engine_{std::move(other.service_engine_)}
    {
        other.handle_ = nullptr;
        other.create_service_ = nullptr;
    }

    ~ServiceLoader()
    {
        service_engine_.reset();
        if (handle_ != nullptr) {
            int result = dlclose(handle_);
            if (result != 0) {
                std::cerr << "could not unload service" << std::endl;
            }
        }
    }

    auto get() const -> Engine*
    {
        return service_engine_.get();
    }

    auto operator->() const -> Engine*
    {
        return get();
    }

private:
    using create_service_fn = std::unique_ptr<Engine> (*)();

private:
    template<typename Fn>
    auto get_function(const std::string& fn_name) -> Fn
    {
        dlerror();
        void* fn_addr = dlsym(handle_, fn_name.c_str());
        const char* error = dlerror();
        if (error != nullptr) {
            throw std::runtime_error("could not find " + fn_name + " function: " + error);
        }
        return reinterpret_cast<Fn>(fn_addr);
    }

private:
    void* handle_;
    create_service_fn create_service_;
    std::unique_ptr<Engine> service_engine_;
};

} // end namespace clara

#endif // end of include guard: CLARA_SERVICE_LOADER_HPP
