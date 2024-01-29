import cv2

img = cv2.imread("test.jpg", cv2.IMREAD_GRAYSCALE)

height = len(img)
width = len(img[0])

img_f = open("test.pgm", "w")

img_f.write("P2\n# Imagen generada por Alfonso Cardenas\n")

img_f.write(str(width) + " " + str(height) + "\n255")

for line in img:
    for val in line:
        img_f.write("\n" + str(val))

img_f.close()