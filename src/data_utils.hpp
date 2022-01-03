/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_DATA_UTILS_HPP
#define CLARA_DATA_UTILS_HPP

#include "constants.hpp"

#include <clara/msg/message.hpp>

#include <sstream>
#include <stdexcept>
#include <string>

namespace clara::util {

auto build_request(const msg::Topic& topic, std::string_view data) -> msg::Message;

auto parse_message(const msg::Message& msg) -> std::string;


class InvalidRequest : public std::logic_error
{
public:
    InvalidRequest(const std::string& msg) : std::logic_error{msg} {}

    InvalidRequest(const char* msg) : std::logic_error{msg} {}
};


class RequestParser
{
public:
    static auto build(const msg::Message& msg) -> RequestParser
    {
        const auto& mt = msg.datatype();
        if (mt == "text/string") {
            return {msg.meta(), parse_message(msg)};
        }
        throw InvalidRequest{"invalid mime-type = " + mt};
    }

private:
    RequestParser(const msg::proto::Meta* meta, std::string&& data)
      : meta_{meta}
      , data_{std::move(data)}
      , ss_{data_}
    {
        // nop
    }

public:
    RequestParser(const RequestParser& other)
      : meta_{other.meta_}
      , data_{other.data_}
      , ss_{data_}
    {
        // nop
    }

    RequestParser(RequestParser&& other) noexcept
      : meta_{other.meta_}
      , data_{std::move(other.data_)}
      , ss_{data_}
    {
        other.ss_.clear();
    }

public:
    auto next_string() -> std::string
    {
        auto item = std::string{};
        if (std::getline(ss_, item, constants::data_sep[0])) {
            if (item.empty()) {
                throw invalid_request();
            }
            return item;
        }
        throw invalid_request();
    }

    auto next_string(std::string_view default_value) -> std::string
    {
        auto item = std::string{};
        if (std::getline(ss_, item, constants::data_sep[0])) {
            if (!item.empty()) {
                return item;
            }
        }
        item = std::string{default_value};
        return item;
    }

    auto next_integer() -> int
    {
        try {
            return std::stoi(next_string());
        } catch (const std::out_of_range& e) {
            throw invalid_request();
        } catch (const std::invalid_argument& e) {
            throw invalid_request();
        }
    }

    auto request() const -> const std::string&
    {
        return data_;
    }

private:
    auto invalid_request() -> InvalidRequest
    {
        std::string msg = "invalid request";
        if (meta_->has_author()) {
            msg += " from author = " + meta_->author();
        }
        msg += ": " + data_;
        return InvalidRequest{msg};
    }

private:
    const msg::proto::Meta* meta_;
    std::string data_;
    std::stringstream ss_;
};


} // end namespace clara::util

#endif // end of include guard: CLARA_DATA_UTILS_HPP
