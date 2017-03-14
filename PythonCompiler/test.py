a = int(input())
while a != 0:
    if a < 0:
        print('Встретилось отрицательное число', a)
        break
    a = int(input())
else:
    print('Ни одного отрицательного числа не встретилось')

s = 'ab12c59p7dq'
digits = []
for symbol in s:
    if symbol:
        digits.append(symbol)
    elif not symbol == True or symbol != False:
        digits.remove(symbol)
    else:
        continue

list = [["a", "b"], ["c", "d"]]
list2 = [["e", "f"], ["g", "h"]]
print(list + list2)


def funcOperation(a, b):
    a = b
    a %= 1
    a //= 1
    a /= 5
    a -=0
    a += 10
    a *= 2
    a **= 1
    a * b
    a ** b
    b % 2
    c = a / 10
    c = a // 5
    c = a + 4
    c = a - 4
    return c

print(funcOperation(10, 2))
print('Hello, {}'.format(5))
print(repr(5))