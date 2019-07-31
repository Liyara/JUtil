#ifndef JUTIL_CLOCK_H
#define JUTIL_CLOCK_H

#include <JUtil/Container/Set.hpp>
#include <JUtil/Core/String.h>

namespace jutil {

	namespace Clock {

		enum Weekday {
			NO_WEEKDAY,
			SUNDAY,
			MONDAY,
			TEUSDAY,
			WEDNSDAY,
			THURSDAY,
			FRIDAY,
			SATURDAY
		};

		enum Month {
			NO_MONTH,
			JANUARY,
			FEBRUARY,
			MARCH,
			APRIL,
			MAY,
			JUNE,
			JULY,
			AUGUST,
			SEPTEMBER,
			OCTOBER,
			NOVEMBER,
			DECEMBER
		};

		enum Timezone {
			LOCAL = 25,
			SST = -11,
			NUT = SST,
			TAHT,
			CKT = TAHT,
			HST = TAHT,
			AKST,
			GAMT = AKST,
			PST,
			MST,
			CST,
			EAST = CST,
			GALT = CST,
			EST,
			PET = EST,
			CIST = EST,
			COT = EST,
			ECT = EST,
			AST,
			AMT = AST,
			BOT = AST,
			CLT = AST,
			FKT = AST,
			GYT = AST,
			PYT = AST,
			VET = AST,
			BRT,
			ART = BRT,
			GFT = BRT,
			PMST = BRT,
			PYST = BRT,
			ROTT = BRT,
			SRT = BRT,
			UYT = BRT,
			WGT = BRT,
			FNT,
			EGT,
			AZOT = EGT,
			CVT = EGT,
			UTC,
			GMT = UTC,
			WET = UTC,
			WT = UTC,
			CET,
			WAT = CET,
			CAT,
			EET = CAT,
			SAST = CAT,
			EAT,
			FET = EAT,
			MSK = EAT,
			GST,
			SCT = GST,
			AZT = GST,
			GET = GST,
			KUYT = GST,
			MUT = GST,
			RET = GST,
			SAMT = GST,
			PKT,
			AMST = PKT,
			AQTT = PKT,
			MAWT = PKT,
			MVT = PKT,
			ORAT = PKT,
			TFT = PKT,
			TJT = PKT,
			TMT = PKT,
			UZT = PKT,
			YEKT = PKT,
			BST,
			ALMT = BST,
			BTT = BST,
			IOT = BST,
			KGT = BST,
			OMST = BST,
			QYZT = BST,
			VOST = BST,
			WIB,
			CXT = WIB,
			DAVT = WIB,
			HOVT = WIB,
			ICT = WIB,
			KRAT = WIB,
			NOVT = WIB,
			AWST,
			BNT = AWST,
			CAST = AWST,
			CHOT = AWST,
			HKT = AWST,
			IRKT = AWST,
			MYT = AWST,
			PHT = AWST,
			SGT = AWST,
			ULAT = AWST,
			WITA = AWST,
			ACST,
			I = ACST,
			JST = ACST,
			KST = ACST,
			PWT = ACST,
			TLT = ACST,
			WIT = ACST,
			YAKT = ACST,
			AEST,
			CHUT = AEST,
			CHST = AEST,
			DDUT = AEST,
			PGT = AEST,
			VLAT = AEST,
			YAPT = AEST,
			KOST,
			MAGI = KOST,
			NCT = KOST,
			NFT = KOST,
			PONT = KOST,
			SAKT = KOST,
			SBT = KOST,
			SRET = KOST,
			VUT = KOST,
		};

		enum TimeAspect {
			SECONDS,
			MINUTES,
			HOURS,
			DAYS,
			MONTHS,
			YEARS,

			__TIME_ASPECTS__
		};

		class DateTime : public StringInterface {
		public:
			DateTime(jutil::Queue<TimeAspect>, jutil::Queue<int>, int = LOCAL);
			DateTime(jutil::Queue<TimeAspect>, jutil::Queue<int>, bool, int = LOCAL);
			DateTime(jutil::Queue<TimeAspect>, jutil::Queue<int>, Weekday, int = LOCAL);
			DateTime(jutil::Queue<TimeAspect>, jutil::Queue<int>, bool, Weekday, int = LOCAL);
			DateTime(int = LOCAL);

			DateTime(const DateTime&);

			DateTime &operator=(const DateTime&);
			DateTime operator+(const DateTime&) const;
			DateTime operator-(const DateTime&) const;
			DateTime &operator+=(const DateTime&);
			DateTime &operator-=(const DateTime&);

			bool operator>(const DateTime&) const;
			bool operator<(const DateTime&) const;
			bool operator==(const DateTime&) const;
			bool operator!=(const DateTime&) const;

			#ifdef JUTIL_CPP11
				DateTime(DateTime&&);
				DateTime &operator=(DateTime&&);
			#endif

			int hour() const;
			int minute() const;
			int second() const;
			int day() const;
			int month() const;
			int year() const;

			bool dsOrS() const;

			DateTime &setDS(bool);
			DateTime &setS(bool);

			Weekday dayOfWeek() const;

			DateTime convert(int);

			operator String() JUTIL_OVERRIDE_ ;
			operator const String() const JUTIL_OVERRIDE_;

		private:
			int zone;
			bool isdst;
			Weekday weekday;
			jutil::Set<static_cast<size_t>(__TIME_ASPECTS__), int> data;

			void determineWeekday();
			int _max(TimeAspect) const;
			int _min(TimeAspect) const;
			void offset(jutil::Queue<TimeAspect>, jutil::Queue<int>);
			void offsetback(jutil::Queue<TimeAspect>, jutil::Queue<int>);
			int compare(const DateTime&) const;
			static Queue<int> createOffset(const DateTime &dt);
		};

		DateTime hours(int);
		DateTime minutes(int);
		DateTime seconds(int);
		DateTime days(int);
		DateTime months(int);
		DateTime years(int);

		bool setLocalTime(const DateTime&);

		#ifdef JUTIL_CPP11
			namespace literals {

				typedef unsigned long long int __jutil_dt_literal__;

				DateTime operator "" _h(__jutil_dt_literal__);
				DateTime operator "" _m(__jutil_dt_literal__);
				DateTime operator "" _s(__jutil_dt_literal__);
				DateTime operator "" _d(__jutil_dt_literal__);
				DateTime operator "" _mon(__jutil_dt_literal__);
				DateTime operator "" _y(__jutil_dt_literal__);
			}
		#endif
	}
}

#endif