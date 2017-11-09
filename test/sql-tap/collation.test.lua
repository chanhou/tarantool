#!/usr/bin/env tarantool
test = require("sqltester")
test:plan(160)

local prefix = "collation-"

-- we suppose that tables are immutable
local function merge_tables(...)
    local r = {}
    local N = select('#', ...)
    for i = 1, N do
        local tbl = select(i, ...)
        for _, row in ipairs(tbl) do
            table.insert(r, row)
        end
    end
    return r
end

local function insert_into_table(tbl_name, data)
    local sql = string.format([[ INSERT INTO %s VALUES ]], tbl_name)
    local values = {}
    for _, row in ipairs(data) do
        local items = {}
        for _, item in ipairs(row) do
            if type(item) == "string" then
                table.insert(items, "'"..item.."'")
            end
            if type(item) == "number" then
                table.insert(items, item)
            end
        end
        local value = "("..table.concat(items, ",")..")"
        table.insert(values, value)
    end
    values = table.concat(values, ",")
    sql = sql..values
    test:execsql(sql)
end

local data_eng = {
    {1, "Aa"},
    {2, "a"},
    {3, "aa"},
    {4, "ab"},
    {5, "aba"},
    {6, "abc"},
}
local data_num = {
    {21, "1"},
    {22, "2"},
    {23, "21"},
    {24, "23"},
    {25, "3"},
    {26, "9"},
    {27, "0"},
}
local data_symbols = {
    {41, " "},
    {42, "!"},
    {43, ")"},
    {44, "/"},
    {45, ":"},
    {46, "<"},
    {47, "@"},
    {48, "["},
    {49, "`"},
    {50, "}"},
}
local data_ru = {
    -- Russian strings
    {61, "А"},
    {62, "а"},
    {63, "Б"},
    {64, "б"},
    {65, "е"},
    {66, "её"},
    {67, "ё"},
    {68, "Ё"},
    {69, "ж"},
}

local data_combined = merge_tables(data_eng, data_num, data_symbols, data_ru)
--------------------------------------------
-----TEST CASES FOR DIFFERENT COLLATIONS----
--------------------------------------------

local data_test_binary_1 = {
    --   test_name , data to fill with, result output in col
    {"en", data_eng, {1,2,3,4,5,6}},
    {"num", data_num, {27,21,22,23,24,25,26}},
    {"symbols", data_symbols, {41,42,43,44,45,46,47,48,49,50}},
    {"ru", data_ru, {68,61,63,62,64,65,66,69,67}},
    {"combined", data_combined,
        {41,42,43,44,27,21,22,23,24,25,26,45,46,47,1,
            48,49,2,3,4,5,6,50,68,61,63,62,64,65,66,69,67}}
}

local data_test_unicode = {
    --   test_name , data to fill with, result output in col
    {"en", data_eng, {2,3,1,4,5,6}},
    {"num", data_num, {27,21,22,23,24,25,26}},
    {"symbols", data_symbols, {41,45,42,43,48,50,47,44,49,46}},
    {"ru", data_ru, {62,61,64,63,65,67,68,66,69}},
    {"combined", data_combined,
        {41,45,42,43,48,50,47,44,49,46,27,21,22,23,24,25,
            26,2,3,1,4,5,6,62,61,64,63,65,67,68,66,69}}
}


local data_test_unicode_ci = {
    --   test_name , data to fill with, result output in col
    {"en", data_eng, {2,1,3,4,5,6}},
    {"num", data_num, {27,21,22,23,24,25,26}},
    {"symbols", data_symbols, {41,45,42,43,48,50,47,44,49,46}},
    {"ru", data_ru, {61,62,63,64,65,67,68,66,69}},
    {"combined", data_combined,
        {41,45,42,43,48,50,47,44,49,46,27,21,22,23,24,25,
            26,2,1,3,4,5,6,61,62,63,64,65,67,68,66,69}}
}

local data_collations = {
    -- default collation = binary
    {"/*COLLATE DEFAULT*/", data_test_binary_1},
    {"COLLATE BINARY", data_test_binary_1},
    {"COLLATE \"unicode\"", data_test_unicode},
    {"COLLATE \"unicode_ci\"", data_test_unicode_ci},
}

for _, data_collation in ipairs(data_collations) do
    for _, test_case in ipairs(data_collation[2]) do
        local extendex_prefix = string.format("%s1.%s.%s.", prefix, data_collation[1], test_case[1])
        local data = test_case[2]
        local result = test_case[3]
        test:do_execsql_test(
            extendex_prefix.."create_table",
            string.format("create table t1(a primary key, b %s);", data_collation[1]),
            {})
        test:do_test(
            extendex_prefix.."insert_values",
            function()
                return insert_into_table("t1", data)
            end, {})
        test:do_execsql_test(
            extendex_prefix.."select_plan_contains_b-tree",
            string.format("explain query plan select a from t1 order by b %s;",data_collation[1]),
            {0,0,0,"SCAN TABLE T1",
                0,0,0,"USE TEMP B-TREE FOR ORDER BY"})
        test:do_execsql_test(
            extendex_prefix.."select",
            string.format("select a from t1 order by b %s;",data_collation[1]),
            result)
        test:do_execsql_test(
            extendex_prefix.."create index",
            string.format("create index i on t1(b %s)",data_collation[1]),
            {})
        test:do_execsql_test(
            extendex_prefix.."select_from_index_plan_does_not_contain_b-tree",
            string.format("explain query plan select a from t1 order by b %s;",data_collation[1]),
            {0,0,0,"SCAN TABLE T1 USING COVERING INDEX I"})
        test:do_execsql_test(
            extendex_prefix.."select_from_index",
            string.format("select a from t1 order by b %s;",data_collation[1]),
            result)
        test:do_execsql_test(
            extendex_prefix.."drop_table",
            "drop table t1",
            {})
    end
end

test:finish_test()
