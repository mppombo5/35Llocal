def find_sum(someList):
    sum = 0 # initialize sum
    for element in someList:
        sum += element
    return sum # returns the sum

myList = [69, 420, 666, 1337]

listSum = find_sum(myList)

print(listSum)

ansSum = find_sum([1,2,3,4])
print(ansSum)
