/*
Copyright (c) 2020 Josh Watts

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MEDIAINFO_H_
#define MEDIAINFO_H_

#include <map>

class MediaInfo
{
public:
    MediaInfo() = default;
    ~MediaInfo() = default;

    enum field_t {
        FIELD_TITLE = 0,
        FIELD_ARTIST,
        FIELD_ALBUM,
        FIELD_TRACK_NUM,
        FIELD_TRACK_COUNT,
        FIELD_TIME_MS,
    };

    void setField(field_t f, std::string v) { this->_fields[f] = v; }
    std::string getField(field_t f);

private:
    std::map<int,std::string> _fields;
};

#endif