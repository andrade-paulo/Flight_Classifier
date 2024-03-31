import pandas as pd
from typing import Tuple
import os

class Flight_Data:
    def __init__(self, id: int):
        self.id = id
        self.df = pd.read_csv('./data/voo-{:02}-processed.csv'.format(id))
        self.taxiamento_decolagem: Tuple[int, int] = None
        self.decolagem: Tuple[int, int] = None
        self.cruzeiro: Tuple[int, int] = None
        self.pouso: Tuple[int, int] = None
        self.taxiamento_pouso: Tuple[int, int] = None

    def set_label_data(self, start, end, name):
        if 'Label' not in self.df.columns:
            self.df['Label'] = "Default"
        self.df.loc[(self.df['Time'] >= start) & (
            self.df['Time'] < end), 'Label'] = name

    def label_cruzeiro(self):
        if self.cruzeiro:
            self.set_label_data(self.cruzeiro[0], self.cruzeiro[1], "Cruzeiro")

    def label_decolagem(self):
        if self.decolagem:
            self.set_label_data(
                self.decolagem[0], self.decolagem[1], "Decolagem")

    def label_pouso(self):
        if self.pouso:
            self.set_label_data(self.pouso[0], self.pouso[1], "Pouso")

    def label_taxiamento(self):
        if self.taxiamento_decolagem:
            self.set_label_data(
                self.taxiamento_decolagem[0], self.taxiamento_decolagem[1], "Taxiamento")
        if self.taxiamento_pouso:
            self.set_label_data(
                self.taxiamento_pouso[0], self.taxiamento_pouso[1], "Taxiamento")

    def label_data(self):
        self.label_taxiamento()
        self.label_decolagem()
        self.label_cruzeiro()
        self.label_pouso()
        print(f"Rows analysed: {len(self.df)}")
        print(f"Rows labeled: {len(self.df[self.df['Label'] != 'Default'])}")
        print(f"Rows not labeled: {len(self.df[self.df['Label'] == 'Default'])}\n")

    def create_csv_files(self):
        name = "voo-{:02}".format(self.id)
        print(f"Creating labels for {name.upper()}")
        
        # Creating the labeled data
        self.label_data()
        self.df.to_csv(f'./output/{name}-processed-Labeled.csv', index=False)

        # Creating the folder for the separated data
        if (not os.path.exists(f'./output/{name}')):
            os.makedirs(f'./output/{name}')

        # Creating the separated data
        labels = ['Taxiamento', 'Decolagem', 'Cruzeiro', 'Pouso']
        for label in labels:
            if label in self.df['Label'].values:
                df_label = self.df[self.df['Label'] == label]
                df_label.to_csv(
                    f'./output/{name}/{name}-{label}.csv', index=False)
