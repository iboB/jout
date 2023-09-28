# jout

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/) [![Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A fast minimalistic single-header json writer for C++17.

It's essentially a trimmed-down version of [huse](https://github.com/iboB/huse) where only the basic JSON serialization is kept. It might be useful if one only wants to write a simple json without having to rely on all deps and features of huse.

## Usage

Just include `jout.hpp` in your project and you're good to go.

Optionally make use of the provided `CMakeLists.txt` which will add the dependency [mscharconv](https://github.com/iboB/mscharconv] in case the one provided by the standard library doesn't supprt `to_chars` for floating point numbers (which is the case for gcc 10, clang 14 and their previous versions).

## Features

* Single header
* No allocations
* No dependencies besides the standard library (well, unless you use an old one in which case you'll need mscharconv)
* Sequential. Values are added as they are encountered in the code. There is no way to go back and modify a value. 
* Optional extension headers to serialize common types

## Example

```cpp
    // create a document with an ostream and an optional pretty print flag
    jout::Document doc(std::cout, true);

    // create a root object
    auto root = doc.obj();

    // add some values
    root.val("hello", "world");
    root.val("number", 3.14);

    // add an array
    {
        // block, array is closed when ar goes out of scope
        auto ar = root.ar("array");
        ar.val(1);
        ar.val("str");
        ar.val(true);
        ar.val(nullptr);
    }

    // add a subobject
    {
        // block, object is closed when obj goes out of scope
        auto obj = root.obj("object");
        obj.val("key", "value");
    }
```

## License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This software is distributed under the MIT Software License.

See accompanying file LICENSE or copy [here](https://opensource.org/licenses/MIT).

Copyright &copy; 2023 [Borislav Stanimirov](http://github.com/iboB)
