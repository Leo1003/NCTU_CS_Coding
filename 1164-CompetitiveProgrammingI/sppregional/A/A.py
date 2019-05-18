from decimal import Decimal, getcontext
getcontext().prec = 10000
a = int(input())
ans = Decimal(0)
for i in range(a):
    x, y = (Decimal(x) for x in input().split())
    ans += x * y
print(ans)

