o = []
with open('oldschool-cga.txt', 'r') as g:
	for j in g:
		if j[-1] == '\n':
			j = j[:-1]
		t = 0
		for c in j:
			t = 2*t + (1 if c == '*' else 0)
		o.append(t)

print(o)
print(len(o))
