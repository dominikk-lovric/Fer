def factorial(n):
    if n < 2:
        return 1
    else:
        return n * factorial(n-1)
a, b, c, d, e = map(int, input().split())
L= int(input())
total=0
for nA in range(a + 1):
    for nB in range(b + 1):
        for nC in range(c + 1):
            for nD in range(d + 1):
                for nE in range(e + 1):
                    if nA + nB + nC + nD + nE == L:
                        num = factorial(L)
                        den = (factorial(nA) * factorial(nB) * 
                        factorial(nC) * factorial(nD) * 
                        factorial(nE))
                        total += num // den
print(total)
