import matplotlib.pyplot as plt

f = open("output_img.pgm")
for _ in range(4):
    f.readline()

vals = []

for x in f:
    vals.append(int(x))

print(set(vals))

plt.hist(vals, bins = 255)
plt.show()