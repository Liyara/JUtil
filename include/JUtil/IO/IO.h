#ifndef JUTIL_IO_H
#define JUTIL_IO_H

#include "JUtil/Core/String.h"
#include <JUtil/Core/NonCopyable.h>

#define IO_ERR_INDEX 0x02

/**
    @section DESCRIPTION
    Contains classes used for interacting with input and output steams.
    This file is a part of JUtil. @see jutil.h.
*/
namespace jutil JUTIL_PUBLIC_ {

    typedef void* IOBuffer;
    typedef void *const IOBuffer_const;

    struct IOCommand {
        JUTIL_EXPL_ IOCommand(int v) : value(v) {}
        int value;
    };

    class IOTarget {
    public:
        enum TargetType {
            BUFFER_TARGET,
            STRING_TARGET
        };

        TargetType getTargetType() const;

        IOBuffer_const buffer() const;

    private:
        IOTarget();
        IOBuffer realTarget;

        TargetType targetType;

        size_t stringIndex;

        friend IOTarget *makeIOTarget(IOBuffer);
        template<typename T>
        friend IOTarget *makeIOTarget(StringBase<T>*);
    };

    IOTarget *makeIOTarget(IOBuffer);

    template<typename T>
    IOTarget *makeIOTarget(StringBase<T> *target) {
        IOTarget *r = new IOTarget();
        r->realTarget = (void*)target;
        r->targetType = IOTarget::STRING_TARGET;
        return r;
    }


    namespace io_base JUTIL_PRIVATE_ {

        template <typename T>
        class JUTIL_PRIVATE_ OutputHandler {
        public:

            typedef T Type;
            typedef StringBase<T> LocalString;

            OutputHandler(void (*c)(const OutputHandler<T>*, IOCommand), IOTarget *t) : commandHandler(c), otarget(t) {

            }

            template <typename U>
            const OutputHandler &operator<<(const U &data) const {
                put(LocalString(data));
                return *this;
            }

            const OutputHandler &operator<<(const IOCommand &command) const {
                commandHandler(this, command);
                return *this;
            }

            IOTarget *const getTarget() const {
                return otarget;
            }
        protected:
            virtual void put(const LocalString&) const = 0;
            void (*commandHandler)(const OutputHandler<T>*, IOCommand);
            IOTarget *otarget;
        };

        template <typename T>
        class JUTIL_PRIVATE_ InputHandler {
        public:

            typedef StringBase<T> LocalString;

            InputHandler(IOTarget *t) : itarget(t) {

            }

            template <typename U>
            const InputHandler &operator>>(U &buffer) const {
                LocalString input = scan(sizeof(buffer));
                if (input.size()) buffer = static_cast<U>(input);
                return *this;
            }
            IOTarget *const getTarget() const {
                return itarget;
            }
        protected:
            virtual LocalString scan(size_t) const = 0;
            IOTarget *itarget;
        };

    }

    enum {
        READ        = 0x01,
        WRITE       = 0x02,
        APPEND      = 0x04,
        ERASE       = 0x08,
        CREATE      = 0x10,
        INSERT      = 0x20
    };

    class JUTIL_PRIVATE_ FileBase : public jutil::NonCopyable {
    public:
        FileBase(const String&, unsigned =  READ | WRITE);
        unsigned mode() const;
        size_t getPosition() const;
        bool setPosition(size_t);
        bool open() const;
        virtual bool eof() const = 0;
        virtual size_t length() const = 0;
    protected:
        String path;
        unsigned accessMode;
        char *m;
        virtual bool openFile() = 0;
        bool fileOpen;
        IOTarget *target;
        virtual ~FileBase();
    };

    class JUTIL_PUBLIC_ File : public FileBase, public io_base::OutputHandler<char>, public io_base::InputHandler<char> {
    public:
        File(const String&, unsigned = READ | WRITE);
        bool eof() const JUTIL_OVERRIDE_;
        virtual size_t length() const JUTIL_OVERRIDE_;
        void dump(String*) const;
    private:
        void put(const String&) const JUTIL_OVERRIDE_;
        String scan(size_t) const JUTIL_OVERRIDE_;
        bool openFile() JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ WideFile : public FileBase, public io_base::OutputHandler<wchar_t>, public io_base::InputHandler<wchar_t> {
    public:
        WideFile(const String&, unsigned = READ | WRITE);
        bool eof() const JUTIL_OVERRIDE_;
        virtual size_t length() const JUTIL_OVERRIDE_;
        void dump(WideString*) const;
    private:
        void put(const WideString&) const JUTIL_OVERRIDE_;
        WideString scan(size_t) const JUTIL_OVERRIDE_;
        bool openFile() JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ BinaryFile : public FileBase, public io_base::OutputHandler<Byte>, public io_base::InputHandler<Byte> {
    public:
        BinaryFile(const String&, size_t = 1, unsigned = READ | WRITE);
        bool eof() const JUTIL_OVERRIDE_;
        virtual size_t length() const JUTIL_OVERRIDE_;
        void dump(ByteString*) const;
    private:
        void put(const ByteString&) const JUTIL_OVERRIDE_;
        ByteString scan(size_t) const JUTIL_OVERRIDE_;
        bool openFile() JUTIL_OVERRIDE_;
        size_t chunkSize;
    };

    struct JUTIL_PUBLIC_ DirectoryEntry {
            enum Type {
                FILE,
                DIRECTORY
            };
            String name;
            String directory;
            String fullPath;
            Type type;
            DirectoryEntry(const String&, const String&, Type t);
    };

    class JUTIL_PUBLIC_ Directory {
    public:
        Directory(const String&);
        bool valid() const;
        const Queue<DirectoryEntry> &entries() const;
        virtual ~Directory();
    private:
        bool _success;
        String path;
        Queue<DirectoryEntry> _entries;
        void *handle;
    };

    class JUTIL_PUBLIC_ OutputStream : public io_base::OutputHandler<char>, private jutil::NonCopyable {
    public:
        OutputStream(IOTarget*);
    protected:
        void put(const String&) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ WideOutputStream JUTIL_F_ : public io_base::OutputHandler<wchar_t>, private jutil::NonCopyable {
    private:
        public:
        WideOutputStream(IOTarget*);
    protected:
        void put(const WideString&) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ DataOutputStream JUTIL_F_ : public io_base::OutputHandler<Byte>, private jutil::NonCopyable {
        public:
        DataOutputStream(IOTarget*);
    protected:
        void put(const ByteString&) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ ErrorOutputStream JUTIL_F_ : public io_base::OutputHandler<char>, private jutil::NonCopyable {
    private:
        public:
        ErrorOutputStream(IOTarget*);
    protected:
        void put(const String&) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ WideErrorOutputStream JUTIL_F_ : public io_base::OutputHandler<wchar_t>, private jutil::NonCopyable {
    private:
        public:
        WideErrorOutputStream(IOTarget*);
    protected:
        void put(const WideString&) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ InputStream JUTIL_F_ : public io_base::InputHandler<char>, private jutil::NonCopyable {
    private:
        public:
        InputStream(IOTarget*);
    protected:
        String scan(size_t) const JUTIL_OVERRIDE_;
    };

    class JUTIL_PUBLIC_ WideInputStream JUTIL_F_ : public io_base::InputHandler<wchar_t>, private jutil::NonCopyable {
    private:
        public:
        WideInputStream(IOTarget*);
    protected:
        WideString scan(size_t) const JUTIL_OVERRIDE_;
    };

    //built-in IO streams for interacting with the shell.

    extern OutputStream           out;
    extern WideOutputStream       wout;
    extern DataOutputStream       dout;
    extern ErrorOutputStream      err;
    extern WideErrorOutputStream  werr;
    extern InputStream            in;
    extern WideInputStream        win;

    extern const IOCommand     endl;
    extern const IOCommand     flush;

}

#endif //JUTIL_IO_H
