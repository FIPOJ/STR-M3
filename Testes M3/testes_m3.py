import serial
import time

def main1():
    port = input("Informe a porta COM (ex. COM6): ")
    baud_rate = 115200  # Ajuste conforme necessário
    bytes_to_read = 10  # Número de bytes para ler de cada vez

    try:
        with serial.Serial(port, baud_rate, timeout=1) as ser:
            print(f"Conectado à porta {port} com {baud_rate} baud.")
            time.sleep(2)  # Espera 2 segundos para começar a receber dados

            while True:
                if ser.in_waiting > 0:
                    data = ser.read(bytes_to_read)
                    #print(data)  # Imprime os dados brutos recebidos
                    with open('output.txt', 'ab') as file:
                        file.write(data)
                        file.flush()
                    #print("Dados escritos no arquivo.")

    except serial.SerialException as e:
        print(f"Erro ao abrir a porta serial: {e}")

def main2():
    #with open("PiorCaso.txt", 'r') as file:
    with open("MedioCaso.txt", 'r') as file:
        dados = file.read()

    # Dividir as amostras
    amostras = dados.split("Acelerador: ")

    # Processar cada amostra
    tempos_resposta = {
        "Acelerador": [],
        "Freio": [],
        "AirBag": [],
        "Cinto": [],
        "Luz": [],
        "TM": [],
        "TP": [],
        "VM": [],
        "VP": [],
        "Superaquecimento": []
    }
    amostras.pop(0)
    for amostra in amostras:
        
        if int(amostra.split(" -")[0]) == 1:
            tempos_resposta["Acelerador"].append(int(amostra.split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Freio: ")[1].split(" -")[0]) == 1:
            tempos_resposta["Freio"].append(int(amostra.split("Freio: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Airbag: ")[1].split(" -")[0]) == 1:
            tempos_resposta["AirBag"].append(int(amostra.split("Airbag: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Cinto colocado: ")[1].split(" -")[0]) == 1:
            tempos_resposta["Cinto"].append(int(amostra.split("Cinto colocado: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Luz: ")[1].split(" -")[0]) == 1:
            tempos_resposta["Luz"].append(int(amostra.split("Luz: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Trava Motorista: ")[1].split(" -")[0]) == 1:
            tempos_resposta["TM"].append(int(amostra.split("Trava Motorista: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Trava Passageiro: ")[1].split(" -")[0]) == 1:
            tempos_resposta["TP"].append(int(amostra.split("Trava Passageiro: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Vidro Motorista: ")[1].split(" -")[0]) == 1:
            tempos_resposta["VM"].append(int(amostra.split("Vidro Motorista: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Vidro Passageiro: ")[1].split(" -")[0]) == 1:
            tempos_resposta["VP"].append(int(amostra.split("Vidro Passageiro: ")[1].split("Tempo de resposta: ")[1].split(" us")[0]))

        if int(amostra.split("Superaquecimento:")[1].split(" -")[0]) == 1:
            tempos_resposta["Superaquecimento"].append(int(amostra.split("Superaquecimento:")[1].split("Tempo de resposta: ")[1].split(" us")[0]))
    # Calcular médias
    # medias = {var: sum(tempos) / len(tempos) if tempos else 0 for var, tempos in tempos_resposta.items()}
    media_acelerador = sum(tempos_resposta["Acelerador"]) / len(tempos_resposta["Acelerador"]) if tempos_resposta["Acelerador"] else 0
    media_freio = sum(tempos_resposta["Freio"]) / len(tempos_resposta["Freio"]) if tempos_resposta["Freio"] else 0
    media_airbag = sum(tempos_resposta["AirBag"]) / len(tempos_resposta["AirBag"]) if tempos_resposta["AirBag"] else 0
    media_cinto = sum(tempos_resposta["Cinto"]) / len(tempos_resposta["Cinto"]) if tempos_resposta["Cinto"] else 0
    media_luz = sum(tempos_resposta["Luz"]) / len(tempos_resposta["Luz"]) if tempos_resposta["Luz"] else 0
    media_tm = sum(tempos_resposta["TM"]) / len(tempos_resposta["TM"]) if tempos_resposta["TM"] else 0
    media_tp = sum(tempos_resposta["TP"]) / len(tempos_resposta["TP"]) if tempos_resposta["TP"] else 0
    media_vm = sum(tempos_resposta["VM"]) / len(tempos_resposta["VM"]) if tempos_resposta["VM"] else 0
    media_vp = sum(tempos_resposta["VP"]) / len(tempos_resposta["VP"]) if tempos_resposta["VP"] else 0
    media_superaquecimento = sum(tempos_resposta["Superaquecimento"]) / len(tempos_resposta["Superaquecimento"]) if tempos_resposta["Superaquecimento"] else 0

    # Imprimir as médias
    print(f"Média Acelerador: {media_acelerador} us")
    print(f"Média Freio: {media_freio} us")
    print(f"Média AirBag: {media_airbag} us")
    print(f"Média Cinto: {media_cinto} us")
    print(f"Média Luz: {media_luz} us")
    print(f"Média Trava Motorista: {media_tm} us")
    print(f"Média Trava Passageiro: {media_tp} us")
    print(f"Média Vidro Motorista: {media_vm} us")
    print(f"Média Vidro Passageiro: {media_vp} us")
    print(f"Média Superaquecimento: {media_superaquecimento} us")
    

if __name__ == "__main__":
    # main1()
    main2()
