/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_network_monitor_ConnectionInactivityChecker_hpp
#define oatpp_network_monitor_ConnectionInactivityChecker_hpp

#include "MetricsChecker.hpp"
#include "oatpp/core/base/Compiler.hpp"

namespace oatpp { namespace network { namespace monitor {

/**
 * ConnectionInactivityChecker - checks if a connection is inactive (has no read/writes) and whether it should be closed.
 * Extends - &id:oatpp::network::monitor::MetricsChecker;.
 */
class ConnectionInactivityChecker : public MetricsChecker {
private:
  std::chrono::duration<v_int64, std::micro> m_lastReadTimeout;
  std::chrono::duration<v_int64, std::micro> m_lastWriteTimeout;
public:

  /**
   * Constructor.
   * @param lastReadTimeout - how long can live connection without reads.
   * @param lastWriteTimeout - how long can live connection without writes.
   */
  ConnectionInactivityChecker(const std::chrono::duration<v_int64, std::micro>& lastReadTimeout,
                              const std::chrono::duration<v_int64, std::micro>& lastWriteTimeout);

  std::vector<oatpp::String> getMetricsList() override;

  std::shared_ptr<StatCollector> createStatCollector(const oatpp::String& metricName) override GPP_ATTRIBUTE(noreturn);

  bool check(const ConnectionStats& stats, v_int64 currMicroTime) override;

};

}}}

#endif //oatpp_network_monitor_ConnectionInactivityChecker_hpp
