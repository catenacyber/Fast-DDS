// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file StatisticsCommon.hpp
 */

#ifndef _FASTDDS_STATISTICS_RTPS_STATISTICSCOMMON_HPP_
#define _FASTDDS_STATISTICS_RTPS_STATISTICSCOMMON_HPP_

#include <memory>
#include <type_traits>

#include <fastdds/rtps/common/Guid.h>
#include <fastdds/rtps/common/SampleIdentity.h>
#include <fastdds/rtps/common/Time_t.h>
#include <fastdds/statistics/IListeners.hpp>
#include <fastrtps/utils/TimedMutex.hpp>


namespace eprosima {

namespace fastrtps {
namespace rtps {

class RTPSMessageGroup;

} // rtps
} // fastrtps

namespace fastdds {
namespace statistics {

// Members are private details
struct StatisticsAncillary;

class StatisticsListenersImpl
{
    std::unique_ptr<StatisticsAncillary> members_;

protected:

    /**
     * Create a class A auxiliary structure
     * @return true if successfully created
     */
    template<class A>
    void init_statistics()
    {
        static_assert(
            std::is_base_of<StatisticsAncillary, A>::value,
            "Auxiliary structure must derive from StatisticsAncillary");

        if (!members_)
        {
            members_.reset(new A);
        }
    }

    /**
     * Returns the auxiliary members
     * @return The specialized auxiliary structure for each class
     */
    StatisticsAncillary* get_aux_members() const;

    /**
     * Add a listener to receive statistics backend callbacks
     * @param listener
     * @return true if successfully added
     */
    bool add_statistics_listener_impl(
            std::shared_ptr<fastdds::statistics::IListener> listener);

    /**
     * Remove a listener from receiving statistics backend callbacks
     * @param listener
     * @return true if successfully removed
     */
    bool remove_statistics_listener_impl(
            std::shared_ptr<fastdds::statistics::IListener> listener);

    /**
     * Lambda function to traverse the listener collection
     * @param f function object to apply to each listener
     * @return function object after being applied to each listener
     */
    template<class Function>
    Function for_each_listener(
            Function f);

    /**
     * Retrieve endpoint mutexes from derived class
     * @return defaults to the endpoint mutex
     */
    virtual fastrtps::RecursiveTimedMutex& get_statistics_mutex() = 0;

    /**
     * Retrieve the GUID_t from derived class
     * @return endpoint GUID_t
     */
    virtual const fastrtps::rtps::GUID_t& get_guid() const = 0;
};

// Members are private details
struct StatisticsWriterAncillary;

class StatisticsWriterImpl
    : protected StatisticsListenersImpl
{

    /**
     * Create the auxiliary structure
     * @return nullptr on failure
     */
    StatisticsWriterAncillary* get_members() const;

    /**
     * Retrieve endpoint mutexes from derived class
     * @return defaults to the endpoint mutex
     */
    fastrtps::RecursiveTimedMutex& get_statistics_mutex() final;

    /**
     * Retrieve the GUID_t from derived class
     * @return endpoint GUID_t
     */
    const fastrtps::rtps::GUID_t& get_guid() const final;

protected:

    /**
     * Constructor. Mandatory member initialization.
     */
    StatisticsWriterImpl();

    // TODO: methods for listeners callbacks

    /**
     * @brief Report a change on the number of DATA / DATAFRAG submessages sent for a specific sample.
     * @param sample_identity SampleIdentity of the affected sample.
     * @param num_sent_submessages Current total number of submessages sent for the affected sample.
     */
    void on_sample_datas(
            const fastrtps::rtps::SampleIdentity& sample_identity,
            size_t num_sent_submessages);

    /**
     * @brief Report that a HEARTBEAT message is sent
     * @param current count of heartbeats
     */
    void on_heartbeat(
            uint32_t count);

    /**
     * @brief Report that a DATA / DATA_FRAG message is generated
     * @param num_destinations number of locators to which the message will be sent
     */
    void on_data_generated(
            size_t num_destinations);

    /// Notify listeners of DATA / DATA_FRAG counts
    void on_data_sent();

    /// Report that a GAP message is sent
    void on_gap();

    /*
     * @brief Report that several changes are marked for redelivery
     * @param number of changes to redeliver
     */
    void on_resent_data(
            uint32_t to_send);
};

// Members are private details
struct StatisticsReaderAncillary;

class StatisticsReaderImpl
    : protected StatisticsListenersImpl
{
    friend class fastrtps::rtps::RTPSMessageGroup;

    /**
     * Create the auxiliary structure
     * TODO: enable when a member is added to StatisticsReaderAncillary
     * @return nullptr on failure
     */
    StatisticsReaderAncillary* get_members() const
    {
        return nullptr;
    }

    /**
     * Retrieve endpoint mutexes from derived class
     * @return defaults to the endpoint mutex
     */
    fastrtps::RecursiveTimedMutex& get_statistics_mutex() final;

    /**
     * Retrieve the GUID_t from derived class
     * @return endpoint GUID_t
     */
    const fastrtps::rtps::GUID_t& get_guid() const final;

protected:

    /**
     * Constructor. Mandatory member initialization.
     */
    StatisticsReaderImpl();

    // TODO: methods for listeners callbacks

    /**
     * @brief Report that a sample has been notified to the user.
     * @param writer_guid GUID of the writer from where the sample was received.
     * @param source_timestamp Source timestamp received from the writer for the sample being notified.
     */
    void on_data_notify(
            const fastrtps::rtps::GUID_t& writer_guid,
            const fastrtps::rtps::Time_t& source_timestamp);

    /**
     * @brief Report that an ACKNACK message is sent
     * @param count current count of ACKNACKs
     */
    void on_acknack(
            int32_t count);

    /**
     * @brief Report that a NACKFRAG message is sent
     * @param count current count of NACKFRAGs
     */
    void on_nackfrag(
            int32_t count);
};

} // namespace statistics
} // namespace fastdds
} // namespace eprosima

#endif // _FASTDDS_STATISTICS_RTPS_STATISTICSCOMMON_HPP_
