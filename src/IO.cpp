#include "IO/IO.h"
#include <cstdio>

#define BUFFER_LENGTH   1022

namespace jutil JUTIL_PRIVATE_ {

    io_interface::Out out;
    io_interface::Err err;
    io_interface::In in;

    JUTIL_INIT(const String endl, String("\n"));
    JUTIL_CX_ io_base::OutputHandler::OutputHandler() JUTIL_N_ {}
    JUTIL_CX_ io_base::InputHandler::InputHandler() JUTIL_N_ {}
    void io_interface::Out::put(const String &data) JUTIL_CN_ {
        if (this == &out) {
            char str[data.size() + 1];
            data.array(str);
            fputs(str, stdout);
        } else {
            JUTIL_INIT(String msg , String("\
                Attempt to use invalid OutputHandler object {") + String((long long int)this) + String("}.\n\
                Please use jutil::out instead.\
            "));
            char msgcstr[msg.size() + 1];
            msg.array(msgcstr);
            fputs(msgcstr, stdout);
        }
    }
    void io_interface::Err::put(const String &data) JUTIL_CN_ {
        if (this == &err) {
            char str[data.size() + 1];
            data.array(str);
            fputs(str, stderr);
        } else {
            JUTIL_INIT(String msg , String("\
                Attempt to use invalid OutputHandler object {") + String((long long int)this) + String("}.\n\
                Please use jutil::err instead.\
            "));
            char msgcstr[msg.size() + 1];
            msg.array(msgcstr);
            fputs(msgcstr, stderr);
        }
    }
    String io_interface::In::scan() JUTIL_CN_ {
        if (this == &in) {
            char cstr[BUFFER_LENGTH];
            scanf("%s", cstr);
            return String(cstr);
        } else {
            return String("\
                Attempt to use invalid InputHandler object {") + String((long long int)this) + String("}.\n\
                Please use juti::in instead.\
            ");
        }
    }
    File::File(const String &path, Mode m, DataType dt) JUTIL_N_ : JUTIL_INIT(filePath, path), JUTIL_INIT(mode, m), JUTIL_INIT(dataType, dt), JUTIL_INIT(fileEnd, false) {
        String modeStr;
        switch(mode) {
            case READ: {
                modeStr = "r";
                break;
            }
            case WRITE: {
                modeStr = "w";
                fileEnd = true;
                break;
            }
            case APPEND: {
                modeStr = "a";
                fileEnd = true;
                break;
            }
            case READ_WRITE: {
                modeStr = "r+";
                break;
            }
            case OVERWRITE: {
                modeStr = "w+";
                fileEnd = true;
                break;
            }
        }
        char filecstr[path.size() + 1];
        char modecstr[modeStr.size() + 1];
        path.array(filecstr);
        modeStr.array(modecstr);
        file = fopen(filecstr, modecstr);
    }

    String File::scan() JUTIL_CN_ {
        if (mode != WRITE && mode != APPEND && mode != OVERWRITE) {
            char buf[BUFFER_LENGTH];
            if (fgets(buf, sizeof(buf), file)) {
                return String(buf);
            } else {
                return String("\0");
            }
        } else {
            return String();
        }
    }

    void File::put(const String &data) JUTIL_CN_ {
        if (mode != READ) {
            char dat[data.size() + 1];
            data.array(dat);
            fputs(dat, file);
        }
    }
    File::~File() JUTIL_N_ {
        fclose(file);
    }
    bool File::eof() JUTIL_CN_ {
        int c = ungetc(fgetc(file), file);
        return c == EOF;
    }
    File &File::setPosition(int p) JUTIL_N_ {
        if (p >= 0) {
            fseek(file, p, SEEK_SET);
        } else {
            fseek(file, p, SEEK_END);
        }
        if (p == length()) {
            fileEnd = true;
        } else {
            fileEnd = false;
        }
        return *this;
    }
    int File::next() JUTIL_N_ {
        JUTIL_INIT(int r, fgetc(file));
        if (ftell(file) == length()) {
            fileEnd = true;
        }
        return r;
    }
    long File::length() JUTIL_CN_ {
        JUTIL_INIT(long cpos, ftell(file));
        fseek(file, 0L, SEEK_END);
        JUTIL_INIT(long result, ftell(file));
        fseek(file, cpos, SEEK_SET);
        return result;
    }
    size_t File::getPosition() JUTIL_CN_ {return ftell(file);}
    File::File(const File &f) JUTIL_N_ : JUTIL_INIT(filePath, f.filePath), JUTIL_INIT(mode, f.mode), JUTIL_INIT(file, f.file), JUTIL_INIT(fileEnd, f.fileEnd) {}
    #ifdef JUTIL_CPP11
        File::File(File &&f) noexcept : filePath {f.filePath}, mode {f.mode}, file {f.file}, fileEnd {f.fileEnd} {
            fclose(f.file);
            f.fileEnd = true;
            f.mode = READ;
            f.filePath = String();
        }
        File &File::operator=(File &&f) noexcept {
            fclose(file);
            file = f.file;
            fileEnd = f.fileEnd;
            mode = f.mode;
            filePath = f.filePath;
            fclose(f.file);
            f.fileEnd = true;
            f.mode = READ;
            f.filePath = String();
            return *this;
        }
    #endif
    File &File::operator=(const File &f) JUTIL_N_ {
        fclose(file);
        file = f.file;
        fileEnd = f.fileEnd;
        mode = f.mode;
        filePath = f.filePath;
        return *this;
    }
    String File::path() JUTIL_CN_ {return filePath;}
}
