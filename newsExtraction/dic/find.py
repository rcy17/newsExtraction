with open("out.dic","rb") as fp:
    lines = fp.readlines()
with open("stop.dic","rb") as fp:
    lines2 = fp.readlines()
print(len(lines))
print(len(lines2))
#print(lines[0])
#print(lines[0].decode("gbk"))
while True:
    word = bytes(input() + '\n',encoding="gbk")
    #print(word)
    for i in range(len(lines)):
        if lines[i]==word:
            print("dictionary:",i)
            break
    else:
        print("not found!")
    for i in range(len(lines2)):
        if lines2[i]==word:
            print("stop:",i)
            break

