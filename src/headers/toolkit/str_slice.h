#pragma once

#include <string>
#include <functional>

#include "chest.h"

// Quick little char wrapper for simplifying substring operations & parsing character streams
struct StrSlice {
    const char* data;
    size_t start;
    size_t length;

    static const int npos = -1;

    // construct a StrSlice from a string at some starting point and given a size
    static StrSlice from_string(const std::string& str, size_t start, size_t size) {
        return StrSlice{str.c_str(), start, size};
    }

    // construct a StrSlice from a char* at some starting point and given a size
    static StrSlice from_char(const char* str, size_t start, size_t size) {
        return StrSlice{str, start, size};
    }

    // construct a StrSlice from another StrSlice at some starting point and given a size
    static StrSlice from_slice(const StrSlice& slice, size_t start, size_t size) {
        return StrSlice{slice.data, slice.start + start, size};
    }

    char operator [](int index) const {
        return this->data[this->start + index];
    }

    // Apply some function func given a stream of the characters in the StrSlice as an input
    void map(std::function<void(const char)> func) const {
        for (int i = 0; i < this->length; i++) {
            func(this->get(i));
        }
    }

    // Returns this StrSlice as a string
    std::string to_string() const {
        std::string out_str(this->length, ' ');
        for (int i = 0; i < this->length; i++) {
            out_str[i] = this->get(i);
            std::cout << out_str[i] << std::endl;
        }

        return out_str;
    }

    // Get a character in the StrSlice at some index
    char get(size_t index) const {
        return this->data[this->start + index];
    }

    // Given a buffer, a starting indexing, a current index, and a DELIMETER, return a Chest stuffed with a StrSlice
    // (buffer, start, cur_index - start) when the DELIM is met. Otherwise return a Chest with a sign
    static Chest<StrSlice> slice_on_delim(const char* buffer, size_t start, size_t cur_index, const char DELIM) {
        if (buffer[cur_index] == DELIM) { 
            return Chest<StrSlice>::stuff(StrSlice::from_char(buffer, start, cur_index - start - 1)); 
        }

        return Chest<StrSlice>::sign("No slice yet.");
    }

    // Check if two StrSlices are equal to one another
    bool equals(const StrSlice& slice) const {
        if (this->length != slice.length) { return false; }

        for (int i = 0; i < this->length; i++) {
            if (this->get(i) != slice.get(i))  { return false; }
        }

        return true;
    }

    // Check if the StrSlice is equal to a given string str
    bool equals(const std::string& str) const {
        if (length != str.size()) { return false; }

        int index = 0;
        while (index < length) {
            if (this->get(index) != str[index]) { return false; }
            index++;
        }

        return true;
    }

    // Check if the StrSlice contains another string
    int contains(const std::string& str) const {
        if (this->length < str.size()) { return StrSlice::npos; }
        //std::cout << str << std::endl;

        int str_idx = 0;
        int str_len = (int) str.size();

        for (int i = 0; i < this->length; i++) {
            bool match = this->get(i) == str[str_idx];
            str_idx = (str_idx + 1) * match;

            if (str_idx == str_len) {
                return i - (str_len - 2);
            }
        }

        return StrSlice::npos;
    }

    size_t size() const {
        return this->length;
    }
};