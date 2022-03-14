import math
for i in range(2,50):
  #print("***",i)
  #delta=10^(1./i)
  firstRatio=None
  maxProdRatio=1.
  minRatio=1000.
  lastRatio=1.
  lastXR=1.
  prec=0 if i<4 else 1 if i<20 else 2
  roundDiv=1 if i<4 else 2 if i<9 else 5 if i<16 else 10 if i<30 else 20
  divisions=[]
  for j in range(1,i+1):
    x=math.pow(10,j*1./i)

    xr=round(x,prec)
    xr=round(x*roundDiv)/roundDiv
    divisions.append(xr)
    ratio=xr/lastXR
    minRatio=min(minRatio,ratio)
    maxProdRatio=max(maxProdRatio, ratio*lastRatio)
    if firstRatio is None:
      firstRatio=ratio
    #print(j,xr,ratio,lastXR,maxProdRatio)
    lastXR=xr
    lastRatio=ratio
  #print(maxProdRatio, ratio, firstRatio)
  maxProdRatio=max(maxProdRatio, ratio*firstRatio)
  print("Divisions: %3d min: %.3f maxProdRatio: %.3f Efficency %.1f%%"%(i,minRatio,maxProdRatio,(100*(10.**(1/i))/(maxProdRatio/minRatio))))
  print("  ",",".join(map(lambda x:"%.2f"%(x,),divisions)))

# 1 3 10
# 1 2 5 10
# 1 2 3 5 10
# 1.5 2.5 4 6.5 10
# 1.5 2 3 4.5 7 10
# 1.5 2 2.5 3.5 5 7 10
# 1.5 