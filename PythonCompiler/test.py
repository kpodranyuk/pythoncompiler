a = int(input())
while a != 0:
    if a < 0:
        print('Number equals 0', a)
        break
    a = int(input())
else:
    print('Number dont equals 0')

s = 3*'ab12c59p7dq'
d = ''' string1
string2
str\"string3''
'''
g = """ string1
string2
str\"string3"""
del g
digits = []
for symbol in s:
    if symbol:
        digits.append(symbol)
    elif not symbol == True or symbol != False and True:
        digits.remove(symbol)
	elif a<8 or 10>=a:
        continue
    else:
        continue

list = [["a", "b"], ["c", "d"]]
list2 = [["e", "f"], ["g", "h"]]
print(list[1])
print(list2[-1])


def funcOperation(a, b): #Comment
    a = b
    a * b
    a ** b
    c = a / 10
    c = a // 5
    c = a + 4
    c = a - 4
    return c
def baseFunc(a=0,c=False,d="fjghdf",e='fgdjfkj"jkghdf',f='''fgdfjk 
fdhjsdfgdjhf''',g="""jksfhgfjkdh"""): 
	def func(): 
		return 0 
	a=-9 
	return func 
baseFunc()
print ("abc"+"def")
int('0b10011', 2)
print(funcOperation(10, 2))
print('Hello, {}'.format(5))
print(repr(5))