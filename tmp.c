import re

s = "***** **** (key-1, key-2, key-3, key-n) *** ***** **** (val-1, val-2, val-3, val-n) "

# Find all content within parentheses
matches = re.findall(r'\((.*?)\)', s)

# Split the keys and values by commas
keys = matches[0].split(', ')
values = matches[1].split(', ')

# Create a dictionary by zipping keys and values together
d = dict(zip(keys, values))

print(d)
