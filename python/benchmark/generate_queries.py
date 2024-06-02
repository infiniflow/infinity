import random
import os
import re
from collections import Counter
import logging


def generate_terms_txt(src_fp, dst_fp):
    word_counts = Counter()
    i = 0
    with open(src_fp, 'r', encoding='utf-8', errors='ignore') as file:
        for line in file:
            # find pure alphabet words
            words = re.findall(r'\b[a-zA-Z]+\b', line)
            words = [x.lower() for x in words]
            word_counts.update(words)
            i += 1
    word_counts = Counter({key: value for key, value in word_counts.items(
    ) if value >= 100})  # remove words which are very very rare
    logging.info(
        f"read {i} lines {len(word_counts)} words from {src_fp} to generate terms file {dst_fp}")
    sorted_words = word_counts.most_common()        # in descending order
    with open(dst_fp, 'w', encoding='utf-8') as file:
        for word, count in sorted_words:
            file.write(f'{word} {count}\n')


def load_terms(terms_path):
    data = []
    with open(terms_path, 'r', encoding='utf-8', errors='ignore') as file:
        for line in file:
            columns = line.split()
            data.append(columns[0])
    return data


def generate_query_txt(dataset_path, terms_path, query_path, query_cnt=100000):
    if not os.path.exists(terms_path):
        generate_terms_txt(dataset_path, terms_path)
    terms = load_terms(terms_path)
    probabilities = [0.03, 0.15, 0.25, 0.25, 0.15, 0.08, 0.04, 0.03, 0.02]
    with open(query_path, 'w') as file:
        for i in range(query_cnt):
            terms_count = 1 + \
                random.choices(population=range(
                    len(probabilities)), weights=probabilities, k=1)[0]
            query_terms = random.sample(terms, terms_count)
            query = " ".join(query_terms)
            file.write(query + '\n')
    for terms_count in range(2, 20):
        with open(f"{query_path}.{terms_count}", 'w') as file:
            for i in range(query_cnt):
                query_terms = random.sample(terms, terms_count)
                query = " ".join(query_terms)
                file.write(query + '\n')
