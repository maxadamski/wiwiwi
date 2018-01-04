// UWAGA: wszystkie funkcje sa napisane w stylu imperatywnym na potrzeby
// zadan. Do efektywnej pracy w scilab uzywaj tablic!

disp('Zadanie 3')

function R = parzysta(n)
	if modulo(n, 2) == 0 then
		R = %t
	else
		R = %f
	end
endfunction

disp('ok')


disp('Zadanie 4')

function R = fibonacci(n)
	if n == 0 then
		R = 0
	elseif n == 1 then
		R = 1
	else
		R = fibonacci(n - 1) + fibonacci(n - 2)
	end
endfunction

fibsum = 0
for i = 0:9
	fibsum = fibsum + fibonacci(i)
end
disp(fibsum)


disp('Zadanie 5')

function R = pierwsza(n)
	if n <= 1 then
		R = %f
		return
	elseif n == 2 || n == 3 then
		R = %t
		return
	end
	for i = 2:sqrt(n)
		if modulo(n, i) == 0 then
			R = %f
			return
		end
	end
	R = %t
endfunction

disp('ok')


disp('Zadanie 6')

primesum = 0
for i = 1:99
	if pierwsza(i) then
		primesum = primesum + i
	end
end
disp(primesum)


disp('Zadanie 7')

function R = Silnia(n)
	if ~naturalna(n) then
		disp('nie spelnia warunku') 
		return
	end
	if n == 0 | n == 1 then
		R = 1
	else
		R = n * silnia(n - 1)
	end
endfunction

disp('ok')


disp('Zadanie 8')

function R = naturalna(n)
	if round(n) == n && n >= 0 then
		R = %t
	else
		R = %f
	end
endfunction

function R = newton(n, k)
	if ~(n >= k && naturalna(n) && naturalna(k)) then
		disp('nie spelnia warunku') 
		return
	end
	R = silnia(n) / (silnia(k) * silnia(n - k))
endfunction

disp('ok')
