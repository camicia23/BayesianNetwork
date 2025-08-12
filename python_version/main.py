from pgmpy.readwrite import BIFReader
from pgmpy.inference import VariableElimination

input_directory = "../data/"
input_file = input("Insert BIF file: ")

reader = BIFReader(input_directory + input_file + ".bif")
model = reader.get_model()

inference = VariableElimination(model)

while True:
    query_var = input("Insert query value: ")
    if(query_var == ""):
        break
    evidences_line = input("Evidences: ")
    evidences = evidences_line.split()
    evidences_dictionary = {}

    i = 0
    for i in range(0, len(evidences), 2):
        evidences_dictionary[evidences[i]] = evidences[i+1]
    
    
    query_result = inference.query(variables=[query_var], evidence=evidences_dictionary)

    print(query_result)
