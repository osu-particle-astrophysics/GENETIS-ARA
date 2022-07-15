import pandas as pd 
import matplotlib.pyplot as plt
from pandas.plotting import parallel_coordinates
import numpy as np

#This preps the data to become a pandas dataframe
#dtypedict={'InnerRadius':np.float64, 'Length':np.float64,'OpeningAngle':np.float64,'Fitness':np.float64,'Generation':np.int64,'GenerationString':str}
df0 = pd.read_csv("Generation_Data/runData.csv",skiprows=1,names=["InnerRadius","Length","Quadratic","Linear","Fitness","Generation"],index_col=False)
#df1 = df1.replace(r'^\s*$', np.nan, regex=True)

#This adds the Generation column to the file
i = 0
j= 0
print(df0)
while i < df0.shape[0]:
    if pd.isna(df0.iloc[i,3]) == True:
        j += 1
        i += 1

    else:
        df0.at[i,"Generation"] = j-1
        i += 1

df0=df0.dropna()
df0['Generation']=df0.Generation.astype(int)

df0=df0.dropna()
df0=df0.reset_index(drop=True)

#array for first half of bicone
df1=df0.iloc[::2,:]
df1=df1.reset_index(drop=True)
#df1=df1.rename(columns = {'Unnamed: 0':'Individual1'})
#array for second bicone
df2=df0.iloc[1::2,:]
df2=df2.reset_index(drop=True)
df2=df2.rename(columns = {'InnerRadius':'InnerRadius2','Length':'Length2','Quadratic':'Quadratic2','Linear':'Linear2','Generation':'Generation2','Fitness':'Fitness2'})
print(df2)
print(df1)
df3 = pd.concat([df1,df2.reindex(df1.index)],join="inner",axis=1)
print(df3)
df4 = df3[['InnerRadius','Length','Quadratic','Linear','InnerRadius2','Length2','Quadratic2','Linear2','Generation','Fitness']].copy()
df4.reset_index()

converted_array = df4.to_numpy()
#i = 0
#for i in range(df3.shape[0]):
#    df3.iloc[i,9] = df3.iloc[i,9]*(180/np.pi)
#    df3.iloc[i,3] = df3.iloc[i,3]*(180/np.pi)


df4.to_csv("Generation_Data/testpara.csv",index=False)
