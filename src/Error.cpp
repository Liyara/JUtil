#include "Core/Error.h"
#include "IO/IO.h"
#include <cstdlib>

namespace jutil JUTIL_PRIVATE_ {

    List<Error*> stack;

    Error &Error::write(int o, int i, const char *m, const char *f) JUTIL_N_ {
        stack.insert(new Error(o, i, m, f));
        stack.last()->written = true;
        return *(stack.last());
    }
    Error &Error::read(size_t i) JUTIL_N_ {
        return *(stack[i]);
    }
    size_t Error::stackSize() JUTIL_N_ {
        return stack.size();
    }
    bool Error::has(Error *e) JUTIL_N_ {
        return stack.find(e);
    }
    void Error::handle() JUTIL_N_ {
        if (!silenced) {
            jutil::err << "ERROR [" << outer << "." << inner <<  "]: " << msg << " at " << func << jutil::endl;
        }
        destroySelf();
    }
    void Error::handleStack() JUTIL_N_ {
        for (List<Error*>::Iterator it = stack.begin(); it != stack.end(); ++it) {
            (*it)->handle();
        }
    }
    void Error::destroyStack() JUTIL_N_ {
        for (List<Error*>::Iterator it = stack.begin(); it != stack.end(); ++it) {
            (*it)->destroySelf();
        }
    }
    void Error::handleTop() JUTIL_N_ {
        stack.last()->handle();
    }
    void Error::handleBottom() JUTIL_N_ {
        stack.first()->handle();
    }
    Error::~Error() JUTIL_N_ {
        destroySelf();
    }
    Error &Error::silence(bool s) JUTIL_N_ {
        silenced = s;
        return *this;
    }
    JUTIL_CX_ Error::Error(int o, int i, const char *m, const char *f) JUTIL_N_ : outer(o), inner(i), msg(m), func(f), written(false), silenced(false) {}
    void Error::destroySelf() JUTIL_N_ {
        if (written) {
            size_t p;
            if (stack.find(this, &p)) {
                stack.erase(p);
                delete this;
            }
        }
    }
    JUTIL_CX_ FatalError::FatalError(int o, int i, const char *m, const char *f) JUTIL_N_ : Error(o, i, m, f) {}
    void FatalError::write(int o, int i, const char *m, const char *f) JUTIL_N_ {
        (new FatalError(o, i, m, f))->handle();
    }
    void FatalError::handle() JUTIL_N_ {
        jutil::err << "FATAL ERROR [" << outer << "." << inner <<  "]: " << msg << " at " << func << jutil::endl;
        exit(outer + inner);
    }
}
