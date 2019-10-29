a = [1,1,2,3,5,8,13,21,34,55,89]
for i in a:
    if i < 5:
        print(i)

b = []
for i in a:
    if i < 5:
        b += [i]
print (b)

c = []
num = int(input("Number to compare: "))
for i in a:
    if i < num:
        c += [i]
print(c)
