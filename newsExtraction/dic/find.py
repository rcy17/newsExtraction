with open("out.dic","rb") as fp:
    lines = fp.readlines()
print(len(lines))
#print(lines[0])
#print(lines[0].decode("gbk"))
while True:
    word = bytes(input() + '\n',encoding="gbk")
    #print(word)
    for i in range(len(lines)):
        if lines[i]==word:
            print(i)
            break
    else:
        print("not found!")
