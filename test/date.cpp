#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "../util/util.h"
#include "../util/date.h"
}

class DateTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.


  DateTest() {
    // You can do set-up work for each test here.
  }

  virtual ~DateTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

TEST_F(DateTest, TimezoneShift) {
    struct timeval tv;
    struct timezone tz;
    time_t seconds;
    int min_west;
    time_t seconds_adjustment;

    gettimeofday(&tv, &tz);
    min_west = tz.tz_minuteswest;

    tz.tz_dsttime = 0;
    seconds = tv.tv_sec;
    seconds_adjustment = min_west * 60;
    seconds = seconds - seconds_adjustment;
    EXPECT_EQ(seconds, timezone_shift(&tv, &tz));

    tz.tz_dsttime = 1;
    seconds = tv.tv_sec;
    min_west -= 60;
    seconds_adjustment = min_west * 60;
    seconds = seconds - seconds_adjustment;
    EXPECT_EQ(seconds, timezone_shift(&tv, &tz));
}


TEST_F(DateTest, FormatCalendarDate) {
    CalendarDate *cd;
    char *date_string;
    struct timeval tv;

    cd = create_base_calendar_date();
    date_string = format_calendar_date(cd);
    EXPECT_STREQ("January 1, 1970 00:00:00.000000", date_string);

    tv.tv_sec = 1392606296;
    tv.tv_usec = 123456;
    cd = compute_calendar_date(&tv);
    date_string = format_calendar_date(cd);
    EXPECT_STREQ("February 17, 2014 03:04:56.123456", date_string);
}

TEST_F(DateTest, MonthString) {

    EXPECT_STREQ("January", month_string(JAN));
    EXPECT_STREQ("May", month_string(MAY));
    EXPECT_STREQ("December", month_string(DEC));
}

TEST_F(DateTest, YearPlusRemainingDays) {
    int start_year;
    int days_beyond;
    YearPlusDays *ypd;

    start_year = 1970;
    days_beyond = 0;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1970, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1971, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365 + 365 + 366; /* 1970, 1971, 1972 */
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1973, ypd->year);
    EXPECT_EQ(0, ypd->remaining_days);

    start_year = 1970;
    days_beyond = 365 + 365 + 366 + 364;
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(1973, ypd->year);
    EXPECT_EQ(364, ypd->remaining_days);

    start_year = 2012;
    days_beyond = 366 + 365 + 3; /* 2012, 2013 and three days */
    ypd = year_plus_remaining_days(start_year, days_beyond);
    EXPECT_EQ(2014, ypd->year);
    EXPECT_EQ(3, ypd->remaining_days);

    free(ypd);
}

TEST_F(DateTest, MonthPlusDays) {
    int current_year;
    enum months_in_year start_month;
    int days_beyond;
    MonthPlusDays *mpd;

    current_year = 1970;
    start_month = JAN;
    days_beyond = 0;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(JAN, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    current_year = 1970;
    start_month = JAN;

    days_beyond = 3;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(JAN, mpd->month);
    EXPECT_EQ(3, mpd->remaining_days);

    days_beyond = 31;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(FEB, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    days_beyond = 32;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(FEB, mpd->month);
    EXPECT_EQ(1, mpd->remaining_days);

    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(MAR, mpd->month);
    EXPECT_EQ(1, mpd->remaining_days);

    current_year = 2012;
    start_month = JAN;
    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(MAR, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    start_month = FEB;
    days_beyond = 60;
    mpd = month_plus_remaining_days(current_year, start_month, days_beyond);
    EXPECT_EQ(APR, mpd->month);
    EXPECT_EQ(0, mpd->remaining_days);

    free(mpd);
}

TEST_F(DateTest, MonthsInYear) {
    int *days_per_month = months_in_year();
    EXPECT_EQ(31, days_per_month[JAN]);
    EXPECT_EQ(28, days_per_month[FEB]);
    EXPECT_EQ(30, days_per_month[APR]);
    EXPECT_EQ(31, days_per_month[DEC]);
}

TEST_F(DateTest, NextMonth) {
    EXPECT_EQ(FEB, next_month(JAN));
    EXPECT_EQ(MAR, next_month(FEB));
    EXPECT_EQ(JAN, next_month(DEC));
}

TEST_F(DateTest, CreateBaseCalendarDate) {
    CalendarDate *cd = create_base_calendar_date();
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(1, cd->day);
    EXPECT_EQ(0, cd->hour);
    EXPECT_EQ(0, cd->min);
    EXPECT_EQ(0, cd->sec);
    EXPECT_EQ(0, cd->usec);
}

TEST_F(DateTest, IsLeapYear) {
    EXPECT_EQ(1, is_leap_year(2400));
    EXPECT_EQ(1, is_leap_year(2104));
    EXPECT_EQ(0, is_leap_year(2100));
    EXPECT_EQ(1, is_leap_year(2016));
    EXPECT_EQ(0, is_leap_year(2014));
    EXPECT_EQ(1, is_leap_year(2012));
    EXPECT_EQ(1, is_leap_year(2000));
    EXPECT_EQ(1, is_leap_year(1976));
    EXPECT_EQ(0, is_leap_year(1970));
    EXPECT_EQ(1, is_leap_year(1968));
    EXPECT_EQ(0, is_leap_year(1900));
    EXPECT_EQ(0, is_leap_year(1800));
    EXPECT_EQ(1, is_leap_year(1600));
    EXPECT_EQ(0, is_leap_year(300));
    EXPECT_EQ(1, is_leap_year(4));
    EXPECT_EQ(1, is_leap_year(0));
}

TEST_F(DateTest, RelationToBaseDate) {
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int relation;
    relation = relation_to_base_date(&tv);
    EXPECT_EQ(AFTER, relation);

    tv.tv_sec = -1;
    tv.tv_usec = 999999;
    relation = relation_to_base_date(&tv);
    EXPECT_EQ(BEFORE, relation);
}

TEST_F(DateTest, ComputeCalendarDateError) {
    struct timeval tv;
    tv.tv_sec = -1;
    tv.tv_usec = 999999;
    EXPECT_EQ(NULL, compute_calendar_date(&tv));
}

TEST_F(DateTest, DecomposeTimeval) {
    struct timeval tv;
    tv.tv_sec = 1392606296;
    tv.tv_usec = 123456;
    DecomposedTimeval *dtv;
    dtv = decompose_timeval(&tv);
    EXPECT_EQ(16118, dtv->days);
    EXPECT_EQ(3, dtv->hours);
    EXPECT_EQ(4, dtv->minutes);
    EXPECT_EQ(56, dtv->seconds);

    tv.tv_sec = 1392591896;
    dtv = decompose_timeval(&tv);
    EXPECT_EQ(16117, dtv->days);
}

TEST_F(DateTest, ComputeYearMonthDay) {
    int days_since;
    CalendarDate *cd;
    days_since = 0;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(1, cd->day);

    days_since = 1;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(2, cd->day);

    days_since = 16118;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(2014, cd->year);
    EXPECT_EQ(FEB, cd->month);
    EXPECT_EQ(17, cd->day);

    days_since = 16131;
    cd = compute_year_month_day(days_since);
    EXPECT_EQ(2014, cd->year);
    EXPECT_EQ(MAR, cd->month);
    EXPECT_EQ(2, cd->day);
}



TEST_F(DateTest, NumDaysInFeb) {
    int num_days;
    num_days = num_days_in_feb(2012);
    EXPECT_EQ(29, num_days);
    num_days = num_days_in_feb(2014);
    EXPECT_EQ(28, num_days);
}

TEST_F(DateTest, NumDaysInYear) {
    int num_days;
    num_days = num_days_in_year(2012);
    EXPECT_EQ(366, num_days);
    num_days = num_days_in_year(2014);
    EXPECT_EQ(365, num_days);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
