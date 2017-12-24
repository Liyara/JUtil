#ifndef JUTIL_IO_H
#define JUTIL_IO_H

#include "Core/String.h"

struct _iobuf;

/**
    @section DESCRIPTION

    Contains classes used for interacting with input and output steams.

    This file is a part of JUtil. @see jutil.h.
*/
namespace jutil JUTIL_PUBLIC_ {

    /**
        @namespace io_base

        These are base classes that define the basic functionality of io classes.

        @class OutputHandler:   defines functions for interacting with output streams.
        operator<<(const T&):   converts [t] to a string if possible,
                                then passes it to the abstract method put(String).

        @class InputHandler:    defines functions for interacting with input streams.
        operator>>(T&):         Calls the abstract method scan(),
                                then sets [t] as the return of scan() (casting String -> t).
    */
    namespace io_base JUTIL_PRIVATE_ {
        class JUTIL_PRIVATE_ OutputHandler {
        public:
            template <
                typename T
                #ifdef JUTIL_CPP11
                , typename = typename Enable<Convert<const T, const String>::Value>::Type
                #endif
            >
            const OutputHandler &operator<<(const T &t) JUTIL_CN_ {
                put(String::toString(t));
                return *this;
            }

            #ifdef JUTIL_CPP11
                template <
                    typename T, typename = typename Enable<Convert<T, String>::Value>::Type
                >
                const OutputHandler &operator<<(T &&t) JUTIL_CN_ {
                    put(String::toString(move(t)));
                    return *this;
                }
                template <
                    typename ...T,
                    typename = typename Enable<!(Convert<T..., String>::Value || Convert<const T..., const String>::Value)>::Type
                >
                const OutputHandler &operator<<(T...) JUTIL_CN_ {
                    put(String("\n\tOne parameter passed could not be output.\n"));
                    return *this;
                }
            #endif
            virtual ~OutputHandler() JUTIL_N_ {}
        protected:
            JUTIL_CX_ OutputHandler() JUTIL_N_;
            virtual void put(const String&) JUTIL_CN_ = 0;
        };
        class JUTIL_PRIVATE_ InputHandler {
        public:
            template <typename T>
            InputHandler &operator>>(T &t) JUTIL_N_ {
                JUTIL_INIT(String input, scan());
                char arr[input.size() + 1];
                input.array(arr);
                t = String::fromStringTo<T>(arr);
                return *this;
            }
            virtual ~InputHandler() JUTIL_N_ {}
        protected:
            JUTIL_CX_ InputHandler() JUTIL_N_;
            virtual String scan() JUTIL_N_ = 0;
        };
    }

    /**
        @namespace io_interface

        These classes inherit classes defines in io_base.

        Objects of these classes are used for basic console IO,
        and will be declared later.

        Attempting to use objects not declared by jutil will result
        in an error message being thrown.

    */
    namespace io_interface JUTIL_PRIVATE_ {
        class JUTIL_PRIVATE_ In JUTIL_F_: public io_base::InputHandler  {
            String scan() JUTIL_N_ JUTIL_OVERRIDE_;
        };
        class JUTIL_PRIVATE_ Out JUTIL_F_: public io_base::OutputHandler {
            void put(const String&) JUTIL_CN_ JUTIL_OVERRIDE_;
        };
        class JUTIL_PRIVATE_ Err JUTIL_F_: public io_base::OutputHandler {
            void put(const String&) JUTIL_CN_ JUTIL_OVERRIDE_;
        };
    }
    /**
        @class File

        This class handles file I/O.

        Files can be written to using file << string.

        File can be read from using file >> string.
        Note that this will only read one line of the file at a time.
    */
    class JUTIL_PUBLIC_ File : public io_base::OutputHandler, public io_base::InputHandler {
    public:

        enum JUTIL_PUBLIC_ Mode {
            READ,
            WRITE,
            APPEND,
            READ_WRITE,
            OVERWRITE
        };

        File(const String&, Mode = READ_WRITE) JUTIL_N_;
        File(const File&) JUTIL_N_;
        #ifdef JUTIL_CPP11
            File(File&&) noexcept;
            File &operator=(File&&) noexcept;
        #endif
        File &operator=(const File&) JUTIL_N_;
        bool eof() JUTIL_CN_;
        File &setPosition(int) JUTIL_N_;
        long length() JUTIL_CN_;
        int next() JUTIL_N_;
        size_t getPosition() JUTIL_CN_;
        String path() JUTIL_CN_;
        virtual ~File() JUTIL_N_;
    private:
        String filePath;
        Mode mode;
        _iobuf *file;
        bool fileEnd;
        void put(const String&) JUTIL_CN_ JUTIL_OVERRIDE_;
        String scan() JUTIL_N_ JUTIL_OVERRIDE_;
    };

    /**
        @section EXTERN

        These extern variables are defined by jutil.

        out:    Writes the string passed to put(String) to stdout.
                e.g. jutil::out << "One: " << 1 << jutil::endl;

        err:    Identical to out, except that it outputs to stderr,
                rather than stdout.

        in:     Reads data from stdin.
                e.g. jutil::in >> (jutil::String)var;

        endl:   String equating a linebreak character.

        If you'd like to use a custom class with out or err,
        Simply define a conversion operator from your class to a jutil::String.
    */
    extern io_interface::Out out;
    extern io_interface::In in;
    extern io_interface::Err err;
    extern const String endl;
}

#endif //JUTIL_IO_H
