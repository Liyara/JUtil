#include "JUtil/IO/IO.h"
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <dirent.h>
#include <JUtil/Traits/TypeManipulators.hpp>

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
    char __sep__[2] = "\\";
	#ifndef PATH_MAX
		#define PATH_MAX MAX_PATH
	#endif
#elif defined JUTIL_LINUX
    char __sep__[2] = "/";
#endif

namespace jutil JUTIL_PRIVATE_ {

    template<typename T>
    int putBuffer(StringBase<T> data, IOTarget *target, int (*putfunc)(const T*, FILE*), int (*printfunc)(T*, size_t, const T*, ...), size_t (*lenfunc)(const T*), const T *format) {
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
    void defaultCommandHandler(const io_base::OutputHandler<T>* caller, IOCommand command) {
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

    IOTarget::IOTarget() : stringIndex(0) {

    }

    IOTarget::TargetType IOTarget::getTargetType() const {
        return targetType;
    }

    void *const IOTarget::buffer() const {
        return realTarget;
    }

    IOTarget *makeIOTarget(IOBuffer f) {
        IOTarget *r = new IOTarget();
        r->realTarget = f;
        r->targetType = IOTarget::BUFFER_TARGET;
        return r;
    }

    IOTarget *_stdout_ = makeIOTarget((IOBuffer)stdout);
    IOTarget *_stderr_ = makeIOTarget((IOBuffer)stderr);
    IOTarget *_stdin_ = makeIOTarget((IOBuffer)stdin);

     OutputStream           out(_stdout_);
     WideOutputStream       wout(_stdout_);
     DataOutputStream       dout(_stdout_);
     ErrorOutputStream      err(_stderr_);
     WideErrorOutputStream  werr(_stderr_);
     InputStream            in(_stdin_);
     WideInputStream        win(_stdin_);

     const IOCommand     endl(COMMAND_NEWLINE);
     const IOCommand     flush(COMMAND_FLUSH);


    OutputStream::OutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<char>, t) {

    }

    void OutputStream::put(const String &data) const {
        putBuffer<char>(data, otarget, fputs, snprintf, strlen, "%s");
    }


    WideOutputStream::WideOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<wchar_t>, t) {

    }

    void WideOutputStream::put(const WideString &data) const {
        #ifndef JUTIL_MINGW_32
            putBuffer<wchar_t>(data, otarget, fputws, swprintf, wcslen, L"%ls");
        #endif
    }


    DataOutputStream::DataOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<Byte>, t) {

    }

    void DataOutputStream::put(const ByteString &data) const {
		Byte *str = new Byte[data.size()];
        data.array(str);
        char *hex = (char*)malloc(data.size() * 2);

        for (size_t i = 0; i < data.size(); ++i) sprintf(hex + (i * 2), "%02X", str[i]);

        fputs(hex, stdout);

        free(hex);

		delete[] str;
    }


    ErrorOutputStream::ErrorOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<char>, t) {

    }

    void ErrorOutputStream::put(const String &data) const {
        putBuffer<char>(data, otarget, fputs, snprintf, strlen, "%s");
    }


    WideErrorOutputStream::WideErrorOutputStream(IOTarget *t) : OutputHandler(defaultCommandHandler<wchar_t>, t) {

    }

    void WideErrorOutputStream::put(const WideString &data) const {
        #ifndef JUTIL_MINGW_32
            putBuffer<wchar_t>(data, otarget, fputws, swprintf, wcslen, L"%ls");
        #endif
    }

    InputStream::InputStream(IOTarget *t) : InputHandler(t) {

    }

    String InputStream::scan(size_t bytes) const {
		char *cstr = new char[bytes];
        fgets(cstr, bytes, (FILE*)(itarget->buffer()));
		String r(cstr);
		delete[] cstr;
        return r;
    }

    WideInputStream::WideInputStream(IOTarget *t) : InputHandler(t) {

    }

    WideString WideInputStream::scan(size_t words) const {
		wchar_t *cstr = new wchar_t[words];
        fgetws(cstr, words, (FILE*)(itarget->buffer()));
		WideString r(cstr);
		delete[] cstr;
        return r;
    }

    inline void reportInvalidFile(char *cfpath) {
        err << "Could not find file \"" << String(cfpath) << "\"!" << endl;
    }

    bool fileExists(char *cfpath) {
        bool r = false;
        FILE *temp = fopen(cfpath, "r");
        if (temp) {
            r = true;
            fclose(temp);
        }
        return r;
    }

    void emptyFile(char *cfpath) {
        FILE *temp = fopen(cfpath, "w");
        if (temp) fclose(temp);
    }

    bool handleFileInitialization(char *cfpath, unsigned openMode) {
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


    FileBase::FileBase(const String &fpath, unsigned openMode) : path(fpath), accessMode(openMode), m((char*)malloc(8)), fileOpen(false) {
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

    FileBase::~FileBase() {
        if (target && target->buffer()) fclose((FILE*)(target->buffer()));
    }

    unsigned FileBase::mode() const {
        return accessMode;
    }
    size_t FileBase::getPosition() const {
        return ftell((FILE*)(target->buffer()));
    }
    bool FileBase::setPosition(size_t p) {
        FILE *file = (FILE*)(target->buffer());
        if (p >= 0) {
            if (fseek(file, p, SEEK_SET)) return false;
        } else {
            if (fseek(file, p, SEEK_END)) return false;
        }
        return true;
    }

    bool File::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetc(fgetc(file), file);
        return c == EOF;
    }

    File::File(const String &fpath, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR) {
        openFile();
    }

    bool File::openFile() {
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

    String File::scan(size_t) const {
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

    void File::dump(String *dest) const {
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

    void File::put(const String &data) const {
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

    size_t File::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }





    bool WideFile::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetwc(fgetwc(file), file);
        return c == EOF;
    }

    WideFile::WideFile(const String &fpath, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR) {
        openFile();
    }

    bool WideFile::openFile() {
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

    WideString WideFile::scan(size_t) const {
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

    void WideFile::dump(WideString *dest) const {
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

    void WideFile::put(const WideString &data) const {
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

    size_t WideFile::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }






    bool BinaryFile::eof() const {
        FILE *file = (FILE*)(target->buffer());
        int c = ungetc(fgetc(file), file);
        return c == EOF;
    }

    BinaryFile::BinaryFile(const String &fpath, size_t cs, unsigned openMode) : FileBase(fpath, openMode), OutputHandler(JUTIL_NULLPTR, JUTIL_NULLPTR), InputHandler(JUTIL_NULLPTR), chunkSize(cs) {
        openFile();
    }

    bool BinaryFile::openFile() {
        char *cpath = new char[path.size() + 1];
        path.array(cpath);

        size_t epos = 0;
        for (epos; epos < strlen(m); ++epos) {
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

    ByteString BinaryFile::scan(size_t) const {
        if (accessMode & READ) {
            Byte *buf = new Byte[chunkSize];
			ByteString r;
            size_t cread = 0;
            if (cread = fread(buf, 1, chunkSize, (FILE*)(target->buffer()))) {
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

    void BinaryFile::dump(ByteString *dest) const {
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

    void BinaryFile::put(const ByteString &data) const {
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

    size_t BinaryFile::length() const {
        FILE *file = (FILE*)(target->buffer());
        size_t cpos = ftell(file);
        fseek(file, 0L, SEEK_END);
        size_t result = ftell(file);
        fseek(file, cpos, SEEK_SET);
        return result;
    }

    DirectoryEntry::DirectoryEntry(const String &n, const String &d, Type t) : name(n), directory(d), fullPath(d + __sep__ + n), type(t) {

    }

    Directory::Directory(const String &str) {
        char *cstr = new char[str.size() + 1];
        char *rpath = new char[PATH_MAX + 1];
        str.array(cstr);
        DIR *dir = opendir(cstr);
        if (dir) {
            _success = true;
            handle = (void*)dir;
            #ifdef JUTIL_LINUX
            	realpath(cstr, rpath);
            #elif defined JUTIL_WINDOWS
            	_fullpath(rpath, cstr, PATH_MAX);
            #endif
            path = rpath;
            struct dirent *entry;
            while (entry = readdir(dir)) {
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
        } else {
            _success = false;
            handle = JUTIL_NULLPTR;
        }
    }

    bool Directory::valid() const {
        return _success;
    }
    const Queue<DirectoryEntry> &Directory::entries() const {
        return _entries;
    }

    Directory::~Directory() {
        if (handle) closedir((DIR*)handle);
    }
}
