import json
import csv
import random

def load_terms(file_path):
    data = []
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
    return data

def random_select(data, n):
    return random.sample(data, n)

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