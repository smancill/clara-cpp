/*
 * SPDX-FileCopyrightText: © The Clara Framework Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CLARA_STD_EVENT_WRITER_HPP
#define CLARA_STD_EVENT_WRITER_HPP

#include <clara/engine.hpp>
#include <clara/third_party/json11.hpp>

#include <memory>
#include <stdexcept>

namespace clara::stdlib {

/**
 * An abstract writer service that writes all received events into the
 * configured output file.
 */
class EventWriterService : public Engine
{
public:
    EventWriterService();
     ~EventWriterService() override;

public:
    auto configure(EngineData& input) -> EngineData override;

    auto execute(EngineData& input) -> EngineData override;

    auto execute_group(const std::vector<EngineData>& inputs) -> EngineData override;

protected:
    /**
     * A problem in the event writer implementation.
     */
    class EventWriterError : public std::runtime_error
    {
    public:
        EventWriterError(const char* msg)
          : std::runtime_error{msg}
        {}

        EventWriterError(const std::string& msg)
          : std::runtime_error{msg}
        {}
    };

    enum class Endian
    {
        Little,
        Big,
    };

    static auto parse_byte_order(const json11::Json& opts) -> Endian;

private:
    /**
     * Creates a new writer and opens the given output file.
     *
     * @param file the path to the output file
     * @param opts extra options for the writer
     * @throws EventWriterError if the file could not be opened
     */
    virtual void open_file(const std::string& file,
                           const json11::Json& opts) = 0;

    /**
     * Closes the output file.
     */
    virtual void close_file() = 0;

    /**
     * Returns true if an output file is open.
     */
    virtual auto has_file() -> bool = 0;

    /**
     * Writes an event to the output file.
     * The event should be a C++ object with the same type as the one defined
     * by the Clara engine data-type returned by {@link #get_data_type()}.
     *
     * @param event the event to be written
     * @throws EventWriterError if the file could not be read
     */
    virtual void write_event(const std::any& event) = 0;

    /**
     * Gets the Clara engine data-type for the type of the events.
     * The data-type will be used to deserialize the events when the engine data
     * is received from services across the network.
     *
     * @return the data-type of the events
     */
    virtual auto get_data_type() const -> const EngineDataType& = 0;

public:
    auto input_data_types() const -> std::vector<EngineDataType> override;

    auto output_data_types() const -> std::vector<EngineDataType> override;

    auto states() const -> std::set<std::string> override;

public:
    void reset() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // end namespace clara::stdlib

#endif // end of include guard: CLARA_STD_EVENT_WRITER_HPP
