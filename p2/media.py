import os
os.system('./pruebas')
n = 0
datos2 = []
with open("file.txt") as fname:
	for lineas in fname:
		datos2.extend(lineas.split())

k = 0
while k != 30:
    if(k % 3 == 0):
        n = n + float(datos2[k])
    k = k+1

print("Total: " + str(n))
print("Media: " +str(n/10))

