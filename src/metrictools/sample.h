/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <string>
#include <map>
#include <metrictools/metric.h>

namespace fnordmetric {
class ConfigList;
class Backend;

using TimestampType = uint64_t;
using LabelSet = std::map<std::string, std::string>;

enum class SampleFormat {
  STATSD, JSON
};

class Sample {
public:

  Sample(
      const MetricIDType& metric_id,
      const LabelSet& labels,
      TimestampType time,
      const std::string& value);

  const std::string& getMetricID() const;
  const std::string& getValue() const;
  TimestampType getTime() const;
  const LabelSet& getLabels() const;

protected:
  const MetricIDType metric_id_;
  const LabelSet labels_;
  const TimestampType time_;
  const std::string& value_;
};

std::string getSampleFormatName(SampleFormat t);
bool parseSampleFormat(const std::string& s, SampleFormat* t);

ReturnCode parseSamples(
    SampleFormat format,
    const std::string& input,
    std::vector<Sample>* samples);

ReturnCode storeSamples(
    const ConfigList* config,
    Backend* storage_backend,
    const std::vector<Sample>& samples);

} // namespace fnordmetric

