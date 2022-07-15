import plotly.express as px
import pandas as pd
data = pd.read_csv('Generation_Data/testpara.csv')
fig = px.parallel_coordinates(data, color = "Fitness", color_continuous_scale = px.colors.sequential.Turbo, dimensions = ['InnerRadius', 'Length','Quadratic','Linear', 'InnerRadius2', 'Length2', 'Quadratic2','Linear2', 'Generation'],labels={"InnerRadius": "Radius 1 (cm)","Length": "Length 1 (cm)", "Quadratic": "Quadratic 1","Linear": "Linear 1","InnerRadius2": "Radius 2 (cm)", "Length2": "Length 2 (cm)", "Quadratic2": "Quadratic 2","Linear2": "Linear 2","Generation": "Generation"},color_continuous_midpoint=4.25, range_color = [3.5,5.25])
fig.write_image("Generation_Data/Rainbow_Plot.png")


