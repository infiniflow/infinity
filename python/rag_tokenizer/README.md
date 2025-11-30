# rag-tokenizer

# Upload to pypi.org 

```shell
uv build;
uv publish --token $PYPI_API_TOKEN
```

# Install
```shell
pip install rag-tokenizer
```

# Usage

```python
import rag_tokenizer
tks1 = rag_tokenizer.tokenize("Hello world!")
print(tks1)
tks2 = rag_tokenizer.fine_grained_tokenize(tks1)
print(tks2)
```