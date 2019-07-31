#include <JUtil/Core/Clock.h>
#include <time.h>

#ifdef JUTIL_CPP11
	#include <JUtil/Traits/Traits.hpp>
#endif

#ifdef JUTIL_WINDOWS
	#include <windows.h>
#endif

#define ASPECT_TABLE {SECONDS, MINUTES, HOURS, DAYS, MONTHS, YEARS}

namespace jutil {
	namespace Clock {

		time_t getTimer() {
			return time(NULL);
		}

		tm *getLocalTime() {
			time_t tt = getTimer();
			return localtime(&tt);
		}

		tm *getUniversalTime() {
			time_t tt = getTimer();
			return gmtime(&tt);
		}

		tm *dtAsTm(const DateTime &dt) {
			tm *_time = getLocalTime();
			_time->tm_sec = dt.second();
			_time->tm_min = dt.minute();
			_time->tm_hour = dt.hour();
			_time->tm_mday = dt.day();
			_time->tm_year = dt.year() - 1900;
			if (_time->tm_year < 0) _time->tm_year = 0;
			_time->tm_mon = dt.month();
			_time->tm_isdst = dt.dsOrS();
			return _time;
		}

		DateTime::DateTime(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values, int tz) 
			: DateTime(aspects, values, false, tz) {}

		DateTime::DateTime(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values, bool dst, int tz) 
			: DateTime(aspects, values, dst, NO_WEEKDAY, tz) {
				determineWeekday();
/*			}	*/
		}

		DateTime::DateTime(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values, Weekday wd, int tz) 
			: DateTime(aspects, values, false, wd, tz) {}

		DateTime::DateTime(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values, bool dst, Weekday wd, int tz) 
			: zone(dst), isdst(dst), weekday(wd) {
				for (size_t i = 0; i < data.size(); ++i) {
					size_t j;
					for (j = 0; j < aspects.size(); ++j) {
						if ((size_t)aspects[j] == static_cast<int>(i)) {
							data[i] = values[j];
							break;
						}
					}

					if (j < data.size()) continue;

					data[i] = 0;
				}
/*			}	*/
		}

		DateTime::DateTime(int tz) : zone(tz) {
			tm *_time = nullptr;
			DateTime *helper = nullptr;
			if (tz && tz != LOCAL) {
				if (zone < -24) zone = -24;
				if (zone > 24) zone = 24;
				helper = new DateTime(UTC);
				if (zone < 0) helper->offsetback({HOURS}, zone * -1);
				else helper->offset({HOURS}, zone);
				*this = *helper;
				zone = tz;
			} else if (tz == LOCAL) {
				helper = new DateTime(UTC);
				_time = getLocalTime();
				zone = 0;
			} else {
				_time = getUniversalTime();
			}
			if (_time) {
				data[SECONDS] = _time->tm_sec;
				data[MINUTES] = _time->tm_min;
				data[HOURS] = _time->tm_hour;
				data[DAYS] = _time->tm_mday;
				data[YEARS] = 1900 + _time->tm_year;
				data[MONTHS] = _time->tm_mon + 1;
				isdst = _time->tm_isdst;
				weekday = (Weekday)(_time->tm_wday + 1);
				if (tz == LOCAL) {
					if (*helper > *this) {
						if (helper->hour() == hour()) {
							if (helper->day() != day()) {
								zone = -24;
							}
						} else {
							while (helper->hour() != hour() || day() != helper->day()) {
								helper->offsetback({HOURS}, 1);
								--zone;
								if (zone == -24) break;
							}
						}
					} else if (*helper < *this) {
						if (helper->hour() == hour()) {
							if (helper->day() != day()) {
								zone = 24;
							}
						} else {
							while (helper->hour() != hour() || day() != helper->day()) {
								helper->offset({HOURS}, 1);
								++zone;
								if (zone == 24) break;
							}
						}
					}
				}
			}
			if (helper) delete helper;
		}

		DateTime::DateTime(const DateTime &dt) {
			*this = dt;
		}

		#ifdef JUTIL_CPP11
			DateTime::DateTime(DateTime &&dt) {
				*this = move(dt);
			}
		#endif

		int DateTime::_max(TimeAspect aspect) const {

			Queue<int> switcher = {
				59,
				59,
				23,
				-1,
				12,
				9999
			};

			int rm = switcher[(size_t)aspect];

			if (rm == -1) {
				switch((Month)(data[MONTHS])) {
					case JANUARY:
					case MARCH:
					case MAY:
					case JULY:
					case AUGUST:
					case OCTOBER:
					case DECEMBER:
						return 31;

					case FEBRUARY:
						if (data[YEARS] % 4 == 0) return 29;
						else return 28;

					case APRIL:
					case JUNE:
					case SEPTEMBER:
					case NOVEMBER:
					default: return 30;
				}
			}  else return rm;
		}

		int DateTime::_min(TimeAspect aspect) const {
			Queue<int> switcher = {
				0,
				0,
				0,
				1,
				1,
				1900
			};

			return switcher[(size_t)aspect];
		}

		void DateTime::offset(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values) {
			for (size_t i = 0; i < aspects.size(); ++i) {
				size_t t = (size_t)(aspects[i]);
				int adder = values[i];
				if (adder <= 0) continue;
				data[t] += adder;
				int mi = _min(aspects[i]);
				while (true) {
					int ma = _max(aspects[i]);
					if (data[t] <= ma) break;
					if (aspects[i] == YEARS) break;
					data[t] = (mi + (data[t] - ma)) - 1;
					offset({(TimeAspect)((size_t)(aspects[i]) + 1)}, {1});
				}
			}
		}

		void DateTime::offsetback(jutil::Queue<TimeAspect> aspects, jutil::Queue<int> values) {
			for (size_t i = 0; i < aspects.size(); ++i) {
				size_t t = (size_t)(aspects[i]);
				int adder = values[i];
				if (adder <= 0) continue;
				data[t] -= adder;
				int mi = _min(aspects[i]);
				while (true) {
					if (data[t] >= mi) break;
					if (aspects[i] == YEARS) break;
					offsetback({(TimeAspect)((size_t)(aspects[i]) + 1)}, {1});
					int ma = _max(aspects[i]);
					data[t] = ((ma + (1 - mi)) + data[t]);
				}
			}
		}

		int DateTime::compare(const DateTime &dt) const {
			for (size_t i = data.size() - 1; i >= 0; --i) {
				if (data[i] == dt.data[i]) {
					continue;
				} else if (data[i] > dt.data[i]) {
					return 0;
				} else {
					return 1;
				}
			}
			return -1;
		}

		void DateTime::determineWeekday() {
			tm *_time = dtAsTm(*this);
			mktime(_time);
			weekday = (Weekday)(_time->tm_wday + 1);
		}

		Queue<int> DateTime::createOffset(const DateTime &dt) {
			Queue<int> values;
			for (size_t i = 0; i < (size_t)__TIME_ASPECTS__; ++i) {
				values.insert(dt.data[i]);
			}
			return values;
		}

		DateTime &DateTime::operator=(const DateTime &dt) {
			for (size_t i = 0; i < data.size(); ++i) data[i] = dt.data[i];
			isdst = dt.isdst;
			weekday = dt.weekday;
			zone = dt.zone;
			return *this;
		}

		#ifdef JUTIL_CPP11
			DateTime &DateTime::operator=(DateTime &&dt) {
				for (size_t i = 0; i < data.size(); ++i) {
					data[i] = dt.data[i];
					dt.data[i] = 0;
				}
				isdst = dt.isdst;
				weekday = dt.weekday;
				zone = dt.zone;

				dt.isdst = false;
				dt.weekday = NO_WEEKDAY;
				dt.zone = UTC;

				return *this;
			}
		#endif

		DateTime DateTime::operator+(const DateTime &dt) const {
			DateTime ndt = *this;
			ndt.offset(ASPECT_TABLE, createOffset(dt));
			return ndt;
		}

		DateTime DateTime::operator-(const DateTime &dt) const {
			DateTime ndt = *this;
			ndt.offsetback(ASPECT_TABLE, createOffset(dt));
			return ndt;
		}

		DateTime &DateTime::operator+=(const DateTime &dt) {
			*this = *this + dt;
			return *this;
		}

		DateTime &DateTime::operator-=(const DateTime &dt) {
			*this = *this - dt;
			return *this;
		}

		bool DateTime::operator>(const DateTime &dt) const {
			return (compare(dt) == 0);
		}

		bool DateTime::operator<(const DateTime &dt) const {
			return (compare(dt) == 1);
		}

		bool DateTime::operator==(const DateTime &dt) const {
			return (compare(dt) == -1);
		}

		bool DateTime::operator!=(const DateTime &dt) const {
			return (compare(dt) != -1);
		}

		int DateTime::hour() const {
			return data[(size_t)HOURS];
		}

		int DateTime::minute() const {
			return data[(size_t)MINUTES];
		}

		int DateTime::second() const {
			return data[(size_t)SECONDS];
		}

		int DateTime::day() const {
			return data[(size_t)DAYS];
		}

		int DateTime::month() const {
			return data[(size_t)MONTHS];
		}

		int DateTime::year() const {
			return data[(size_t)YEARS];
		}

		bool DateTime::dsOrS() const {
			return isdst;
		}

		Weekday DateTime::dayOfWeek() const {
			return weekday;
		}

		DateTime DateTime::convert(int tz) {
			int diff = tz + (zone * -1);
			DateTime dt = *this;
			dt.offset({HOURS}, diff);
			return dt;
		}

		DateTime &DateTime::setDS(bool ds) {
			if (isdst && !ds) {
				--zone;
				isdst = false;
				offsetback({HOURS}, 1);
			} else if (!isdst && ds) {
				++zone;
				isdst = true;
				offset({HOURS}, 1);
			}
			return *this;
		}

		DateTime &DateTime::setS(bool s) {
			return setDS(s);
		}

		DateTime::operator String() {
			return String(*(const_cast<const DateTime*>(this)));
		}

		DateTime::operator const String() const {
			String r;

			r += String("(UTC ") + (zone > 0? "+" : "") + String(zone) + ") ";

			Queue<String> mn = {
				"",
				"January",
				"Febuary",
				"March",
				"April",
				"May",
				"June",
				"July",
				"August",
				"September",
				"October",
				"November",
				"December"
			};

			r += mn[month()] + " ";

			r += String(day()) + " ";

			r += String(year());

			r += '\t';

			r += String(hour()) + ":" + String(minute()) + ":" + String(second());

			return r;
		}

		DateTime hours(int h) {
			return DateTime(ASPECT_TABLE, {0, 0, h, 0, 0, 0}, false, NO_WEEKDAY, 0);
		}

		DateTime minutes(int m) {
			return DateTime(ASPECT_TABLE, {0, m, 0, 0, 0, 0}, false, NO_WEEKDAY, 0);
		}

		DateTime seconds(int s) {
			return DateTime(ASPECT_TABLE, {s, 0, 0, 0, 0, 0}, false, NO_WEEKDAY, 0);
		}

		DateTime days(int d) {
			return DateTime(ASPECT_TABLE, {0, 0, 0, d, 0, 0}, false, NO_WEEKDAY, 0);
		}

		DateTime months(int m) {
			return DateTime(ASPECT_TABLE, {0, 0, 0, 0, m, 0}, false, NO_WEEKDAY, 0);
		}

		DateTime years(int y) {
			return DateTime(ASPECT_TABLE, {0, 0, 0, 0, 0, y}, false, NO_WEEKDAY, 0);
		}

		bool setLocalTime(const DateTime &dt) {
			#ifdef JUTIL_WINDOWS
				SYSTEMTIME systemTime = {
					(WORD)year(),
					(WORD)month(),
					(WORD)(weekday) - 1,
					(WORD)day(),
					(WORD)hour(),
					(WORD)minute(),
					(WORD)second(),
					0
				};

				return SetSystemTime(&systemTime);

			#elif defined JUTIL_LINUX
				tm *_time = dtAsTm(dt);
				time_t _timer = mktime(_time);
				return (stime(&_timer) == 0);
			#endif
		}

		#ifdef JUTIL_CPP11
			namespace literals {
				DateTime operator "" _h(__jutil_dt_literal__ arg) {
					return hours(arg);
				}
				DateTime operator "" _m(__jutil_dt_literal__ arg) {
					return minutes(arg);
				}
				DateTime operator "" _s(__jutil_dt_literal__ arg) {
					return seconds(arg);
				}
				DateTime operator "" _d(__jutil_dt_literal__ arg) {
					return days(arg);
				}
				DateTime operator "" _mon(__jutil_dt_literal__ arg) {
					return months(arg);
				}
				DateTime operator "" _y(__jutil_dt_literal__ arg) {
					return years(arg);
				}
			}
		#endif
	}
}