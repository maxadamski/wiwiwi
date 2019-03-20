
# SQL Cheat Sheet

## Selection

```
select [unique] { _ [ as _ ] }
from _
where _
group by _
having _
order by { _ [asc | desc] } [ nulls (last | first) ]
```

## Switch

```
case value
when c1 then r1
when ci then ri
else other
end
```

## Groups

```
avg, count, max, min, sum, variance, stddev
count(*)
count([all | distinct] _)
```

## Library

```
-- Operators
T in [T]
T between T and T
Str like Str
T? is null
date 'YYYY-MM-DD'
timestamp 'YYYY-MM-DD HH:MI:SS.MMMM'
interval 'REPR' _ [ to _ ]
listagg(_, Str) within group (order by _)

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

## Formatting

### Date & Time
```
SCC BC AD - Stulecie, Era
D DD DDD DAY fmDAY - Day
MM MON MONTH fmMONTH - Month
YYYY - Year
HH HH24 - Hour
MI - Minute
SS - Second
AM PM - Pora dnia
```

### Number
```
D - fraction separator
G - group separator
9 - digit
0 - zero padding
```
