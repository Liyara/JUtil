#ifndef JUTIL_IO_H
#define JUTIL_IO_H

#include "JUtil/Core/String.h"
#include <JUtil/Core/NonCopyable.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>
#include <JUtil/Traits/TypeManipulators.hpp>
#include <sys/stat.h>
#include <sys/types.h>

#define IO_ERR_INDEX 0x02

#define BUFFER_LENGTH   1022

#ifdef JUTIL_WINDOWS
    #include <io.h>
    #include <fcntl.h>
#endif // JUTIL_WINDOWS

#include <JUtil/Core/Pair.hpp>

#include <iostream>

#define COMMAND_NEWLINE 0
#define COMMAND_FLUSH   1

#ifdef JUTIL_WINDOWS
	#include <windows.h>
    extern char __sep__[2];
    #define _JUTIL_IO_IMPL char __sep__[2] = "\\";
	#ifndef PATH_MAX
		#define PATH_MAX MAX_PATH
	#endif
#elif defined JUTIL_LINUX
    extern char __sep__[2];
    #define _JUTIL_IO_IMPL char __sep__[2] = "/";
#endif

#define JUTIL_IO_IMPL _JUTIL_IO_IMPL \
    namespace jutil { \
        IOTarget *_stdout_ = makeIOTarget((IOBuffer)stdout); \
        IOTarget *_stderr_ = makeIOTarget((IOBuffer)stderr); \
        IOTarget *_stdin_ = makeIOTarget((IOBuffer)stdin); \
        OutputStream           out(_stdout_); \
        WideOutputStream       wout(_stdout_); \
        DataOutputStream       dout(_stdout_); \
        ErrorOutputStream      err(_stderr_); \
        WideErrorOutputStream  werr(_stderr_); \
        InputStream            in(_stdin_); \
        WideInputStream        win(_stdin_); \
        const IOCommand     endl(COMMAND_NEWLINE); \
        const IOCommand     flush(COMMAND_FLUSH); \
    }



/**
    @section DESCRIPTION
    Contains classes used for interacting with input and output steams.
    This file is a part of JUtil. @see jutil.h.
*/
namespace jutil  {

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


    namespace io_base  {

        template <typename T>
        class  OutputHandler {
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

            OutputHandler &operator<<(IOTarget *target) {
                otarget = target;
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
        class  InputHandler {
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

            InputHandler &operator>>(IOTarget *target) {
                itarget = target;
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

    class  FileBase : public NonCopyable {
    public:
        FileBase(const String&, unsigned =  READ | WRITE);
        unsigned mode() const;
        size_t getPosition() const;
        bool setPosition(size_t);
        bool open() const;
        const String &getPath() const;
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

    class  File : public FileBase, public io_base::OutputHandler<char>, public io_base::InputHandler<char> {
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

    class  WideFile : public FileBase, public io_base::OutputHandler<wchar_t>, public io_base::InputHandler<wchar_t> {
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

    class  BinaryFile : public FileBase, public io_base::OutputHandler<Byte>, public io_base::InputHandler<Byte> {
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

    struct  DirectoryEntry {
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

    class  Directory {
    public:
        Directory(const String&);
        bool valid() const;
        const Queue<DirectoryEntry> &entries() const;
        const String &getPath();
        virtual ~Directory();
    private:
        bool _success;
        String path;
        Queue<DirectoryEntry> _entries;
        void *handle;
    };

    class  OutputStream : public io_base::OutputHandler<char>, private NonCopyable {
    public:
        OutputStream(IOTarget*);
    protected:
        void put(const String&) const JUTIL_OVERRIDE_;
    };

    class  WideOutputStream JUTIL_F_ : public io_base::OutputHandler<wchar_t>, private NonCopyable {
    private:
        public:
        WideOutputStream(IOTarget*);
    protected:
        void put(const WideString&) const JUTIL_OVERRIDE_;
    };

    class  DataOutputStream JUTIL_F_ : public io_base::OutputHandler<Byte>, private NonCopyable {
        public:
        DataOutputStream(IOTarget*);
    protected:
        void put(const ByteString&) const JUTIL_OVERRIDE_;
    };

    class  ErrorOutputStream JUTIL_F_ : public io_base::OutputHandler<char>, private NonCopyable {
    private:
        public:
        ErrorOutputStream(IOTarget*);
    protected:
        void put(const String&) const JUTIL_OVERRIDE_;
    };

    class  WideErrorOutputStream JUTIL_F_ : public io_base::OutputHandler<wchar_t>, private NonCopyable {
    private:
        public:
        WideErrorOutputStream(IOTarget*);
    protected:
        void put(const WideString&) const JUTIL_OVERRIDE_;
    };

    class  InputStream JUTIL_F_ : public io_base::InputHandler<char>, private NonCopyable {
    private:
        public:
        InputStream(IOTarget*);
    protected:
        String scan(size_t) const JUTIL_OVERRIDE_;
    };

    class  WideInputStream JUTIL_F_ : public io_base::InputHandler<wchar_t>, private NonCopyable {
    private:
        public:
        WideInputStream(IOTarget*);
    protected:
        WideString scan(size_t) const JUTIL_OVERRIDE_;
    };

    //built-in IO streams for interacting with the shell.

    extern IOTarget
        *_stdout_,
        *_stderr_,
        *_stdin_
    ;

    extern OutputStream           out;
    extern WideOutputStream       wout;
    extern DataOutputStream       dout;
    extern ErrorOutputStream      err;
    extern WideErrorOutputStream  werr;
    extern InputStream            in;
    extern WideInputStream        win;

    extern const IOCommand     endl;
    extern const IOCommand     flush;

    template <typename T>
    struct PutFunc {
        typedef int (*Definition)(const T*, FILE*);
    };

    template <typename T>
    struct PrintFunc {
        typedef int (*Definition)(T*, size_t, const T*, ...);
    };

    template <typename T>
    struct PrintFunc_MINGW32 {
        typedef int (*Definition)(T*, size_t, const T*, ...);
    };

    template <typename T>
    struct LenFunc {
        typedef size_t (*Definition)(const T*);
    };

    #define PUT_FUNC        typename PutFunc<T>::Definition
    #ifndef JUTIL_MINGW_32
        #define PRINT_FUNC  typename PrintFunc<T>::Definition
    #else
         #define PRINT_FUNC typename PrintFunc_MINGW32<T>::Definition
    #endif
    #define LEN_FUNC        typename LenFunc<T>::Definition

    template<typename T>
    inline int putBuffer(StringBase<T> data, IOTarget *target, PUT_FUNC putfunc, PRINT_FUNC printfunc, LEN_FUNC lenfunc, const T *format) {
        T *str = new T[data.size() + 1];
        data.array(str);
        if (target->getTargetType() == IOTarget::BUFFER_TARGET) {
            putfunc(str, (FILE*)(target->buffer()));
			delete[] str;
            return 0;
        }
        else {
            //Edit the string's queue and update its length
            StringBase<T> *targetObject = static_cast<StringBase<T>*>(target->buffer());
            size_t nLength = targetObject->size() + lenfunc(str);
            targetObject->reserve(nLength);
            T *targetArray = targetObject->getArray();
            printfunc(targetArray + targetObject->size(), lenfunc(str), format, str);
            targetObject->resize(nLength);
			delete[] str;
            return 1;
        }
    }

    template<typename T>
    inline void defaultCommandHandler(const io_base::OutputHandler<T>* caller, IOCommand command) {
        switch(command.value) {

        case COMMAND_NEWLINE:
            (*caller) << StringBase<T>(static_cast<T>('\n'));
            break;

        case COMMAND_FLUSH:
            if (caller->getTarget()->getTargetType() == IOTarget::STRING_TARGET) static_cast<StringBase<T>*>(caller->getTarget()->buffer())->clear();
            else fflush((FILE*)(caller->getTarget()->buffer()));
            break;

        default:
            break;

        }
    }

    inline IOTarget::IOTarget() : stringIndex(0) {

    }

    inline IOTarget::TargetType IOTarget::getTargetType() const {
        return targetType;
    }

    inline void *const IOTarget::buffer() const {
        return realTarget;
    }

    inline IOTarget *makeIOTarget(IOBuffer f) {
        IOTarget *r = new IOTarget();
        r->realTarget = f;
        r->targetType = IOTarget::BUFFER_TARGET;
        return r;
    }

    inline OutputStream::OutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<char>, t) {

    }

    inline void OutputStream::put(const String &data) const {
        putBuffer<char>(data, otarget, fputs, snprintf, strlen, "%s");
    }


    inline WideOutputStream::WideOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<wchar_t>, t) {

    }

    inline void WideOutputStream::put(const WideString &data) const {
        #ifndef JUTIL_MINGW_32
            putBuffer<wchar_t>(data, otarget, fputws, swprintf, wcslen, L"%ls");
        #endif
    }


    inline DataOutputStream::DataOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<Byte>, t) {

    }

    inline void DataOutputStream::put(const ByteString &data) const {
		Byte *str = new Byte[data.size()];
        data.array(str);
        char *hex = (char*)malloc(data.size() * 2);

        for (size_t i = 0; i < data.size(); ++i) sprintf(hex + (i * 2), "%02X", str[i]);

        fputs(hex, stdout);

        free(hex);

		delete[] str;
    }


    inline ErrorOutputStream::ErrorOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<char>, t) {

    }

    inline void ErrorOutputStream::put(const String &data) const {
        putBuffer<char>(data, otarget, fputs, snprintf, strlen, "%s");
    }


    inline WideErrorOutputStream::WideErrorOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<wchar_t>, t) {

    }

    inline void WideErrorOutputStream::put(const WideString &data) const {
        #ifndef JUTIL_MINGW_32
            putBuffer<wchar_t>(data, otarget, fputws, swprintf, wcslen, L"%ls");
        #endif
    }

    inline InputStream::InputStream(IOTarget *t) : InputHandler(t) {

    }

    inline String InputStream::scan(size_t) const {
		char *cstr = new char[BUFFER_LENGTH];
        fgets(cstr, BUFFER_LENGTH, (FILE*)(itarget->buffer()));
        cstr[strcspn(cstr, "\r\n")] = 0;
		String r(cstr);
		delete[] cstr;
        return r;
    }

    inline WideInputStream::WideInputStream(IOTarget *t) : InputHandler(t) {

    }

    inline WideString WideInputStream::scan(size_t) const {
		wchar_t *cstr = new wchar_t[BUFFER_LENGTH];
        fgetws(cstr, BUFFER_LENGTH, (FILE*)(itarget->buffer()));
        cstr[wcscspn(cstr, L"\r\n")] = 0;
		WideString r(cstr);
		delete[] cstr;
        return r;
    }

    inline void reportInvalidFile(char *cfpath) {
        err << "Could not find file \"" << String(cfpath) << "\"!" << endl;
    }

    inline bool fileExists(char *cfpath) {
        bool r = false;
        FILE *temp = fopen(cfpath, "r");
        if (temp) {
            r = true;
            fclose(temp);
        }
        return r;
    }

    inline void emptyFile(char *cfpath) {
        FILE *temp = fopen(cfpath, "w");
        if (temp) fclose(temp);
    }

   inline bool handleFileInitialization(char *cfpath, unsigned openMode) {
        if (openMode & CREATE) {
            FILE *temp = fopen(cfpath, "a");
            if (temp) fclose(temp);
        } else {
            if (!fileExists(cfpath)) {
                reportInvalidFile(cfpath);
                return false;
            }
        }

        if (openMode & ERASE) {
            if (fileExists(cfpath)) {
                emptyFile(cfpath);
            }
        }

        return true;
    }


    inline FileBase::FileBase(const String &fpath, unsigned openMode) : path(fpath), accessMode(openMode), m((char*)malloc(8)), fileOpen(false) {
        char *cfpath = new char[fpath.size() + 1];
        fpath.array(cfpath);

        handleFileInitialization(cfpath, openMode);

        if (openMode & READ) {
            if (openMode & WRITE) {
                if (openMode & APPEND) {
                    sprintf(m, "%s", "a+");
                } else {
                    sprintf(m, "%s", "r+");
                }
            } else {
                if (openMode & APPEND) {
                    sprintf(m, "%s", "a+");
                } else {
                    sprintf(m, "%s", "r");
                }
            }
        } else {
            if (openMode & WRITE) {
                if (openMode & APPEND) {
                    sprintf(m, "%s", "a");
                } else {
                    sprintf(m, "%s", "r+");
                }
            } else {
                err << "Attempt to open file \"" << fpath << "\" with invalid permissions; please specify permissions using the READ and WRITE options." << endl;
            }
        }

		delete[] cfpath;
    }

    inline const String &FileBase::getPath() const {
        return path;
    }

    inline FileBase::~FileBase() {
        if (target && target->buffer()) fclose((FILE*)(target->buffer()));
    }

    inline unsigned FileBase::mode() const {
        return accessMode;
    }
    inline size_t FileBase::getPosition() const {
        return ftell((FILE*)(target->buffer()));
    }
    inline bool FileBase::setPosition(size_t p) {
        FILE *file = (FILE*)(target->buffer());
        if (p >= 0) {
            if (fseek(file, p, SEEK_SET)) return false;
        } else {
            if (fseek(file, p, SEEK_END)) return false;
        }
        return true;
    }
    
    inline bool FileBase::open() const {
        return fileOpen;
    }

    inline bool File::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetc(fgetc(file), file);
        return c == EOF;
    }

    inline File::File(const String &fpath, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR) {
        openFile();
    }

    inline bool File::openFile() {
        char *cpath = new char[path.size() + 1];
        path.array(cpath);
        target = makeIOTarget((IOBuffer)fopen(cpath, m));
        if (accessMode & READ) {
            itarget = target;
        } else {
            itarget = JUTIL_NULLPTR;
        }
        if (accessMode & WRITE) {
            otarget = target;
        } else {
            otarget = JUTIL_NULLPTR;
        }

		delete[] cpath;

        if (target->buffer()) {
            fileOpen = true;
        } else {
            err << "Unable to open file \"" << path << "\"!" << endl;
            return false;
        }

        fseek((FILE*)(target->buffer()), 0L, SEEK_SET);

        return true;
    }

    inline String File::scan(size_t) const {
        if (accessMode & READ) {
            char buf[BUFFER_LENGTH];
            if (fgets(buf, sizeof(buf), (FILE*)(target->buffer()))) {
                return String(buf);
            } else {
                return String("\0");
            }
        } else {
            err << "Illegal read request!" << endl;
            return String();
        }
    }

    inline void File::dump(String *dest) const {
        if (accessMode & READ) {
            String line;
            size_t fpos = ftell((FILE*)(target->buffer()));
            dest->clear();
            while (!eof()) {
                (*this) >> line;
                (*dest) += line;
            }
            fseek((FILE*)(target->buffer()), fpos, SEEK_SET);
        }
    }

    inline void File::put(const String &data) const {
        if (accessMode & WRITE) {
            FILE *file = (FILE*)(target->buffer());
            size_t fpos = ftell(file);
            if (accessMode & INSERT) {
                char *dat = new char[data.size() + 1];
                data.array(dat);
                fputs(dat, file);
				delete[] dat;
            } else {
                char buffer[BUFFER_LENGTH];
                char *dat = new char[data.size() + 1];
                data.array(dat);
                String fname = hash(path) + ".tmp";
                char *cfname = new char[fname.size() + 1];
                fname.array(cfname);
                FILE *tempFile = fopen(cfname, "w");
				delete[] dat;
				delete[] cfname;
                if (!tempFile) {
                    err << "Error creating buffer file!" << endl;
                } else {
                    while (!feof(file) && fread(buffer, 1, BUFFER_LENGTH - 1, file) > 0) {
                        buffer[BUFFER_LENGTH - 1] = 0;
                        fputs(buffer, tempFile);
                    }

                    fseek(file, fpos, SEEK_SET);
                    fputs(dat, file);

                    fclose(tempFile);
                    tempFile = fopen(cfname, "r");

                    while (!feof(tempFile) && fread(buffer, 1, BUFFER_LENGTH - 1, tempFile) > 0) {
                        buffer[BUFFER_LENGTH - 1] = 0;
                        fputs(buffer, file);
                    }

                    fclose(tempFile);
                    remove(cfname);
                }

            }

            if (accessMode & APPEND) fseek(file, fpos, SEEK_SET);
        } else {
            err << "Illegal write request!" << endl;
        }
    }

    inline size_t File::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }





    inline bool WideFile::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetwc(fgetwc(file), file);
        return c == EOF;
    }

    inline WideFile::WideFile(const String &fpath, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR) {
        openFile();
    }

    inline bool WideFile::openFile() {
        char *cpath = new char[path.size() + 1];
        path.array(cpath);
        target = makeIOTarget((IOBuffer)fopen(cpath, m));
        if (accessMode & READ) {
            itarget = target;
        } else {
            itarget = JUTIL_NULLPTR;
        }
        if (accessMode & WRITE) {
            otarget = target;
        } else {
            otarget = JUTIL_NULLPTR;
        }

		delete[] cpath;

        if (target->buffer()) {
            fileOpen = true;
        } else {
            err << "Unable to open file \"" << path << "\"!" << endl;
            return false;
        }

        fseek((FILE*)(target->buffer()), 0L, SEEK_SET);

        return true;
    }

    inline WideString WideFile::scan(size_t) const {
        if (accessMode & READ) {
            wchar_t buf[BUFFER_LENGTH];
            if (fgetws(buf, sizeof(buf), (FILE*)(target->buffer()))) {
                return WideString(buf);
            } else {
                return WideString("\0");
            }
        } else {
            err << "Illegal read request!" << endl;
            return WideString();
        }
    }

    inline void WideFile::dump(WideString *dest) const {
        if (accessMode & READ) {
            WideString line;
            size_t fpos = ftell((FILE*)(target->buffer()));
            dest->clear();
            while (!eof()) {
                (*this) >> line;
                (*dest) += line;
            }
            fseek((FILE*)(target->buffer()), fpos, SEEK_SET);
        }
    }

    inline void WideFile::put(const WideString &data) const {
        if (accessMode & WRITE) {
            FILE *file = (FILE*)(target->buffer());
            size_t fpos = ftell(file);
            if (accessMode & INSERT) {
                wchar_t *dat = new wchar_t[data.size() + 1];
                data.array(dat);
                fputws(dat, file);
				delete[] dat;
            } else {
                wchar_t buffer[BUFFER_LENGTH];
                wchar_t *dat = new wchar_t[data.size() + 1];
                data.array(dat);
                String fname = hash(path) + ".tmp";
                char *cfname = new char[fname.size() + 1];
                fname.array(cfname);
                FILE *tempFile = fopen(cfname, "w");
				delete[] dat;
				delete[] cfname;
                if (!tempFile) {
                    err << "Error creating buffer file!" << endl;
                } else {
                    while (!feof(file) && fread(buffer, 1, BUFFER_LENGTH - 1, file) > 0) {
                        buffer[BUFFER_LENGTH - 1] = 0;
                        fputws(buffer, tempFile);
                    }

                    fseek(file, fpos, SEEK_SET);
                    fputws(dat, file);

                    fclose(tempFile);
                    tempFile = fopen(cfname, "r");

                    while (!feof(tempFile) && fread(buffer, 1, BUFFER_LENGTH - 1, tempFile) > 0) {
                        buffer[BUFFER_LENGTH - 1] = 0;
                        fputws(buffer, file);
                    }

                    fclose(tempFile);
                    remove(cfname);
                }
            }
            if (accessMode & APPEND) fseek(file, fpos, SEEK_SET);
        } else {
            err << "Illegal write request!" << endl;
        }
    }

    inline size_t WideFile::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }






    inline bool BinaryFile::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetc(fgetc(file), file);
        return c == EOF;
    }

    inline BinaryFile::BinaryFile(const String &fpath, size_t cs, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR), chunkSize(cs) {
        openFile();
    }

    inline bool BinaryFile::openFile() {
        char *cpath = new char[path.size() + 1];
        path.array(cpath);

        size_t epos;
        for (epos = 0; epos < strlen(m); ++epos) {
            if (m[epos] == 0) break;
        }

        sprintf(m + epos, "%s", "b");

        target = makeIOTarget((IOBuffer)fopen(cpath, m));
        if (accessMode & READ) {
            itarget = target;
        } else {
            itarget = JUTIL_NULLPTR;
        }
        if (accessMode & WRITE) {
            otarget = target;
        } else {
            otarget = JUTIL_NULLPTR;
        }

		delete[] cpath;

        if (target->buffer()) {
            fileOpen = true;
        } else {
            err << "Unable to open file \"" << path << "\"!" << endl;
            return false;
        }

        fseek((FILE*)(target->buffer()), 0L, SEEK_SET);

        return true;
    }

    inline ByteString BinaryFile::scan(size_t) const {
        if (accessMode & READ) {
            Byte *buf = new Byte[chunkSize];
			ByteString r;
            size_t cread = 0;
            if ((cread = fread(buf, 1, chunkSize, (FILE*)(target->buffer())))) {
                r = makeByteString(buf, (cread < chunkSize? cread : chunkSize));
            } else {
                r = ByteString(0);
            }
			delete[] buf;
			return r;
        } else {
            err << "Illegal read request!" << endl;
            return ByteString();
        }
    }

    inline void BinaryFile::dump(ByteString *dest) const {
        if (accessMode & READ) {
            ByteString line;
            size_t fpos = ftell((FILE*)(target->buffer()));
            dest->clear();
            while (!eof()) {
                (*this) >> line;
                (*dest) += line;
            }
            fseek((FILE*)(target->buffer()), fpos, SEEK_SET);
        }
    }

    inline void BinaryFile::put(const ByteString &data) const {
        if (accessMode & WRITE) {
            FILE *file = (FILE*)(target->buffer());
            size_t fpos = ftell(file);
            if (accessMode & INSERT) {
                Byte *dat = new Byte[data.size() + 1];
                data.array(dat);
                fwrite(dat, 1, data.size(), file);
				delete[] dat;
            } else {
                Byte buffer[BUFFER_LENGTH];
                Byte *dat = new Byte[data.size() + 1];
                data.array(dat);
                String fname = hash(path) + ".tmp";
                char *cfname = new char[fname.size() + 1];
                fname.array(cfname);
                FILE *tempFile = fopen(cfname, "w");
				delete[] dat;
				delete[] cfname;
                if (!tempFile) {
                    err << "Error creating buffer file!" << endl;
                } else {

                    size_t bytesRead = 0;

                    while (!feof(file) && (bytesRead = fread(buffer, 1, BUFFER_LENGTH, file)) > 0) {
                        fwrite(buffer, 1, bytesRead, tempFile);
                    }

                    fseek(file, fpos, SEEK_SET);
                    fwrite(dat, 1, data.size(), file);

                    fclose(tempFile);
                    tempFile = fopen(cfname, "r");

                    while (!feof(tempFile) && (bytesRead = fread(buffer, 1, BUFFER_LENGTH, tempFile)) > 0) {
                        fwrite(buffer, 1, bytesRead, file);
                    }

                    fclose(tempFile);
                    remove(cfname);
                }
            }
            if (accessMode & APPEND) fseek(file, fpos, SEEK_SET);
        } else {
            err << "Illegal write request!" << endl;
        }
    }

    inline size_t BinaryFile::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }

    inline DirectoryEntry::DirectoryEntry(const String &n, const String &d, Type t) : name(n), directory(d), fullPath(d + __sep__ + n), type(t) {

    }

    inline Directory::Directory(const String &str) {
        char *cstr = new char[str.size() + 1];
        char *rpath = new char[PATH_MAX + 1];
        str.array(cstr);
        DIR *dir = opendir(cstr);
        if (!dir) {
            #ifdef JUTIL_LINUX
                int r = mkdir(cstr, 0777);
            #elif defined JUTIL_WINDOWS
                int r = mkdir(cstr);
            #endif
            if (r == -1) {
                _success = false;
                handle = JUTIL_NULLPTR;
                return;
            }
            dir = opendir(cstr);
            if (!dir) {
                _success = false;
                handle = JUTIL_NULLPTR;
                return;
            }
        }
        _success = true;
        handle = (void*)dir;
        #ifdef JUTIL_LINUX
            realpath(cstr, rpath);
        #elif defined JUTIL_WINDOWS
            _fullpath(rpath, cstr, PATH_MAX);
        #endif
        path = rpath;
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
                bool isFile = true;
                char fname[PATH_MAX + 1];
                strcpy(fname, rpath);
                strcat(fname, __sep__);
                strcat(fname, entry->d_name);
                DIR *tdir = opendir(fname);
                if (tdir) {
                    closedir(tdir);
                    isFile = false;
                }
                _entries.insert(DirectoryEntry(String(entry->d_name), path, (isFile? DirectoryEntry::Type::FILE : DirectoryEntry::Type::DIRECTORY)));
            }
        }
    }

    inline bool Directory::valid() const {
        return _success;
    }
    inline const Queue<DirectoryEntry> &Directory::entries() const {
        return _entries;
    }

    inline Directory::~Directory() {
        if (handle) closedir((DIR*)handle);
    }

    inline const String &Directory::getPath() {
        return path;
    }

}

#endif //JUTIL_IO_H
