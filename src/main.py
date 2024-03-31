import os

from components.Flight_Data import Flight_Data

def create_output_folder():
    if (not os.path.exists(f'./output/')):
        os.makedirs(f'./output/')

if __name__ == '__main__':
    create_output_folder()
    flight1 = Flight_Data(1)
    flight1.taxiamento_decolagem = (0, 12980)
    flight1.decolagem = (12980, 19390)
    flight1.cruzeiro = (19390, 79310)
    flight1.pouso = (79310, 90630)
    flight1.taxiamento_pouso = (90630, 93980)
    flight1.create_csv_files()

    flight2 = Flight_Data(2)
    flight2.taxiamento_decolagem = (0, 3730)
    flight2.decolagem = (3730, 7620)
    flight2.cruzeiro = (70200, 77700)
    flight2.pouso = (77700, 79380)
    flight2.taxiamento_pouso = (79380, 83130)
    flight2.create_csv_files()

    flight3 = Flight_Data(3)
    flight3.taxiamento_decolagem = (0, 3930)
    flight3.decolagem = (3930, 7380)
    flight3.cruzeiro = (51090, 66890)
    flight3.pouso = (66890, 70370)
    flight3.create_csv_files()

    flight4 = Flight_Data(4)
    flight4.taxiamento_decolagem = (0, 5410)
    flight4.decolagem = (5410, 8620)
    flight4.cruzeiro = (52420, 64100)
    flight4.create_csv_files()
