import os

from components.Flight_Data import Flight_Data

def create_output_folder():
    if (not os.path.exists(f'./output/')):
        os.makedirs(f'./output/')

if __name__ == '__main__':
    create_output_folder()
    flight1 = Flight_Data(1)
    flight1.repouso_inicial = (0, 37760)
    flight1.taxiamento_decolagem = (37760, 50740)
    flight1.decolagem = (50740, 57150)
    flight1.cruzeiro = (57150, 117070)
    flight1.pouso = (117070, 128390)
    flight1.taxiamento_pouso = (128390, 131740)
    flight1.repouso_final = (131740, 143893)
    flight1.create_csv_files()

    flight2 = Flight_Data(2)
    flight2.repouso_inicial = (0, 3890)
    flight2.taxiamento_decolagem = (3890, 7620)
    flight2.decolagem = (7620, 11510)
    flight2.cruzeiro = (11510, 74090)
    flight2.pouso = (74090, 81590)
    flight2.repouso_final = (81590, 91098)
    flight2.create_csv_files()

    flight3 = Flight_Data(3)
    flight3.repouso_inicial = (0, 5950)
    flight3.taxiamento_decolagem = (5950, 9880)
    flight3.decolagem = (9880, 13330)
    flight3.cruzeiro = (13330, 57040)
    flight3.pouso = (57040, 72840)
    flight3.taxiamento_pouso = (72840, 76320)
    flight3.create_csv_files()

    flight4 = Flight_Data(4)
    flight4.repouso_inicial = (0, 3670)
    flight4.taxiamento_decolagem = (3670, 9080)
    flight4.decolagem = (9080, 12290)
    flight4.cruzeiro = (12290, 48110)
    flight4.create_csv_files()

    flight5 = Flight_Data(5)
    flight5.decolagem = (0, 3503)
    flight5.create_csv_files()

    flight6 = Flight_Data(6)
    flight6.taxiamento_decolagem = (0, 13773)
    flight6.create_csv_files()

    flight7 = Flight_Data(7)
    flight7.repouso_inicial = (0, 52691)
    flight7.create_csv_files()


