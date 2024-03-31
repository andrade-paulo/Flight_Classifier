import pandas as pd
import os


def label_data(df, name, flags=[]):
    df['Label'] = "Default"
    df.loc[df['Time'] <= flags[0], 'Label'] = "Taxiamento"
    df.loc[(df['Time'] > flags[0]) & (
        df['Time'] <= flags[1]), 'Label'] = "Decolagem"
    df.loc[(df['Time'] > flags[1]) & (
        df['Time'] <= flags[2]), 'Label'] = "Cruzeiro"
    df.loc[(df['Time'] > flags[2]) & (
        df['Time'] <= flags[3]), 'Label'] = "Pouso"
    if len(flags) > 4:
        df.loc[(df['Time'] > flags[3]) & (df['Time'] <= flags[4]), 'Label'] = "Taxiamento"
        # df.loc[df['Time'] > flags[4], 'Label'] = "Repouso-Final"
    else:
        df.loc[df['Time'] > flags[3], 'Label'] = "Taxiamento"

    if (not os.path.exists(f'./output/')):
        os.makedirs(f'./output/')

    df.to_csv(f'./output/{name}-processed-Labeled.csv', index=False)
    return df


def create_folder(folder):
    if not os.path.exists("./output/" + folder):
        os.makedirs("./output/" + folder)


def split_data(df, name):
    create_folder(name)
    labels = ['Taxiamento', 'Decolagem', 'Cruzeiro',
              'Pouso']
    for label in labels:
        if label in df['Label'].values:
            df_label = df[df['Label'] == label]
            df_label.to_csv(f'./output/{name}/{name}-{label}.csv', index=False)


def process_data(name, flags):
    df = pd.read_csv(f'./data/{name}-processed.csv')
    data_label = label_data(df, name, flags)
    split_data(data_label, name)


def main():
    data_dict = {
        'voo-01': [12980, 19390, 79310, 90630, 93980],
        'voo-02': [3730, 7620, 70200, 77700, 79380, 83130],
        'voo-03': [3930, 7380, 51090, 66890, 70370],
        'voo-04': [5410, 8620, 52420, 64100, 70370] # Os últimos 2 são valores inválidos
    }

    for name, flags in data_dict.items():
        process_data(name, flags)


if __name__ == '__main__':
    main()
