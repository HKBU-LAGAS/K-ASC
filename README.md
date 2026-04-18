# KASC

Code for the paper titled "Efficient Swing Computation for Retrieval in Large-Scale
Recommender Systems" (SIGMOD Submission ID: 642)

## Environment

- System: Oracle Linux 8.8 (64-bit)
- Compiler: gcc 8.5.0

## Datasets

Download them from [here](https://)

## Generating queries

```python
$ cd data/
$ python genseed.py Gowalla # python data_name
```

## Running

```shell
$ sh build.sh
$ ./main -g Gowalla -a KASC
```
