/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryservice.h>
#include <fnordmetric/sql/backends/csv/csvtableref.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/sql/parser.h>
#include <fnordmetric/sql/resultlist.h>
#include <fnordmetric/sql/tableref.h>
#include <fnordmetric/sql/tablescan.h>
#include <fnordmetric/sql/tablerepository.h>
#include <fnordmetric/sql/token.h>
#include <fnordmetric/sql/tokenize.h>
#include <fnordmetric/ui/canvas.h>
#include <fnordmetric/ui/svgtarget.h>
#include <fnordmetric/util/inputstream.h>
#include <fnordmetric/util/outputstream.h>
#include <fnordmetric/util/unittest.h>
#include <fnordmetric/util/runtimeexception.h>

using namespace fnordmetric::query;

UNIT_TEST(QueryTest);

static Parser parseTestQuery(const char* query) {
  Parser parser;
  parser.parse(query, strlen(query));
  return parser;
}

static void compareChart(
    fnordmetric::ui::Canvas* chart,
    const std::string& file_name) {
  auto output_stream = fnordmetric::util::FileOutputStream::openFile(
      "build/tests/tmp/" + file_name);

  fnordmetric::ui::SVGTarget target(output_stream.get());
  chart->render(&target);

  EXPECT_FILES_EQ(
    "test/fixtures/" + file_name,
    "build/tests/tmp/" + file_name);
}

class TestTableRef : public TableRef {
  int getColumnIndex(const std::string& name) override {
    if (name == "one") return 0;
    if (name == "two") return 1;
    if (name == "three") return 2;
    return -1;
  }
  void executeScan(TableScan* scan) override {
    int64_t one = 0;
    int64_t two = 100;
    for (int i = two; i > 0; --i) {
      std::vector<SValue> row;
      row.emplace_back(SValue(++one));
      row.emplace_back(SValue(two--));
      row.emplace_back(SValue((int64_t) (i % 2 ? 10 : 20)));
      if (!scan->nextRow(row.data(), row.size())) {
        return;
      }
    }
  }
};

class TestTable2Ref : public TableRef {
  int getColumnIndex(const std::string& name) override {
    if (name == "one") return 0;
    if (name == "two") return 1;
    if (name == "three") return 2;
    return -1;
  }
  void executeScan(TableScan* scan) override {
    for (int i = 10; i > 0; --i) {
      std::vector<SValue> row;
      row.emplace_back(SValue((int64_t) i));
      row.emplace_back(SValue((int64_t) (i * 2)));
      row.emplace_back(SValue((int64_t) (i % 2 ? 100 : 200)));
      if (!scan->nextRow(row.data(), row.size())) {
        return;
      }
    }
  }
};

TEST_CASE(QueryTest, TestDrawQueryNeedsSeriesColAssert, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTable2Ref()));

  auto query = Query(
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'series1' as fnord, one AS x, two AS y"
      "  FROM"
      "    testtable;",
      &repo);

  const char err[] = "can't draw SELECT because it has no 'series' column";

  EXPECT_EXCEPTION(err, [&query] () {
    query.execute();
  });
});

TEST_CASE(QueryTest, TestDrawQueryNeedsXColAssert, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTable2Ref()));

  auto query = Query(
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'series1' as series, one AS f, two AS y"
      "  FROM"
      "    testtable;",
      &repo);

  const char err[] = "can't draw SELECT because it has no 'x' column";

  EXPECT_EXCEPTION(err, [&query] () {
    query.execute();
  });
});

TEST_CASE(QueryTest, TestDrawQueryNeedsYColAssert, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTable2Ref()));

  auto query = Query(
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'series1' as series, one AS x, two AS f"
      "  FROM"
      "    testtable;",
      &repo);

  const char err[] = "can't draw SELECT because it has no 'y' column";

  EXPECT_EXCEPTION(err, [&query] () {
    query.execute();
  });
});

TEST_CASE(QueryTest, TestSimpleDrawQuery, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTable2Ref()));

  auto query = Query(
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'series1' as series, one AS x, two AS y"
      "  FROM"
      "    testtable;"
      ""
      "  SELECT"
      "    'series2' as series, one as x, two + 5 as y"
      "  from"
      "    testtable;"
      ""
      "  SELECT"
      "    'series3' as series, one as x, two / 2 + 4 as y"
      "  FROM"
      "    testtable;"
      "",
      &repo);

  query.execute();
  auto chart = query.getChart(0);

  compareChart(
      chart,
      "QueryTest_TestSimpleDrawQuery_out.svg.html");
});

TEST_CASE(QueryTest, TestDerivedSeriesDrawQuery, [] () {
  TableRepository repo;
  repo.addTableRef("testtable",
      std::unique_ptr<TableRef>(new TestTable2Ref()));

  auto query = Query(
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    one % 3 as series, one / 3 as x, two + one AS y"
      "  FROM"
      "    testtable;",
      &repo);

  query.execute();
  auto chart = query.getChart(0);

  compareChart(
      chart,
      "QueryTest_TestDerivedSeriesDrawQuery_out.svg.html");
});

TEST_CASE(QueryTest, SimpleEndToEndTest, [] () {
  TableRepository repo;

  auto query = Query(
      "  IMPORT TABLE gbp_per_country "
      "     FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;"
      ""
      "  DRAW BAR CHART;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'gross domestic product per country' as series,"
      "    country as x,"
      "    gbp as y"
      "  FROM"
      "    gbp_per_country"
      "  LIMIT 30;",
      &repo);

  query.execute();
  auto chart = query.getChart(0);

  compareChart(
      chart,
      "QueryTest_SimpleEndToEndTest_out.svg.html");
});


TEST_CASE(QueryTest, TestQueryService, [] () {
  auto query =
      "  IMPORT TABLE gbp_per_country "
      "     FROM CSV 'test/fixtures/gbp_per_country_simple.csv' HEADER;"
      ""
      "  DRAW BAR CHART;"
      "  DRAW BOTTOM AXIS;"
      "  DRAW LEFT AXIS;"
      ""
      "  SELECT"
      "    'gross domestic product per country' as series,"
      "    country as x,"
      "    gbp as y"
      "  FROM"
      "    gbp_per_country"
      "  LIMIT 30;";

  QueryService query_service;
  auto input = fnordmetric::util::StringInputStream::fromString(query);
  auto output = fnordmetric::util::FileOutputStream::openFile(
      "build/tests/tmp/QueryTest_TestQueryService_out.svg.html");

  query_service.executeQuery(
      input.get(),
      QueryService::FORMAT_SVG,
      output.get());

  EXPECT_FILES_EQ(
      "test/fixtures/QueryTest_TestQueryService_out.svg.html",
      "build/tests/tmp/QueryTest_TestQueryService_out.svg.html");
});


