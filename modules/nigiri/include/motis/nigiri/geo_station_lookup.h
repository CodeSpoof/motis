#pragma once

#include "geo/point_rtree.h"

#include "motis/module/message.h"
#include "motis/nigiri/tag_lookup.h"

namespace nigiri {
struct timetable;
}

namespace motis::nigiri {

motis::module::msg_ptr geo_station_lookup(tag_lookup const& tags,
                                          ::nigiri::timetable const&,
                                          geo::point_rtree const&,
                                          motis::module::msg_ptr const&);

motis::module::msg_ptr station_location(tag_lookup const& tags,
                                        ::nigiri::timetable const&,
                                        motis::module::msg_ptr const&);

}  // namespace motis::nigiri