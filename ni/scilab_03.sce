disp('Zadanie 1')

function R = Fibonacci(n)
	if n < 0 then
		disp('nie spelnia warunku')
		return
	end
	if n == 0 then
		R = [0]
	else
		R = [0 1]
		for i = 3:n
			R(i) = R(i - 1) + R(i - 2)
		end
	end
endfunction

function R = fibonacci(n)
	R = Fibonacci(n)($)
endfunction

disp('ok')


disp('Zadanie 2')

function R = naturalna(n)
	R = %f
	if round(n) == n && n >= 0 then
		R = %t
	end
endfunction

function R = Silnia(n)
	if ~naturalna(n) then
		disp('nie spelnia warunku')
		return
	end
	R = [1]
	for i = 2:n
		R(i) = R(i - 1) * i
	end
endfunction

function R = silnia(n)
	R = Silnia(n)($)
endfunction

disp('ok')


disp('Zadanie 3')

function R = newton(n, k)
	if ~(n >= k && naturalna(n) && naturalna(k)) then 
		disp('nie spelnia warunku')
		return
	end
	R = silnia(n) / (silnia(k) * silnia(n - k))
endfunction

disp('ok')


disp('Zadanie 4')

function zadanie4()
	clf()
	x = linspace(-1, 2, 25)
	plot(x, %e**x)
endfunction

disp('ok')


disp('Zadanie 5')

function zadanie5(A)
	clf()
	x = linspace(-180, 180, 40)
	plot(x, cosd(A * x))
endfunction

disp('ok')


disp('Zadanie 6')

function zadanie6(A, GRANICA)
	clf()
	if ~(GRANICA >= -1 && GRANICA <= 1) then
		disp('nie spelnia warunku')
		return
	end
	x = linspace(-180, 180)
	plot(x, cosd(A * x), 'b')
	plot(x, min(cosd(A * x), GRANICA), 'r')
endfunction

disp('ok')


disp('Zadanie 7')

function zadanie7(a, b, k)
	clf()
	[x, y] = meshgrid(0:5:360) 
	z = sind(a*x).**k + sind(b*y)
	surf(x, y, z)
endfunction

disp('ok')
