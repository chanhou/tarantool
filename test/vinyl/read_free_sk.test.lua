--
-- gh-2129: This issue introduces a new type of a vinyl space
-- with read-free REPLACE and DELETE. A vinyl space becames
-- read-free, if it contains only not unique secondary indexes.
-- In such a case garbage collection is deferred until primary
-- index compaction or dump.
--

s = box.schema.create_space('test', {engine = 'vinyl'})
pk = s:create_index('pk', {run_count_per_level = 100})
sk = s:create_index('sk', {parts = {2, 'unsigned'}, unique = false, run_count_per_level = 100})

s:replace{1, 20}
s:replace{1, 30}
s:replace{1, 60}
s:replace{1, 50}
s:replace{1, 40}
sk:info().disk.iterator.read.rows
pk:info().disk.iterator.read.rows

box.snapshot()

-- Sk contains 5 replaces in a one run and 4 deletes in another.
sk:info().run_count
sk:info().disk.rows
pk:info().disk.rows

s:select{}
sk:select{20}
sk:select{30}
sk:select{60}
sk:select{50}
sk:select{40}

sk:info().disk.iterator.read.rows
pk:info().disk.iterator.read.rows

s:drop()
