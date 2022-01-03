/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_CONCURRENT_MAP_HPP
#define CLARA_CONCURRENT_MAP_HPP

#include <memory>
#include <mutex>
#include <unordered_map>

namespace clara::util {

template <typename K, typename T>
class ConcurrentMap
{
public:
    using key_type = K;
    using element_type = T;
    using mapped_type = std::shared_ptr<element_type>;

public:
    ConcurrentMap() = default;

    ConcurrentMap(const ConcurrentMap&) = delete;
    ConcurrentMap(ConcurrentMap&&) = delete;

    ~ConcurrentMap() = default;

    template <typename... Args>
    auto insert(const key_type& name, Args&&... args) -> mapped_type
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto data = std::make_shared<T>(std::forward<Args>(args)...);
        auto [val, ins] = cont_.insert({name, data});
        if (ins) {
            return val->second;
        }
        return nullptr;
    }

    auto find(const key_type& name) -> mapped_type
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto it = cont_.find(name);
        if (it != cont_.end()) {
            return it->second;
        }
        return nullptr;
    }

    auto remove(const key_type& name) -> mapped_type
    {
        std::unique_lock<std::mutex> lock{mutex_};

        auto it = cont_.find(name);
        if (it == cont_.end()) {
            return nullptr;
        }
        auto c = std::move(it->second);
        cont_.erase(it);
        return c;
    }

    template <typename F>
    void for_each(F f)
    {
        for (auto& x : cont_) {
            f(x.second.get());
        }
    }

    void clear()
    {
        std::unique_lock<std::mutex> lock{mutex_};
        cont_.clear();
    }

private:
    std::mutex mutex_;
    std::unordered_map<key_type, mapped_type> cont_;
};

} // end namespace clara::util

#endif // end of include guard: CLARA_CONCURRENT_MAP_HPP
