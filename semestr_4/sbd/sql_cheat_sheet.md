
# SQL Cheat Sheet


## DQL

```
with
  <name> as (<select>), ...
select
  [distinct] <expr> [ as <name> ], ...
from
  {<table> <join>}
where
  <cond>
group by
  <name> [asc | desc], ...
having
  <cond>
order by
  <name> [asc | desc] [nulls {last | first}], ...
fetch first <n> rows with ties
```

### Joins

```
join := [ inner | {full | left | right [outer] ] join <table> on { <cond> | using (<name>, ...) }
join := natural [ inner | {full | left | right} [outer] ] join <table> 
```

### Aggregation

```
avg, count, max, min, sum, variance, stddev
count(*)
count([all | distinct] <attr>)
```

### Sets

```
<select> {union | intersect | minus} <select> 
```



## DML

```
insert into <table> (<attr>, ...) values (<expr>, ...)
delete from <table> where <cond>
update <table> set (<attr>, ...) = (<select>) where <cond>
```



## DDL

```
create table <table> (
  <attr> <type> [default <expr>] [[constraint <name>] <constr>] [generated always as identity],
  ...
  [constraint <name>] <constr>,
  ...
)
```

```
alter table <table> {enable | disable} constraint <name>;
alter table <table> add (...) modify (...);
drop table <table> [cascade constraints];
describe <table>;
```

### Constraints

```
<constr>
  | {primary | foreign} key(<attr>, ...) [on delete cascade]
  | references <table>(<attr>, ...) [on delete cascade]
  | unique(<attr>)
  | check(<expr>)
  | not null
```

### Views

```
create [or replace] view <view> (<attr>, ...)
as <select>
[with check option [constraint <name>]];
```

```
drop view <view>;
```



## Library

### Tables

```
user_constraints: (table_name, constraint_name, constraint_type, search_condition)
user_updatable_columns: (table_name, column_name, updatable, insertable, deletable)
user_tables: (table_name)
dual: (dummy)
```

### Switch

```
case [<expr>]
when <expr> then <expr>
when <expr> then <expr>
else <expr>
end
```

### Functions

```
-- Operators
T in [T]
T between T and T
Str like Str
T? is null
date 'YYYY-MM-DD'
timestamp 'YYYY-MM-DD HH:MI:SS.MMMM'
interval 'REPR' _ [ to _ ]
listagg(<attr>, Str) within group (order by <attr>, ...)

-- Optional
coalesce/nvl(T?, T): T

-- Strings
length(Str): Int
replace(Str, sub: Str, new: Str): Str
substr(Str, start: Int, length: Int): Str
instr(Str, sub: Str, start: Int, count: Int): Int
upper/lower(Str): Str
translate(Str, old: Str, new: Str): Str
trim([leading|trailing|both] Str from Str): Str
lpad/rpad(Str, length: Int, chars: Str): Str

-- Numbers
round/trunc(Num, Int?): Num
floor/ceil(Num): Num
power/mod(Num, Num): Num
sqrt/abs/sign(Num): Num
greatest/least(Num...): Num

-- Date & Time
extract([year|month|day|hour|minute|second] from Date)
current_date/current_time/current_timestamp
months_between(Date, Date): Int
add_months(Date, Int): Date
next_day(Date, ): Date
last_day(Date): Date

-- Conversions
cast(Any as Type)
to_char(Date, fmt: Str): Str
to_char(Num, fmt: Str): Str
to_number(Str, fmt: Str): Num
to_date(Str, fmt: Str): Date

-- Oracle Shit
decode(value, c1, r1, ..., ci, ri, other)
```

### Formatting

#### Date & Time
```
SCC BC AD - Century, Era
D DD DDD DAY fmDAY - Day
MM MON MONTH fmMONTH - Month
YYYY - Year
HH HH24 - Hour
MI - Minute
SS - Second
AM PM - Time of day
```

#### Number
```
D - fraction separator
G - group separator
9 - digit
0 - zero padding
```



## Nulls

- Arithmetic operations with NULL return NULL
- Boolean comparisons with NULL return UNKNOWN
- `count(*)` counts all NULL and non-NULL tuples
- `count(attr)` counts all tuples where attr is not NULL
- Other aggregate functions ignore NULL
