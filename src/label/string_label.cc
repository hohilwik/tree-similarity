// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik, and Nikolaus Augsten
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file node/string_node_data.cc
///
/// \details
/// Contains the implementation of the StringLabel class.

#include "string_label.h"
#include <iostream>

namespace label {

//StringLabel::StringLabel(const std::string& label) : label_(label) { std::cout << "Def" << std::endl; }
StringLabel::StringLabel(std::string&& label) : label_(std::move(label)) {}

const std::string& StringLabel::label() const {
    return label_;
}

bool StringLabel::operator==(const StringLabel& other) const {
    return (label_.compare(other.label()) == 0);
}

} // namespace label