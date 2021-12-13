/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "api/audio_codecs/opus/audio_decoder_opus.h"

#include <memory>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "modules/audio_coding/codecs/opus/audio_decoder_opus.h"

#include <string.h>
#if defined(WEBRTC_WIN)
#define strncasecmp _strnicmp
#endif

static bool EqualsIgnoreCase(const std::string &piece1, const std::string_view &piece2)
{
  return (piece1.size() == piece2.size() &&
          0 == ::strncasecmp(piece1.data(), piece2.data(), piece1.size()));
}

namespace webrtc {

absl::optional<AudioDecoderOpus::Config> AudioDecoderOpus::SdpToConfig(
    const SdpAudioFormat& format) {
  const auto num_channels = [&]() -> absl::optional<int> {
    auto stereo = format.parameters.find("stereo");
    if (stereo != format.parameters.end()) {
      if (stereo->second == "0") {
        return 1;
      } else if (stereo->second == "1") {
        return 2;
      } else {
        return absl::nullopt;  // Bad stereo parameter.
      }
    }
    return 1;  // Default to mono.
  }();
  if (::EqualsIgnoreCase(format.name, "opus") &&
      format.clockrate_hz == 48000 && format.num_channels == 2 &&
      num_channels) {
    return Config{*num_channels};
  } else {
    return absl::nullopt;
  }
}

void AudioDecoderOpus::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  AudioCodecInfo opus_info{48000, 1, 64000, 6000, 510000};
  opus_info.allow_comfort_noise = false;
  opus_info.supports_network_adaption = true;
  SdpAudioFormat opus_format(
      {"opus", 48000, 2, {{"minptime", "10"}, {"useinbandfec", "1"}}});
  specs->push_back({std::move(opus_format), opus_info});
}

std::unique_ptr<AudioDecoder> AudioDecoderOpus::MakeAudioDecoder(
    Config config,
    absl::optional<AudioCodecPairId> /*codec_pair_id*/) {
  return absl::make_unique<AudioDecoderOpusImpl>(config.num_channels);
}

}  // namespace webrtc
