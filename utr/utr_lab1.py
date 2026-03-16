ulazi = []
stanja = []
abeceda = []
prihvatljivo = []
pocetno = 0
prijelazi=[]
content = " "
j=0
while (content != ""):
    content=input()
    if(j==0):
        ulazi1=content.split("|")
        for i in range(len(ulazi1)):
            ulazi2=ulazi1[i].split(",")
            for k in range(len(ulazi2)):
                if(k==0):
                    ulazi.append([ulazi2[k]])
                else:
                    ulazi[i].append(ulazi2[k])
    elif(j==1):
        stanja=content.split(",")
        for i in range(len(stanja)):
            prijelazi.append([])
    elif(j==2):
        abeceda=content.split(",")
        abeceda.insert(0,"$")
        for  i in range(len(stanja)):
            for k in range(len(abeceda)):
                prijelazi[i].append(["#"])
    elif(j==3):
        prihvatljivo=content.split(",")
    elif(j==4):
        pocetno=content
    else:
        if (content!=""):
            pr=content.split(",")
            st=stanja.index(pr[0])
            pr2=pr[1].split("->")
            up=pr2[0]
            up=abeceda.index(up)
            prijelazi[st][up][0] = pr2[1]
            if(len(pr)>2):
                for i in range(len(pr)-2):
                    prijelazi[st][up].append(pr[i+2])

    j=j+1
cs=[]
cs.append(pocetno)
str=""
for i in range(len(ulazi)):
    for j in range(len(ulazi[i])):
        dalje = []
        if(j==0):
            k=0
            while(k!=len(cs)):
                for l in range(len(prijelazi[stanja.index(cs[k])][0])):
                    if(prijelazi[stanja.index(cs[k])][0][l]!="#"):
                        if(prijelazi[stanja.index(cs[k])][0][l] not in cs):
                            cs.append(prijelazi[stanja.index(cs[k])][0][l])
                k=k+1
            cs.sort()
        if (len(cs) == 0):
            str = str + "#"
        else:
            for k in range(len(cs)):
                if(k!=0):
                    str=str+","
                str=str+cs[k]
        str=str+"|"
        for k in range(len(cs)):
            for n in range(len(prijelazi[stanja.index(cs[k])][abeceda.index(ulazi[i][j])])):
                if(prijelazi[stanja.index(cs[k])][abeceda.index(ulazi[i][j])][n]!="#"):
                    if(prijelazi[stanja.index(cs[k])][abeceda.index(ulazi[i][j])][n] not in dalje):
                        dalje.append(prijelazi[stanja.index(cs[k])][abeceda.index(ulazi[i][j])][n])
        k=0
        while(k!=len(dalje)):
            for l in range(len(prijelazi[stanja.index(dalje[k])][0])):
                if(prijelazi[stanja.index(dalje[k])][0][l]!="#"):
                    if(prijelazi[stanja.index(dalje[k])][0][l] not in dalje):
                        dalje.append(prijelazi[stanja.index(dalje[k])][0][l])
            k=k+1
        cs=dalje
        cs.sort()
    if(len(cs)==0):
        str=str+"#"
    else:
        for k in range(len(cs)):
            if (k != 0):
                str = str + ","
            str = str + cs[k]
    print(str)