#pragma once

#include <string>
#include <functional>

#include "chest.h"

// char* wrapper made 
struct StrSlice {
    const char* data;
    size_t start;
    size_t length;

    static const int npos = -1;

    static StrSlice from_string(const std::string& str, size_t start, size_t size) {
        return StrSlice{str.c_str(), start, size};
    }

    static StrSlice from_char(const char* str, size_t start, size_t size) {
        return StrSlice{str, start, size};
    }

    static StrSlice from_slice(const StrSlice& slice, size_t start, size_t size) {
        return StrSlice{slice.data, slice.start + start, size};
    }

    char operator [](int index) const {
        return this->data[this->start + index];
    }

    void map(std::function<void(const char)> func) const {
        for (int i = 0; i < this->length; i++) {
            func(this->get(i));
        }
    }

    std::string to_string() const {
        std::string out_str(this->length, ' ');
        for (int i = 0; i < this->length; i++) {
            out_str[i] = this->get(i);
        }

        return out_str;
    }

    char get(size_t index) const {
        return this->data[this->start + index];
    }

    static Chest<StrSlice> slice_on_delim(const char* buffer, size_t start, size_t cur_index, const char DELIM) {
        if (buffer[cur_index] == DELIM) { 
            return Chest<StrSlice>::stuff(StrSlice::from_char(buffer, start, cur_index - start)); 
        }

        return Chest<StrSlice>::sign("No slice yet.");
    }

    bool equals(const StrSlice& slice) const {
        if (this->length != slice.length) { return false; }

        for (int i = 0; i < this->length; i++) {
            if (this->get(i) != slice.get(i))  { return false; }
        }

        return true;
    }

    bool equals(const std::string& str) const {
        if (length != str.size()) { return false; }

        int index = 0;
        while (index < length) {
            if (this->get(index) != str[index]) { return false; }
            index++;
        }

        return true;
    }

    int contains(const std::string& str) const {
        if (this->length < str.size()) { return StrSlice::npos; }
        //std::cout << str << std::endl;

        int str_idx = 0;
        int str_len = (int) str.size();

        for (int i = 0; i < this->length; i++) {
            bool match = this->get(i) == str[str_idx];
            str_idx = (str_idx + 1) * match;

            if (str_idx == str_len) {
                //std::cout << "match, i: " << i << " str_len: " << str_len << " str_idx: " << str_idx << std::endl;
                return i - (str_len - 2);
            }
        }

        return StrSlice::npos;
    }

    size_t size() const {
        return this->length;
    }
};