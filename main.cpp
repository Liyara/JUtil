#include <JUtil/jutil.h>
JUTIL_IMPL

int main() {
    jutil::Timer t;
    t.start();
    jutil::RNG rng(0, 10);
    jutil::Queue<int> queue({rng.get<int>(), rng.get<int>(), rng.get<int>()});
    for (auto &i: queue) {
        jutil::out << i << jutil::endl;
    }
    jutil::readCharacter();
    jutil::out << t.stop(jutil::MILLISECONDS) << jutil::endl;
}
