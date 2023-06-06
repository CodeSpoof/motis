#include "motis/rt/metrics.h"

#include <vector>

using namespace motis::module;

namespace motis::rt {

void count_message(rt_metrics& metrics, motis::ris::RISMessage const* msg,
                   unixtime const processing_time) {
  auto const count = [&](auto const& fn) {
    metrics.update(msg->timestamp(), processing_time, [&](metrics_entry* m) {
      ++m->messages_;
      fn(m);
    });
  };

  switch (msg->content_type()) {
    case ris::RISMessageUnion_DelayMessage:
      count([](metrics_entry* m) { ++m->delay_messages_; });
      break;
    case ris::RISMessageUnion_CancelMessage:
      count([](metrics_entry* m) { ++m->cancel_messages_; });
      break;
    case ris::RISMessageUnion_AdditionMessage:
      count([](metrics_entry* m) { ++m->additional_messages_; });
      break;
    case ris::RISMessageUnion_RerouteMessage:
      count([](metrics_entry* m) { ++m->reroute_messages_; });
      break;
    case ris::RISMessageUnion_TrackMessage:
      count([](metrics_entry* m) { ++m->track_messages_; });
      break;
    case ris::RISMessageUnion_FullTripMessage:
      count([](metrics_entry* m) { ++m->full_trip_messages_; });
      break;
    case ris::RISMessageUnion_TripFormationMessage:
      count([](metrics_entry* m) { ++m->trip_formation_messages_; });
      break;
    default: break;
  }
}

msg_ptr get_metrics_api(rt_metrics const& metrics) {
  message_creator mc;

  auto const metrics_to_fbs = [&](rt_metrics_storage const& m) {
    std::vector<std::uint64_t> messages, delay_messages, cancel_messages,
        additional_messages, reroute_messages, track_messages,
        full_trip_messages, trip_formation_messages, new_trips, cancellations,
        reroutes, rule_service_reroutes, trip_delay_updates,
        event_delay_updates, trip_track_updates, trip_id_not_found,
        trip_id_ambiguous;

    messages.reserve(m.size());
    delay_messages.reserve(m.size());
    cancel_messages.reserve(m.size());
    additional_messages.reserve(m.size());
    reroute_messages.reserve(m.size());
    track_messages.reserve(m.size());
    full_trip_messages.reserve(m.size());
    trip_formation_messages.reserve(m.size());
    new_trips.reserve(m.size());
    cancellations.reserve(m.size());
    reroutes.reserve(m.size());
    rule_service_reroutes.reserve(m.size());
    trip_delay_updates.reserve(m.size());
    event_delay_updates.reserve(m.size());
    trip_track_updates.reserve(m.size());
    trip_id_not_found.reserve(m.size());
    trip_id_ambiguous.reserve(m.size());

    for (auto i = 0UL; i < m.size(); ++i) {
      auto const& entry = m.data_[(m.start_index_ + i) % m.size()];
      messages.push_back(entry.messages_);
      delay_messages.push_back(entry.delay_messages_);
      cancel_messages.push_back(entry.cancel_messages_);
      additional_messages.push_back(entry.additional_messages_);
      reroute_messages.push_back(entry.reroute_messages_);
      track_messages.push_back(entry.track_messages_);
      full_trip_messages.push_back(entry.full_trip_messages_);
      trip_formation_messages.push_back(entry.trip_formation_messages_);
      new_trips.push_back(entry.ft_new_trips_);
      cancellations.push_back(entry.ft_cancellations_);
      reroutes.push_back(entry.ft_reroutes_);
      rule_service_reroutes.push_back(entry.ft_rule_service_reroutes_);
      trip_delay_updates.push_back(entry.ft_trip_delay_updates_);
      event_delay_updates.push_back(entry.ft_event_delay_updates_);
      trip_track_updates.push_back(entry.ft_trip_track_updates_);
      trip_id_not_found.push_back(entry.ft_trip_id_not_found_);
      trip_id_ambiguous.push_back(entry.ft_trip_id_ambiguous_);
    }

    return CreateRtMetrics(
        mc, m.start_time(), m.size(), mc.CreateVector(messages),
        mc.CreateVector(delay_messages), mc.CreateVector(cancel_messages),
        mc.CreateVector(additional_messages), mc.CreateVector(reroute_messages),
        mc.CreateVector(track_messages), mc.CreateVector(full_trip_messages),
        mc.CreateVector(trip_formation_messages), mc.CreateVector(new_trips),
        mc.CreateVector(cancellations), mc.CreateVector(reroutes),
        mc.CreateVector(rule_service_reroutes),
        mc.CreateVector(trip_delay_updates),
        mc.CreateVector(event_delay_updates),
        mc.CreateVector(trip_track_updates), mc.CreateVector(trip_id_not_found),
        mc.CreateVector(trip_id_ambiguous));
  };

  mc.create_and_finish(
      MsgContent_RtMetricsResponse,
      CreateRtMetricsResponse(mc, metrics_to_fbs(metrics.by_msg_timestamp_),
                              metrics_to_fbs(metrics.by_processing_time_))
          .Union());
  return make_msg(mc);
}

}  // namespace motis::rt
