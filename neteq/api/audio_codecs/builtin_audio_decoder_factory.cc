/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "api/audio_codecs/builtin_audio_decoder_factory.h"

#include <memory>
#include <vector>

#include "api/audio_codecs/audio_decoder_factory_template.h"
#if WEBRTC_USE_BUILTIN_OPUS
#include "api/audio_codecs/opus/audio_decoder_opus.h"  // nogncheck
#endif

namespace webrtc {

rtc::scoped_refptr<AudioDecoderFactory> CreateBuiltinAudioDecoderFactory() {
  return CreateAudioDecoderFactory<
#if WEBRTC_USE_BUILTIN_OPUS
      AudioDecoderOpus
#endif
    >();
}

}  // namespace webrtc
