#pragma once

namespace RtSound {

enum FrequencyUnit {
  Frequency_Index = 0,
  Frequency_Hertz,
  Frequency_mHertz,
  Frequency_Normal,
  Frequency_mNormal,
};

inline float hertz_to_mHertz(float hertz) { return hertz * 1e+3f; }
inline float mHertz_to_hertz(float mHertz) { return mHertz * 1e-3f; }
inline float normal_to_mNormal(float normal) { return normal * 1e+3f; }
inline float mNormal_to_normal(float mNormal) { return mNormal * 1e-3f; }

// Hertz & Index
// -----------------------------------------------------------------------------
inline float index_to_hertz(float index, float sampleRate, float bufferSize) {
  return index * sampleRate / bufferSize;
}

inline float index_to_mHertz(float index, float sampleRate, float bufferSize) {
  return mHertz_to_hertz(index_to_hertz(index, sampleRate, bufferSize));
}

inline float hertz_to_index(float hertz, float sampleRate, float bufferSize) {
  return hertz / sampleRate * bufferSize;
}

// Hertz to Normal
// -----------------------------------------------------------------------------
inline float hertz_to_normal(float hertz, float sampleRate) {
  return hertz / sampleRate * 2.0f;
}

inline float hertz_to_mNormal(float hertz, float sampleRate) {
  return normal_to_mNormal(hertz_to_normal(hertz, sampleRate));
}

inline float mHertz_to_normal(float mHertz, float sampleRate) {
  return hertz_to_normal(mHertz_to_hertz(mHertz), sampleRate);
}

inline float mHertz_to_mNormal(float mHertz, float sampleRate) {
  return hertz_to_mNormal(mHertz_to_hertz(mHertz), sampleRate);
}

// Normal to Hertz
// -----------------------------------------------------------------------------
inline float normal_to_hertz(float percent, float sampleRate) {
  return percent * sampleRate / 2.0f;
}

inline float normal_to_mHertz(float percent, float sampleRate) {
  return hertz_to_mHertz(normal_to_hertz(percent, sampleRate));
}

inline float mNormal_to_hertz(float mNormal, float sampleRate) {
  return normal_to_hertz(mNormal_to_normal(mNormal), sampleRate);
}

inline float mNormal_to_mHertz(float mNormal, float sampleRate) {
  return hertz_to_mHertz(
      normal_to_hertz(mNormal_to_normal(mNormal), sampleRate));
}

// Normal & Index
// -----------------------------------------------------------------------------
inline float index_to_normal(float index, float sampleRate, float bufferSize) {
  return hertz_to_normal(index_to_hertz(index, sampleRate, bufferSize),
                         sampleRate);
}

inline float index_to_mNormal(float index, float sampleRate, float bufferSize) {
  return hertz_to_mNormal(index_to_hertz(index, sampleRate, bufferSize),
                          sampleRate);
}

inline float normal_to_index(float normal, float sampleRate, float bufferSize) {
  return hertz_to_index(normal_to_hertz(normal, sampleRate), sampleRate,
                        bufferSize);
}

inline float mNormal_to_index(float mNormal, float sampleRate,
                              float bufferSize) {
  return hertz_to_index(mNormal_to_hertz(mNormal, sampleRate), sampleRate,
                        bufferSize);
}

// Converter
// -----------------------------------------------------------------------------

inline float convertFrequency(FrequencyUnit fromUnit, FrequencyUnit toUnit,
                              float value, float sampleRate,
                              float bufferSize = 0.0f) {
  float valueHertz{};

  if (fromUnit == Frequency_Index) {
    valueHertz = index_to_hertz(value, sampleRate, bufferSize);
  } else if (fromUnit == Frequency_Hertz) {
    valueHertz = value;
  } else if (fromUnit == Frequency_mHertz) {
    valueHertz = mHertz_to_hertz(value);
  } else if (fromUnit == Frequency_Normal) {
    valueHertz = normal_to_hertz(value, sampleRate);
  } else if (fromUnit == Frequency_mNormal) {
    valueHertz = mNormal_to_hertz(value, sampleRate);
  }

  float valueUnit{};

  if (toUnit == Frequency_Index) {
    valueUnit = hertz_to_index(valueHertz, sampleRate, bufferSize);
  } else if (toUnit == Frequency_Hertz) {
    valueUnit = valueHertz;
  } else if (toUnit == Frequency_mHertz) {
    valueUnit = hertz_to_mHertz(valueHertz);
  } else if (toUnit == Frequency_Normal) {
    valueUnit = hertz_to_normal(valueHertz, sampleRate);
  } else if (toUnit == Frequency_mNormal) {
    valueUnit = hertz_to_mNormal(valueHertz, sampleRate);
  }

  return valueUnit;
}

} // namespace RtSound
