import json
import csv
import random
import os

def load_terms(file_path):
    data = []
    _, ext = os.path.splitext(file_path)
    if ext == '.csv':
        with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
            reader = csv.reader(file, delimiter='\t')
            line_count = 0
            for row in reader:
                if line_count >= 100:
                    break
                if len(row) >= 3:
                    body = row[2]
                    terms = body.split()
                    data.extend(terms)
                    line_count += 1
    elif ext == '.txt':
        with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
            for line in file:
                columns = line.split()
                if columns[0] == "HighTerm:":
                    continue
                if "'" in columns[1]:
                    continue
                data.append(columns[1])
    return data

def random_select(data, n):
    return random.sample(data, n)

def generate_query_txt(terms_path, query_cnt = 1, terms_count = 4, operation_path="datasets/enwiki/operations.txt"):
    querys = []
    for i in range(query_cnt):
        new_value = ""
        terms = load_terms(terms_path)
        query_terms = random_select(terms, terms_count)
        new_value = " ".join(query_terms)
        querys.append(new_value)
        # print(f"generate terms = {new_value}")
        # write new value to file operation_path
    with open(operation_path, 'w') as file:
        for query in querys:
            file.write(query + '\n')


def generate_query_json(terms_path, terms_count = 10, operation_path="datasets/enwiki/operations.json"):
    data = [
        {
            "name": "term",
            "body": {
                "query": {
                    "match": {
                        "body": ""
                    }
                }
            }
        }
    ]

    # new_value = "Chicago culture"
    new_value = ""
    terms = load_terms(terms_path)
    query_terms = random_select(terms, terms_count)
    new_value = " ".join(query_terms)
    data[0]["body"]["query"]["match"]["body"] = new_value
    print(f"generate terms = {new_value}")

    with open(operation_path, "w") as json_file:
        json.dump(data, json_file, indent=2)
# if __name__ == "__main__":
#     generate_query_json("datasets/enwiki/enwiki-10.csv", 10, "datasets/enwiki/operations1.json")