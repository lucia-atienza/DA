import os
os.system('./prueba')
n = 0
datos2 = []
with open("file.txt") as fname:
	for lineas in fname:
		datos2.extend(lineas.split())

k = 0
while k != 10:
    n = n + float(datos2[0])
    k = k+1

print("Total: " + str(n))
print("Media: " +str(n/10))