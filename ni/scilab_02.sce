disp('Zadanie 1')
disp(82:92)

disp('Zadanie 2')
disp((1:4) + 1)

disp('Zadanie 3')
disp((-8:2:-4)./(8:-2:4))

disp('Zadanie 4')
V = [5, -5, 9 ,12, -1, 0, 4]
V(V > 0) = 10
disp(V)

disp('Zadanie 5')
MOD = [-5 9 12; -1 0 -12; 9 6 1]
MOD(modulo(MOD, 3) == 0) = -2
disp(MOD)

disp('Zadanie 6')
disp(rand(3, 3) + eye(3, 3)*3)

disp('Zadanie 7')
MAG_A = testmatrix('magi', 5)
MAG_B = MAG_A(2:3, :)
disp(MAG_A)
disp(MAG_B)

disp('Zadanie 8')
MAG_C = [MAG_A; MAG_B]
disp(MAG_C)

disp('Zadanie 9')
disp(roots([1, 0, -4, -2]))

disp('Zadanie 10')
A = [1 2 -1; 1 3 2; -1 -3 -1]
B = [2 -1 0; 1 2 1; 1 0 0]
disp('A * X = B <-> X = inv(A) * B')
disp(inv(A)*B)
disp('X * A = B <-> X = B * inv(A)')
disp(B*inv(A))

disp('Zadanie 11')
F = [0, 1]
F_n = 10
for i = 3:F_n
	F(i) = F(i - 2) + F(i - 1)
end
disp(F)
disp(sum(F))

disp('Zadanie 12')
H = []
for i = 1:5
	for j = 1:5
		H(i,j) = 1 / (i + j - 1)
	end
end
disp(H)
