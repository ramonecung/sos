#include "gtest/gtest.h"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "../shell/shell.h"
#include "../util/util.h"
#include "../util/date.h"
#include "../memory/memory.h"
}

class DateTest : public ::testing::Test {
    protected:

  // You can remove any or all of the following functions if its body
  // is empty.


  DateTest() {
    // You can do set-up work for each test here.
    void *start_address;
    /* obtain chunk of memory from system for myMalloc and myFree */
    start_address = malloc(TOTAL_SPACE);
    if (start_address == 0) {
        efputs("initialize_shell: could not allocate system memory\n", ostrm);
        return;
    }
    initialize_memory(start_address, TOTAL_SPACE);
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

TEST_F(DateTest, YearsMonthsDays) {
    int start_year;
    int days_beyond;
    YearsMonthsDays *ymd;

    start_year = 1970;
    days_beyond = 0;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(0, ymd->years);
    EXPECT_EQ(0, ymd->months);
    EXPECT_EQ(0, ymd->days);

    days_beyond = 365;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(1, ymd->years);
    EXPECT_EQ(0, ymd->months);
    EXPECT_EQ(0, ymd->days);

    days_beyond = 365 + 365 + 366; /* 1970, 1971, 1972 */
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(3, ymd->years);
    EXPECT_EQ(0, ymd->months);
    EXPECT_EQ(0, ymd->days);

    days_beyond = 31;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(0, ymd->years);
    EXPECT_EQ(1, ymd->months);
    EXPECT_EQ(0, ymd->days);

    days_beyond = 32;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(0, ymd->years);
    EXPECT_EQ(1, ymd->months);
    EXPECT_EQ(1, ymd->days);

    days_beyond = 60;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(0, ymd->years);
    EXPECT_EQ(2, ymd->months);
    EXPECT_EQ(1, ymd->days);

    days_beyond = 365 + 365 + 366 + 364;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(3, ymd->years);
    EXPECT_EQ(11, ymd->months);
    EXPECT_EQ(30, ymd->days);

    start_year = 2012;
    days_beyond = 366 + 365 + 3; /* 2012, 2013 and three days */
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(2, ymd->years);
    EXPECT_EQ(0, ymd->months);
    EXPECT_EQ(3, ymd->days);

    days_beyond = 60;
    ymd = years_months_days(start_year, days_beyond);
    EXPECT_EQ(0, ymd->years);
    EXPECT_EQ(2, ymd->months);
    EXPECT_EQ(0, ymd->days);

    efree(ymd);
}

TEST_F(DateTest, DaysInMonth) {
    int *days_array = days_per_month();
    EXPECT_EQ(31, days_array[JAN]);
    EXPECT_EQ(28, days_array[FEB]);
    EXPECT_EQ(30, days_array[APR]);
    EXPECT_EQ(31, days_array[DEC]);
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
    EXPECT_EQ(44, dtv->years);
    EXPECT_EQ(1, dtv->months);
    EXPECT_EQ(16, dtv->days);
    EXPECT_EQ(3, dtv->hours);
    EXPECT_EQ(4, dtv->minutes);
    EXPECT_EQ(56, dtv->seconds);
}

TEST_F(DateTest, ComputeCalendarDate) {
    struct timeval tv;
    CalendarDate *cd;
    tv.tv_usec = 0;
    tv.tv_sec = 0;
    cd = compute_calendar_date(&tv);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(1, cd->day);

    tv.tv_sec = NUM_SECONDS_IN_DAY;
    cd = compute_calendar_date(&tv);
    EXPECT_EQ(1970, cd->year);
    EXPECT_EQ(JAN, cd->month);
    EXPECT_EQ(2, cd->day);

    tv.tv_sec = 16118 * NUM_SECONDS_IN_DAY;
    cd = compute_calendar_date(&tv);
    EXPECT_EQ(2014, cd->year);
    EXPECT_EQ(FEB, cd->month);
    EXPECT_EQ(17, cd->day);

    tv.tv_sec = 16131 * NUM_SECONDS_IN_DAY;
    cd = compute_calendar_date(&tv);
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
