// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <cstdint>
#include <optional>
#include <iostream>
#include <string_view>

#if JOUT_USE_MSCHARCONV
#   include <msstl/charconv.hpp>
#else
#   include <charconv>
#endif

#include <cassert>

namespace jout {

// json imposed limits (max integer which can be stored in a double)
static inline constexpr int64_t Max_Int64 = 9007199254740992ll;
static inline constexpr int64_t Min_Int64 = -9007199254740992ll;
static inline constexpr uint64_t Max_Uint64 = 9007199254740992ull;

class Document;
class Object;
class Array;

class Node {
protected:
    Node(Document& s)
        : m_doc(s)
    {}

    friend class Document;
    Document& m_doc;
public:
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(Node&&) = delete;

    Document& _d() { return m_doc; }

    Object obj();
    Array ar();

    template <typename T>
    void val(const T& v);

    [[noreturn]] void throwException(const std::string& msg) const;
};

class Array : public Node
{
public:
    Array(Document& d);
    ~Array();
};

class Object : private Node
{
public:
    Object(Document& d);
    ~Object();

    using Node::throwException;
    using Node::_d;

    Node& key(std::string_view k);

    Object obj(std::string_view k)
    {
        return key(k).obj();
    }
    Array ar(std::string_view k)
    {
        return key(k).ar();
    }

    template <typename T>
    void val(std::string_view k, const T& v)
    {
        key(k).val(v);
    }

    template <typename T>
    void val(std::string_view k, const std::optional<T>& v)
    {
        if (v) val(k, *v);
    }
};

namespace
{
inline std::optional<std::string_view> escapeUtf8Byte(char c)
{
    auto u = uint8_t(c);

    // http://www.json.org/
    if (u > '\\') return {}; // no escape needed for characters above backslash
    if (u == '"') return "\\\"";
    if (u == '\\') return "\\\\";
    if (u >= ' ') return {}; // no escape needed for other characters above space
    static constexpr std::string_view belowSpace[' '] = {
        "\\u0000","\\u0001","\\u0002","\\u0003","\\u0004","\\u0005","\\u0006","\\u0007",
          "\\b"  ,  "\\t"  ,  "\\n"  ,"\\u000b",  "\\f"  ,  "\\r"  ,"\\u000e","\\u000f",
        "\\u0010","\\u0011","\\u0012","\\u0013","\\u0014","\\u0015","\\u0016","\\u0017",
        "\\u0018","\\u0019","\\u001a","\\u001b","\\u001c","\\u001d","\\u001e","\\u001f"};
    return belowSpace[u];
}

inline void writeEscapedUTF8StringToStreambuf(std::streambuf& buf, std::string_view str)
{
    // we could use this simple code here
    // but it writes bytes one by one
    //
    // for (auto c : str)
    // {
    //     auto e = escapeUtf8Byte(c);
    //     if (!e) m_out.put(c);
    //     else m_out << *e;
    // }
    //
    // to optimize, we'll use the following which writes in chunks
    // if there is nothing to be escaped in a string,
    //  it will print the whole string at the end as a single operation

    auto begin = str.data();
    const auto end = str.data() + str.size();

    auto p = begin;
    while (p != end) {
        auto esc = escapeUtf8Byte(*p);
        if (!esc) ++p;
        else
        {
            if (p != begin) buf.sputn(begin, p - begin);
            buf.sputn(esc->data(), esc->length());
            begin = ++p;
        }
    }
    if (p != begin) buf.sputn(begin, p - begin);
}
}

class Document : public Node {
    std::ostream& m_out;
    std::optional<std::string_view> m_pendingKey;
    bool m_hasValue = false; // used to check whether a coma is needed
    const bool m_pretty;
    uint32_t m_depth = 0; // used to indent if pretty
public:
    Document(std::ostream& out, bool pretty = false)
        : Node(*this)
        , m_out(out)
        , m_pretty(pretty)
    {}

    Document(const Document&) = delete;
    Document& operator==(const Document&) = delete;
    Document(Document&&) = delete;
    Document& operator==(Document&&) = delete;

    ~Document() {
        if (std::uncaught_exceptions()) return; // nothing smart to do
        assert(m_depth == 0);
    }

    void writeRawJson(std::string_view json)
    {
        prepareWriteVal();
        m_out.rdbuf()->sputn(json.data(), json.size());
    }

    template <typename T>
    void writeSmallInteger(T n)
    {
        prepareWriteVal();

        auto& out = *m_out.rdbuf();

        using Unsigned = std::make_unsigned_t<T>;
        Unsigned uvalue = Unsigned(n);

        if constexpr (std::is_signed_v<T>) {
            if (n < 0) {
                out.sputc('-');
                uvalue = 0 - uvalue;
            }
        }

        char buf[24]; // enough for signed 2^64 in decimal
        const auto end = buf + sizeof(buf);
        auto p = end;

        do {
            *--p = char('0' + uvalue % 10);
            uvalue /= 10;
        } while (uvalue != 0);

        out.sputn(p, end - p);
    }

    void write(bool val)
    {
        static constexpr std::string_view t = "true", f = "false";
        writeRawJson(val ? t : f);
    }

    void write(std::nullptr_t) { writeRawJson("null"); }

    void write(short val) { writeSmallInteger(val); }
    void write(unsigned short val) { writeSmallInteger(val); }
    void write(int val) { writeSmallInteger(val); }
    void write(unsigned int val) { writeSmallInteger(val); }

    template <typename T>
    void writePotentiallyBigIntegerValue(T val)
    {
        static const std::string_view IntegerTooBig = "Integer value is bigger than maximum allowed for JSON";

        if constexpr (sizeof(T) <= 4)
        {
            // gcc and clang have long equal intptr_t, msvc has long at 4 bytes
            writeSmallInteger(val);
        }
        else if constexpr (std::is_signed_v<T>)
        {
            if (val >= Min_Int64 && val <= Max_Int64)
            {
                writeSmallInteger(val);
            }
            else
            {
                throwException(std::string(IntegerTooBig));
            }
        }
        else
        {
            if (val <= Max_Uint64)
            {
                writeSmallInteger(val);
            }
            else
            {
                throwException(std::string(IntegerTooBig));
            }
        }

    }

    // some values may not fit json's numbers
    void write(long val) { writePotentiallyBigIntegerValue(val); }
    void write(unsigned long val) { writePotentiallyBigIntegerValue(val); }
    void write(long long val) { writePotentiallyBigIntegerValue(val); }
    void write(unsigned long long val) { writePotentiallyBigIntegerValue(val); }

    template <typename T>
    void writeFloatValue(T val)
    {
        if (std::isfinite(val))
        {
            char out[25]; // max length of double
            auto result = std::to_chars(out, out + sizeof(out), val);
            writeRawJson(std::string_view(out, result.ptr - out));
        }
        else
        {
            throwException("Floating point value is not finite. Not supported by JSON");
        }
    }

    void write(float val) { writeFloatValue(val); }
    void write(double val) { writeFloatValue(val); }

    void writeQuotedEscapedUTF8String(std::string_view str)
    {
        auto& out = *m_out.rdbuf();
        out.sputc('"');
        writeEscapedUTF8StringToStreambuf(out, str);
        out.sputc('"');
    }

    void write(std::string_view val)
    {
        prepareWriteVal();
        writeQuotedEscapedUTF8String(val);
    }

    void write(const char* val) // helper (otherwise literals get converted to bool)
    {
        write(std::string_view(val));
    }

    void write(std::nullopt_t)
    {
        m_pendingKey.reset();
    }

    void pushKey(std::string_view k)
    {
        assert(!m_pendingKey);
        m_pendingKey = k;
    }

    void open(char o)
    {
        prepareWriteVal();
        m_out.rdbuf()->sputc(o);
        m_hasValue = false;
        ++m_depth;
    }

    void close(char c)
    {
        assert(m_depth);
        --m_depth;
        if (m_hasValue) newLine();
        m_out.rdbuf()->sputc(c);
        m_hasValue = true;
    }

    void openObject() { open('{'); }
    void closeObject() { close('}'); }
    void openArray() { open('['); }
    void closeArray() { close(']'); }

    void prepareWriteVal()
    {
        auto& out = *m_out.rdbuf();

        if (m_hasValue)
        {
            out.sputc(',');
        }

        newLine();

        if (m_pendingKey)
        {
            writeQuotedEscapedUTF8String(*m_pendingKey);
            out.sputc(':');
            m_pendingKey.reset();
        }

        m_hasValue = true;
    }

    void newLine()
    {
        if (!m_pretty) return; // not pretty
        if (m_depth == 0 && !m_hasValue) return; // no new line for initial value

        auto& out = *m_out.rdbuf();
        out.sputc('\n');
        static constexpr std::string_view indent = "  ";
        for (uint32_t i = 0; i < m_depth; ++i)
        {
            out.sputn(indent.data(), indent.size());
        }
    }

    void throwException(const std::string& msg) const
    {
        throw std::runtime_error(msg);
    }
};

inline Object Node::obj()
{
    return Object(m_doc);
}

inline Array Node::ar()
{
    return Array(m_doc);
}

template <typename T>
void Node::val(const T& v)
{
    joutWrite(*this, v);
}

inline void Node::throwException(const std::string& msg) const {
    m_doc.throwException(msg);
}

inline Array::Array(Document& s)
    : Node(s)
{
    m_doc.openArray();
}

inline Array::~Array()
{
    m_doc.closeArray();
}

inline Object::Object(Document& s)
    : Node(s)
{
    m_doc.openObject();
}

inline Object::~Object()
{
    m_doc.closeObject();
}

inline Node& Object::key(std::string_view k)
{
    m_doc.pushKey(k);
    return *this;
}

void joutWrite(Node& n, bool val) { n._d().write(val); }
void joutWrite(Node& n, std::nullptr_t val) { n._d().write(val); }
void joutWrite(Node& n, short val) { n._d().write(val); }
void joutWrite(Node& n, unsigned short val) { n._d().write(val); }
void joutWrite(Node& n, int val) { n._d().write(val); }
void joutWrite(Node& n, unsigned int val) { n._d().write(val); }
void joutWrite(Node& n, long val) { n._d().write(val); }
void joutWrite(Node& n, unsigned long val) { n._d().write(val); }
void joutWrite(Node& n, long long val) { n._d().write(val); }
void joutWrite(Node& n, unsigned long long val) { n._d().write(val); }
void joutWrite(Node& n, float val) { n._d().write(val); }
void joutWrite(Node& n, double val) { n._d().write(val); }
void joutWrite(Node& n, std::string_view val) { n._d().write(val); }
void joutWrite(Node& n, const char* val) { n._d().write(val); }
void joutWrite(Node& n, std::nullopt_t val) { n._d().write(val); }

} // namespace jout
