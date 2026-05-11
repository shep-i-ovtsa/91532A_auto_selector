#include "bad_gif/bad_gif.hpp"
#include <algorithm>

void video_stream::parseHeader(const std::string& line) {
    //<w>$<h>$<enc>$<scale>$<fps>$0$
    if (line.size() < 2) return;
    std::string content = line.substr(1, line.size() - 2);

    int field = 0;
    int raw_w = 0;
    int raw_h = 0;
    uint8_t scale = 1;
    int fps_val= 30;
    std::string token;

    for (char c : content) {
        if (c == '$') {
            int val = token.empty() ? 0 : std::stoi(token);
            switch (field) {
                case 0: raw_w = val; break;
                case 1: raw_h = val; break;
                case 3: scale = static_cast<uint8_t>(val < 1 ? 1 : val); break;
                case 4: fps_val = val < 1 ? 30 : val; break;
                default: break;
            }
            ++field;
            token.clear();
        } else {
            token += c;
        }
    }

    width = raw_w / scale;
    height = raw_h / scale;
    frame_time = static_cast<uint32_t>(1000.0f / fps_val);

    frame.assign(static_cast<size_t>(width * height), 0);
}

uint8_t video_stream::decodeChar(char c) const {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'A' && c <= 'Z') return static_cast<uint8_t>((c - 'A') + 10);
    if (c >= 'a' && c <= 'z') return static_cast<uint8_t>((c - 'a') + 36);
    if (c == '$') return 62;
    if (c == '#')return 63;
    return 0;
}

void video_stream::paintFrame(const std::string& encoded) {
    std::fill(frame.begin(), frame.end(), 0);
    int idx = 0;
    const int sz = static_cast<int>(frame.size());

    for (int i = 0; i + 1 < static_cast<int>(encoded.size()); i += 2) {
        uint8_t color_idx = decodeChar(encoded[i]);
        uint8_t run = decodeChar(encoded[i + 1]) + 1;

        if (color_idx > 63) color_idx = 0;

        for (int r = 0; r < run && idx < sz; ++r)
            frame[idx++] = color_idx;
    }
}

bool video_stream::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    streaming = false;
    frame_cache.clear();
    current_frame_idx = 0;
    ended = false;

    std::string line;

    if (!std::getline(file, line)) return false;
    parseHeader(line);

    std::getline(file, line);

    while (std::getline(file, line))
        if (!line.empty())
            frame_cache.push_back(std::move(line));

    if (!frame_cache.empty())
        paintFrame(frame_cache[0]);

    return true;
}

bool video_stream::open(const std::string& path) {
    stream_file.open(path);
    if (!stream_file.is_open()) return false;

    streaming = true;
    ended= false;

    std::string line;

    if (!std::getline(stream_file, line)) return false;
    parseHeader(line);

    std::getline(stream_file, line);

    return true;
}


bool video_stream::update(unsigned long now) {
    if (ended)return false;
    if (now < next_frame_time) return false;

    next_frame_time = now + frame_time;

    if (streaming) {
        std::string line;
        if (!std::getline(stream_file, line)) {
            ended = true;
            return false;
        }
        if (!line.empty()) paintFrame(line);
        return true;

    } else {
        if (frame_cache.empty()) return false;

        ++current_frame_idx;

        if (current_frame_idx >= static_cast<int>(frame_cache.size())) {
            if (looping) {
                current_frame_idx = 0;
            } else {
                current_frame_idx = static_cast<int>(frame_cache.size()) - 1;
                ended = true;
                return false;
            }
        }

        paintFrame(frame_cache[current_frame_idx]);
        return true;
    }
}


void video_stream::renderTo(
    std::vector<uint8_t>& target,
    int targetW,
    int targetH
) const {
    if (!visible || frame.empty() || draw_width <= 0 || draw_height <= 0) return;

  
    const float sx = static_cast<float>(width)/ static_cast<float>(draw_width);
    const float sy = static_cast<float>(height)/ static_cast<float>(draw_height);

    const int maxSrcX = width - 1;
    const int maxSrcY = height - 1;

    for (int py = 0; py < draw_height; ++py) {
        const int srcY = std::min(static_cast<int>(py * sy), maxSrcY);
        const int dstY = y + py;
        if (dstY < 0 || dstY >= targetH) continue;

        const int srcRowOff = srcY * width;
        const int dstRowOff = dstY * targetW;

        for (int px = 0; px < draw_width; ++px) {
            const int srcX = std::min(static_cast<int>(px * sx), maxSrcX);
            const int dstX = x + px;
            if (dstX < 0 || dstX >= targetW) continue;

            target[dstRowOff + dstX] = frame[srcRowOff + srcX];
        }
    }
}