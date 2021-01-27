//
// Created by Vardan Gyurjyan on 1/23/21.
//

#ifndef CLARA_STREAMING_SERVICE_HPP
#define CLARA_STREAMING_SERVICE_HPP

#include <clara/engine.hpp>
#include <clara/third_party/json11.hpp>

#include <memory>
#include <set>
#include <stdexcept>

namespace clara {
    namespace stdlib {

        /**
         * An abstract class using TriDAS framework that reads VTP streams,
         * aggregates them, and builds events that are passed to the event
         * processing services.
         */
    class StreamingService : public clara::Engine {
        public:
        StreamingService();
        ~StreamingService() override;

        public:
            EngineData configure(EngineData &) override;

            EngineData execute(EngineData &input) override;

            EngineData execute_group(const std::vector<EngineData> &) override;

        protected:
            enum class Endian {
                Little,
                Big,
            };

            /**
             * An exception during the TriDAS operation
             */
            class StreamingServiceError : public std::runtime_error {
            public:
                explicit StreamingServiceError(const char *msg)
                        : std::runtime_error{msg} {}

                explicit StreamingServiceError(const std::string &msg)
                        : std::runtime_error{msg} {}
            };

        private:
            /**
             * Connect to a stream
             *
             * @param stream_port id
             * @param opts for the streamTriDAS
             * @throws TriDASError if connection can not be established
             */
            virtual void connect(int stream_port,
                                 const json11::Json &opts) = 0;

            /**
             * Disconnect from the stream
             */
            virtual void disconnect() = 0;

            /**
             * Reads a frame from all connected streams.
             * The event should be a C++ object with the same type as the one defined
             * by the CLARA engine data-type returned by {@link #get_data_type()}.
             * Note: "binary/data-hipo" is the defined data type in CLAS12.
             *
             * @param event_number
             * @return build hits from all fADC's of a crate for the a stream frame
             * @throws TriDASError if failed to process streams
             */
            virtual clara::any process_frame(int event_number) = 0;

            /**
             * Gets the byte order of the streamed data.
             *
             * @return the byte order of the data
             * @throws TriaDASError if failed
             */
            virtual Endian get_byte_order() = 0;

            /**
             * Gets the CLARA engine data-type for the type of the events.
             * The data-type will be used to serialize the events when the engine data
             * result needs to be sent to services over the network.
             *
             * @return the data-type of the events
             */
            virtual const EngineDataType &get_data_type() const = 0;

        public:
            std::vector<EngineDataType> input_data_types() const override;

            std::vector<EngineDataType> output_data_types() const override;

            std::set<std::string> states() const override;

        public:
            void reset() override;

        private:
            class Impl;
            std::unique_ptr<Impl> impl_;
        };

    }// end namespace stdlib
}// end namespace clara

#endif //CLARA_STREAMING_SERVICE_HPP
