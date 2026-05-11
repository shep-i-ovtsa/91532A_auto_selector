#pragma once
#include "main.h"
#include <array>
#include <fstream>
#include <string>
#include <vector>

class video_stream {
    public:

    int width = 0; 
    int height = 0;

    int x = 0;
    int y = 0;
    int draw_width = 0;
    int draw_height = 0;
    bool visible = true;

    std::vector<uint8_t> frame;

    uint32_t next_frame_time = 0;
    uint32_t frame_time = 33;
    bool looping = true;
    bool ended= false; 

    bool load(const std::string& path);

    bool open(const std::string& path);

    
    bool update(unsigned long now);

    void renderTo(std::vector<uint8_t>& target, int targetW, int targetH) const;
    void reset() {
        current_frame_idx = -1; //resets our current frame to the beggining
        ended = false;
        next_frame_time = 0;
        // streaming mode — seek back past the header
        if (streaming) {
            stream_file.clear();
            stream_file.seekg(0);
            std::string line;
            std::getline(stream_file, line);
            std::getline(stream_file, line); //skip our headers since the info is already loaded
        }
    }
    int frame_count() const { return static_cast<int>(frame_cache.size()); }

private:
    std::ifstream stream_file;
    bool streaming = false;

    std::vector<std::string> frame_cache;
    int current_frame_idx = 0;

    void parseHeader(const std::string& line);
    uint8_t decodeChar(char c) const;
    void paintFrame(const std::string& encoded);
};