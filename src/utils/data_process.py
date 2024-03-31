import pandas as pd


def relative_time(df):
    first_time = df["Time"].iloc[0]
    df["Time"] = df["Time"] - first_time


def sensor_calibration(df):
    voo01_AcelX_mean = (df['Acel. X'].iloc[0:370]).mean()
    voo01_AcelY_mean = (df['Acel. Y'].iloc[0:370]).mean()
    voo01_AcelZ_mean = (df['Acel. Z'].iloc[0:370]).mean()

    df['Acel. X'] = df['Acel. X'] - voo01_AcelX_mean
    df['Acel. Y'] = df['Acel. Y'] - voo01_AcelY_mean
    df['Acel. Z'] = (df['Acel. Z'] - voo01_AcelZ_mean) + 9.807

    voo01_RotX_mean = (df['Rot. X'].iloc[0:370]).mean()
    voo01_RotY_mean = (df['Rot. Y'].iloc[0:370]).mean()
    voo01_RotZ_mean = (df['Rot. Z'].iloc[0:370]).mean()

    df['Rot. X'] = df['Rot. X'] - voo01_RotX_mean
    df['Rot. Y'] = df['Rot. Y'] - voo01_RotY_mean
    df['Rot. Z'] = df['Rot. Z'] - voo01_RotZ_mean


def save_data(df, name):
    df.to_csv(f'./data/{name}-processed.csv', index=False)


def process_data(df, name):
    relative_time(df)
    sensor_calibration(df)
    save_data(df, name)


df = pd.read_csv('./data/teste_bateria.csv')
process_data(df, 'voo-04')