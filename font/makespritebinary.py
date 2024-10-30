import sys

o = []
f = []
fi = 0
cm = {' ': 0, '@': 1, '$': 2, '&': 3}
with open('sprites.txt', 'r') as g:
	while True:
		j = g.readline()
		if len(j) == 0:
			break
		p = []
		for i in range(4):
			j = list(map(int, g.readline().split()))
			# print(j)
			if len(sys.argv) == 1:
				tt = 0
				for k in range(4):
					tt = 64*tt + j[k]
				# print(hex(tt))
				o.append(tt//65536)
				o.append(tt//256 % 256)
				o.append(tt % 256)
				fi += 3
			else:
				p.append(0)
				for k in range(4):
					p[-1] = 256*p[-1] + j[k]
		j = list(map(int, g.readline().split()))
		# print(j)
		if len(sys.argv) == 1:
			o.append(j[0])
			o.append(j[1])
			fi += 2
		if len(sys.argv) == 1:
			f.append([fi, j[0], j[1]])
		else:
			f.append([p, fi, j[0], j[1]])
		
		u = 4
		t = 0
		for i in range(j[1]):
			h = g.readline()
			if h[-1] == '\n':
				h = h[:-1]
			# print(h)
			# print(list(h))
			for k in range(j[0]):
				# print(k, u)
				if k >= len(h):
					break
				if u == 0:
					# print(hex(t))
					o.append(t)
					fi += 1
					t = 0
					u = 4
				t = 4*t + cm[h[k]]
				u -= 1
		while u > 0:
			t *= 4
			u -= 1
		# print(hex(t))
		o.append(t)
		fi += 1
		t = 0
		u = 4

def cp(x):
	return str(x).replace('[', '{').replace(']', '}')

print(cp(o))
print(len(o))
print(cp(f))
print(len(f))
# print(list(map(hex, o)))
