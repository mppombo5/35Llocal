#!/usr/local/bin/python3

dict = {}
dict['france'] = "paris"
dict['japan'] = "tokyo"
print(dict['france'])

dict['germany'] = "berlin"
if (dict['france'] == "paris"):
    print("Correct!")
else:
    print("Wrong!")

del dict['france']
del dict
